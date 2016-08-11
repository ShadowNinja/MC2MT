#include "MCMap.hpp"
#include "MTMap.hpp"
#include "threads.hpp"
#include <iostream>

// These are selected rather arbitrarily
#define SAVE_QUEUE_MAX (1 << 10)
#define SAVE_QUEUE_MIN (1 << 8)


LockedQueue<MCGroup*> convert_queue;
std::atomic<size_t> groups_done;
std::atomic<size_t> blocks_done;
static bool g_finished;
static std::vector<std::thread> threads;
static LockedQueue<std::pair<MTPos, std::string*>> save_queue;


static void convert_thread(MCMap *mc_map, MTMap *mt_map)
{
	MCGroup *group;
	MCChunk chunk;
	while (convert_queue.pop(group)) {
		MCChunkList chunks;
		mc_map->listChunks(group, &chunks);
		for (const auto & chunk_pos : chunks) {
			mc_map->loadChunk(&chunk, *group, chunk_pos);
			MTSector mts(chunk);
			for (unsigned i = 0; i < ARRAY_SIZE(mts.blocks); ++i) {
				MTBlock *mtb = mts.blocks[i];
				if (mtb == nullptr)
					continue;
				std::string *data = new std::string;
				if (mt_map->serializeBlock(data, *mtb)) {
					save_queue.push(std::make_pair(mtb->pos, data));
				} else {
					std::cerr << "Failed to serialize block: "
						<< *data << std::endl;
					delete data;
				}
			}
			blocks_done += chunk.size();
			chunk.clear();
		}
		delete group;
		++groups_done;
	}
}


static void save_thread(MTMap * map)
{
	// Whether we've exclusively locked the queue to pause the conversion threads
	bool locked = false;
	std::pair<MTPos, std::string*> item;
	using timer = std::chrono::steady_clock;
	timer::time_point t = timer::now();

	map->beginSave();
	while (true) {
		timer::time_point now = timer::now();
		if (now - t > std::chrono::seconds(1)) {
			map->endSave();
			map->beginSave();
			t = now;
		}

		if (locked) {
			if (save_queue.q.empty()) {
				save_queue.m.unlock();
				locked = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			item = save_queue.q.front();
		} else {
			if (!save_queue.pop(item)) {
				if (g_finished)
					break;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
		}

		map->saveBlock(item.first, *(item.second));
		delete item.second;

		if (locked) {
			save_queue.q.pop();
			if (save_queue.q.size() < SAVE_QUEUE_MIN) {
				save_queue.m.unlock();
				locked = false;
			}
		} else if (save_queue.size() > SAVE_QUEUE_MAX) {
			// If we start running behind, keep the queue
			// locked to pause the producer threads.
			save_queue.m.lock();
			locked = true;
		}
	}
	map->endSave();
}


void init_threads(MCMap *mc_map, MTMap *mt_map)
{
	g_finished = false;
	size_t native_threads = std::thread::hardware_concurrency();
	if (convert_queue.size() < native_threads)
		native_threads = convert_queue.size();
	std::cerr << "Using " << native_threads << " conversion threads and 1 save thread." << std::endl;
	for (unsigned i = 0; i < native_threads; ++i)
		threads.emplace_back(convert_thread, mc_map, mt_map);
	threads.emplace_back(save_thread, mt_map);
}

void deinit_threads()
{
	g_finished = true;
	for (auto & th : threads)
		th.join();
}
