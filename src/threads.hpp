#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>

class MCMap;
class MTMap;

void init_threads(MCMap *mc_map, MTMap *mt_map);
void deinit_threads();

#define Q_SCOPE_LOCK std::lock_guard<std::mutex> l(m)
template <typename T>
class LockedQueue {
public:
	std::queue<T> q;
	std::mutex m;
	void push(const T &x) { Q_SCOPE_LOCK; q.push(x); }
	bool pop(T &x)
	{
		Q_SCOPE_LOCK;
		if (q.empty())
			return false;
		x = q.front();
		q.pop();
		return true;
	}
	size_t size() { Q_SCOPE_LOCK; return q.size(); }
};
#undef LOCK_Q

extern LockedQueue<MCGroup*> convert_queue;
extern std::atomic<size_t> groups_done;
extern std::atomic<size_t> blocks_done;
