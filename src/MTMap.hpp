#pragma once

#include "Map.hpp"
#include "convert.hpp"
#include <string>
#include <cstdint>
#include <unordered_map>
#include <map>
#include <vector>
#include <set>
#include <mutex>

extern "C" {
#include "sqlite3.h"
}


class MCBlock;
class MTBlock;
typedef std::vector<MCBlock *> MCChunk;


struct MTNode {
	content_t param0;
	uint8_t param1, param2;
};


struct MTPos {
	int16_t x, y, z;
	bool operator == (const MTPos & o) const
		{ return x == o.x && y == o.y && z == o.z; }
};

/*
namespace std {
template<> struct hash<MTPos> {
	std::size_t operator () (const MTPos & p) const
		{ return (p.x + 0xFF) * (p.y + 0xFF00) * (p.z + 0xFF0000); }
};
}
*/

struct MTItemStack {
	MTItemStack() : count(0), wear(0), item(CONTENT_IGNORE) {}
	uint16_t count;
	uint16_t wear;
	content_t item;
};


struct MTInventory {
	MTInventory(uint8_t width, std::vector<MTItemStack> &&items) :
		width(width), items(items) {}
	MTInventory(uint8_t width, const std::vector<MTItemStack> &items) :
		width(width), items(items) {}
	uint8_t width;
	std::vector<MTItemStack> items;
};

typedef std::map<std::string, MTInventory> MTInventoryList;

typedef std::map<std::string, std::string> MTNodeMetaStrings;

struct MTNodeMeta {
	MTNodeMeta() : meta(nullptr), inv(nullptr) {}
	MTNodeMeta(const MTNodeMetaStrings *meta, const MTInventoryList *inv,
			bool own_meta, bool own_inv) :
		own_meta(own_meta), own_inv(own_inv), meta(meta), inv(inv) {}
	~MTNodeMeta() {
		if (own_meta) delete meta;
		if (own_inv) delete inv;
	}
	bool own_meta, own_inv;
	const MTNodeMetaStrings *meta;
	const MTInventoryList *inv;
};


class MTMap {
public:
	MTMap(const std::string & path);
	~MTMap();

	void beginSave();
	void endSave();

	bool serializeBlock(std::string *data, const MTBlock & block);
	bool saveBlock(const MTPos &pos, const std::string &data);

	static content_t getId(const std::string & name);
	static const std::string & getName(content_t id) { return content_name[id]; }
	static content_t getNextContent() { return next_content; }

	static void reportUnknown(uint16_t id, uint8_t data)
	{
		std::lock_guard<std::mutex> l(unknown_lock);
		unknown_mc_names.insert(std::to_string(id) + ':' + std::to_string(data));
	}
	static void reportUnknown(const std::string &name, uint8_t data)
	{
		std::lock_guard<std::mutex> l(unknown_lock);
		unknown_mc_names.insert(name + ':' + std::to_string(data));
	}
	static void getUnknown(std::set<std::string> **names)
	{
		*names = &unknown_mc_names;
	}

private:
	std::string path;

	static std::map<std::string, content_t> content_id;
	static std::vector<std::string> content_name;
	static content_t next_content;

	static std::set<std::string> unknown_mc_names;
	static std::mutex unknown_lock;

	sqlite3 * db;
	sqlite3_stmt * stmt_begin;
	sqlite3_stmt * stmt_end;
	sqlite3_stmt * stmt_write;
};


class MTSector {
public:
	MTSector(const MCChunk & mcc);
	~MTSector();

	MTBlock *getBlock(int8_t y) { return blocks[y + BLOCK_Y_OFFSET]; }

	MTBlock *blocks[MC_MAP_HEIGHT / MAP_BLOCK_SIZE];
};


class MTBlock {
public:
	MTBlock(const MCBlock & mcb);
	~MTBlock();

	bool serialize(std::string * s) const;
	void finishBlock(MTSector *mts);

	MTPos pos;

	content_t content[NODES_PER_BLOCK];
	uint8_t param1[NODES_PER_BLOCK];
	uint8_t param2[NODES_PER_BLOCK];
	std::vector<std::pair<uint16_t, const MTNodeMeta *>> meta;

private:
	bool serializeNodes(std::string * data, uint16_t * content_sbi,
			std::vector<content_t> * sbi_content) const;
	bool serializeNodeMeta(std::string * data) const;
	void serializeInventoryList(std::string * data,
		const MTInventoryList * inv_list) const;

	std::vector<std::pair<uint16_t, ConversionCallback>> callbacks;
	std::vector<MTNodeMeta *> owned_meta;
};
