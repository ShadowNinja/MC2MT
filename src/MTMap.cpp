#include "MTMap.hpp"
#include "MCMap.hpp"
#include "util.hpp"
#include "serialization.hpp"
#include "compression.hpp"

#include <iostream>
#include <memory>
#include <cassert>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;


#define SER_FMT_VER_HIGHEST_WRITE 25


#define SQLRES(s, res, msg) \
	if ((s) != (res)) { \
		throw std::runtime_error(std::string( \
				"SQLite3: error in " msg " (" \
				__FILE__ ":" TOSTRING(__LINE__) \
				"): ") + \
			sqlite3_errmsg(db)); \
	}

#define SQLOK(s, msg) SQLRES(s, SQLITE_OK, msg)

#define PREPARE_STATEMENT(name, query) \
	SQLOK(sqlite3_prepare_v2(db, query, -1, &stmt_##name, NULL), \
			"preparing statement " TOSTRING(name))

#define FINALIZE_STATEMENT(name) \
	SQLOK(sqlite3_finalize(stmt_##name), "finalizing statement " #name)

#define MAKE_STR_WRITER(T) \
	inline void write##T(std::string & str, NBT::U##T val) \
	{ \
		char buf[sizeof(NBT::T)]; \
		NBT::write##T(reinterpret_cast<NBT::UByte *>(buf), val); \
		str.append(buf, sizeof(buf)); \
	}

#define writeByte(str, val) (str).push_back(static_cast<char>(val))

MAKE_STR_WRITER(Short)
MAKE_STR_WRITER(Int)
MAKE_STR_WRITER(Long)

#define writeString(str, s)     writeShort(str, (s).size()); (str).append(s)
#define writeLongString(str, s) writeInt  (str, (s).size()); (str).append(s)


MTMap::MTMap(const std::string & path) :
	path(path)
{
	init_conversions();

	if (!fs::is_directory(path) && !fs::create_directories(path))
		throw std::runtime_error("Failed to create database save "
				"directory: " + path);

	sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);

	SQLOK(sqlite3_open_v2((path + DIR_DELIM "map.sqlite").c_str(), &db,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
			nullptr), "opening database");

	SQLOK(sqlite3_exec(db,
		"CREATE TABLE IF NOT EXISTS `blocks` (\n"
		"	`pos` INT PRIMARY KEY,\n"
		"	`data` BLOB\n"
		");\n",
		nullptr, nullptr, nullptr),
		"creating schema");

	PREPARE_STATEMENT(begin, "BEGIN");
	PREPARE_STATEMENT(end, "COMMIT");
	PREPARE_STATEMENT(write,  "INSERT INTO `blocks` (`pos`, `data`) VALUES (?, ?)");
}


void MTMap::beginSave()
{
	SQLRES(sqlite3_step(stmt_begin), SQLITE_DONE, "starting transaction");
	sqlite3_reset(stmt_begin);
}


void MTMap::endSave()
{
	SQLRES(sqlite3_step(stmt_end), SQLITE_DONE, "ending transaction");
	sqlite3_reset(stmt_end);
}


constexpr int64_t encodePos(const MTPos & p)
{
	return static_cast<uint64_t>(p.z) * 0x1000000 +
		static_cast<uint64_t>(p.y) * 0x1000 +
		static_cast<uint64_t>(p.x);
}


bool MTMap::serializeBlock(std::string *data, const MTBlock & block)
{
	uint8_t version = SER_FMT_VER_HIGHEST_WRITE;
	writeByte(*data, version);
	return block.serialize(data);
}


bool MTMap::saveBlock(const MTPos &pos, const std::string &data)
{
	SQLOK(sqlite3_bind_int64(stmt_write, 1, encodePos(pos)),
			"binding position for save");
	SQLOK(sqlite3_bind_blob(stmt_write, 2, data.data(), data.size(), nullptr),
			"binding block BLOB for save");

	SQLRES(sqlite3_step(stmt_write), SQLITE_DONE, "saving block");
	sqlite3_reset(stmt_write);

	return true;
}


MTMap::~MTMap()
{
	FINALIZE_STATEMENT(begin);
	FINALIZE_STATEMENT(end);
	FINALIZE_STATEMENT(write);

	SQLOK(sqlite3_close(db), "closing database");
}


std::map<std::string, content_t> MTMap::content_id = {
	{"ignore", CONTENT_IGNORE},
	{"air", CONTENT_AIR},
};
std::vector<std::string> MTMap::content_name = {"ignore", "air"};
content_t MTMap::next_content = CONTENT_FIRST;

content_t MTMap::getId(const std::string &name)
{
	auto it = content_id.find(name);
	if (it != content_id.end())
		return it->second;
	content_id[name] = next_content;
	content_name.push_back(name);
	return next_content++;
}


std::set<std::string> MTMap::unknown_mc_names;
std::mutex MTMap::unknown_lock;


/************
 * MCSector *
 ************/

MTSector::MTSector(const MCChunk & mcc)
{
	memset(blocks, 0, sizeof(blocks));
	for (const MCBlock* mcb : mcc) {
		assert(mcb->pos.y < ARRAY_SIZE(blocks));
		blocks[mcb->pos.y] = new MTBlock(*mcb);
		delete mcb;
	}
	for (unsigned i = 0; i < ARRAY_SIZE(blocks); ++i) {
		if (blocks[i])
			blocks[i]->finishBlock(this);
	}
}


MTSector::~MTSector()
{
	for (unsigned i = 0; i < ARRAY_SIZE(blocks); ++i) {
		delete blocks[i];
	}
}


/***********
 * MTBlock *
 ***********/

MTBlock::MTBlock(const MCBlock & mcb)
{
	// Make sure positions fit into 16 bits
	assert(mcb.pos.x <= INT16_MAX && mcb.pos.x >= INT16_MIN &&
			mcb.pos.z <= INT16_MAX && mcb.pos.z >= INT16_MIN);

	// Shift the position down four blocks (64 nodes) to
	// align MC water level (64) with MT water level (0).
	pos = {(int16_t)mcb.pos.x, (int16_t)(mcb.pos.y - BLOCK_Y_OFFSET), (int16_t)mcb.pos.z};

	const ConversionData default_cd = {false, 0, CONTENT_AIR, nullptr};

	// Load all the nodes in the 16x16x16 block
	for (uint16_t i = 0; i < NODES_PER_BLOCK; ++i) {
		const ConversionData *cd;
		if (!get_conversion(&cd, mcb.blocks[i], mcb.data[i])) {
			MTMap::reportUnknown(mcb.blocks[i], mcb.data[i]);
			cd = &default_cd;
		}

		if (cd->cb) {
			callbacks.emplace_back(i, cd->cb);
		}

		content[i] = cd->cid;
		param2[i] = cd->param2;
		// Day light in lower half, night light in upper half
		param1[i] = (mcb.block_light[i] << 4) |
			std::max(mcb.block_light[i], mcb.sky_light[i]);
	}

	for (const auto & te : mcb.tile_entities) {
		NBT::String id = te["id"];
		auto it = be_convert.find(std::string(id.value, id.size));
		if (it == be_convert.end())
			continue;

		auto converter = it->second;

		int32_t x = (NBT::Int)te["x"];
		int32_t y = (NBT::Int)te["y"];
		int32_t z = (NBT::Int)te["z"];
		auto ret = converter(te);
		meta.emplace_back(BLOCK_NODE_IDX(x & 0xF, y & 0xF, z & 0xF), ret.second);
		if (ret.first)
			owned_meta.push_back(ret.second);
	}
}


MTBlock::~MTBlock()
{
	for (const auto ptr : owned_meta) {
		delete ptr;
	}
}


void MTBlock::finishBlock(MTSector *mts)
{
	for (const auto &cb_data : callbacks) {
		cb_data.second(mts, this, cb_data.first);
	}
	callbacks.clear();
}


bool MTBlock::serialize(std::string * data) const
{
	// Write header (flags, content_width, params_width)
	uint8_t flags = 0;
	if (pos.y < -1)
		flags |= 0x01;  // is_underground
	flags |= 0x02;  // day_night_differs
	writeByte(*data, flags);

	writeByte(*data, 2);  // content_width
	writeByte(*data, 2);  // params_width

	// Map from global content IDs to serialized block IDs.
	std::unique_ptr<uint16_t[]> content_sbi(
			new uint16_t[MTMap::getNextContent()]);
	memset(content_sbi.get(), 0xFF, MTMap::getNextContent() *
			sizeof(uint16_t));

	// For serialized node -> ID map.  Keys are block-specific node IDs,
	// values are global content IDs.
	std::vector<content_t> sbi_content;

	// Write nodes.  Fills in content structures.
	if (!serializeNodes(data, content_sbi.get(), &sbi_content))
		return false;


	// Write node meta
	if (!serializeNodeMeta(data))
		return false;


	// Static objects
	writeByte(*data, 0);  // Version
	writeShort(*data, 0);  // Number of objects


	// Timestamp
	writeInt(*data, 0xFFFFFFFF);  // BLOCK_TIMESTAMP_UNDEFINED

	// Name-ID mapping
	writeByte(*data, 0);  // Version
	writeShort(*data, sbi_content.size());
	for (uint_fast16_t i = 0; i < sbi_content.size(); ++i) {
		writeShort(*data, i);
		writeString(*data, MTMap::getName(sbi_content[i]));
	}


	// Node timers
	writeByte(*data, 2 + 4 + 4);  // Timer data len
	writeShort(*data, 0);  // Number of timers

	return true;
}


bool MTBlock::serializeNodes(std::string * data,
		uint16_t * content_sbi,
		std::vector<content_t> * sbi_content) const
{
	// There are two types of node IDs: Content IDs and serialized block IDs.
	// Content IDs are used by the map so it doesn't have to distinguish
	// between blocks, but blocks serialize their own IDs refering only to
	// the nodes in the block to save space.

	// Buffer to write nodes to for compression
	std::string cbuffer;

	// XXX: Remove?
	uint16_t k = 0;  // To change axis order fron YZX to ZYX

	uint16_t next_cid = 0;

	// Write node content IDs (aka param0)
	for (uint_fast8_t z = 0; z < MAP_BLOCK_SIZE; ++z) {
	for (uint_fast8_t y = 0; y < MAP_BLOCK_SIZE; ++y) {
	for (uint_fast8_t x = 0; x < MAP_BLOCK_SIZE; ++x) {
		content_t c = content[k];
		uint16_t block_cid = content_sbi[c];
		if (block_cid != 0xFFFF) {
			writeShort(cbuffer, static_cast<NBT::UShort>(block_cid));
		} else {
			content_sbi[c] = next_cid;
			sbi_content->push_back(c);
			writeShort(cbuffer, next_cid);
			++next_cid;
		}
		++k;
	}
	k += BLOCK_YSTRIDE - BLOCK_ZSTRIDE;
	}
	k += BLOCK_ZSTRIDE - MAP_BLOCK_SIZE * BLOCK_YSTRIDE;
	}
	k = 0;

	// Write param 1 (light)
	for (uint_fast8_t z = 0; z < MAP_BLOCK_SIZE; ++z) {
		for (uint_fast8_t y = 0; y < MAP_BLOCK_SIZE; ++y) {
			for (uint_fast8_t x = 0; x < MAP_BLOCK_SIZE; ++x)
				writeByte(cbuffer, param1[k++]);
			k += BLOCK_YSTRIDE - BLOCK_ZSTRIDE;
		}
		k += BLOCK_ZSTRIDE - MAP_BLOCK_SIZE * BLOCK_YSTRIDE;
	}
	k = 0;

	// Write param 2 (facedir, et.all.)
	for (uint_fast8_t z = 0; z < MAP_BLOCK_SIZE; ++z) {
		for (uint_fast8_t y = 0; y < MAP_BLOCK_SIZE; ++y) {
			for (uint_fast8_t x = 0; x < MAP_BLOCK_SIZE; ++x)
				writeByte(cbuffer, param2[k++]);
			k += BLOCK_YSTRIDE - BLOCK_ZSTRIDE;
		}
		k += BLOCK_ZSTRIDE - MAP_BLOCK_SIZE * BLOCK_YSTRIDE;
	}

	// Compress nodes and add then to the main buffer
	return NBT::compress(data, cbuffer.data(), cbuffer.size());
}


bool MTBlock::serializeNodeMeta(std::string * data) const
{
	std::string cbuffer;
	if (meta.empty()) {
		writeByte(cbuffer, 0);
		goto finish;
	}
	writeByte(cbuffer, 1);  // Version
	writeShort(cbuffer, meta.size());
	for (const auto & pos_and_data : meta) {
		const uint16_t & idx = pos_and_data.first;
		const auto meta_strs = pos_and_data.second->meta;
		const auto inv_list = pos_and_data.second->inv;
		writeShort(cbuffer, (BLOCK_IDX_Z(idx) * MAP_BLOCK_SIZE * MAP_BLOCK_SIZE) +
				(BLOCK_IDX_Y(idx) * MAP_BLOCK_SIZE) + BLOCK_IDX_X(idx));
		writeInt(cbuffer, meta_strs->size());
		for (std::map<std::string, std::string>::const_iterator it = meta_strs->begin();
				it != meta_strs->end(); ++it) {
			writeString(cbuffer, it->first);
			writeLongString(cbuffer, it->second);
		}
		serializeInventoryList(&cbuffer, inv_list);
	}
finish:
	return NBT::compress(data, cbuffer.data(), cbuffer.size());
}


void MTBlock::serializeInventoryList(std::string * data,
		const MTInventoryList * inv_list) const
{
	if (inv_list == nullptr)
		goto end_inv;
	for (const auto & inv : *inv_list) {
		*data += "List " + inv.first + ' ';
		*data += std::to_string(inv.second.items.size()) + '\n';
		*data += "Width " + std::to_string(inv.second.width) + '\n';
		for (const MTItemStack & item : inv.second.items) {
			if (item.count == 0) {
				*data += "Empty\n";
				continue;
			}
			*data += "Item ";
			*data += MTMap::getName(item.item);
			*data += ' ';
			*data += std::to_string(item.count);
			// TODO: Item meta?
			*data += '\n';
		}
		// "EndInventoryList" ends Inventories, "EndInventory" ends InventoryLists.
		*data += "EndInventoryList\n";
	}
end_inv:
	*data += "EndInventory\n";
}
