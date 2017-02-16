#include "convert.hpp"
#include "util.hpp"
#include "MTMap.hpp"
#include <cassert>
#include <cstring>
#include <iostream>

#define MC_ID_MAX 348
#define MC_DATA_MAX 15

static ConversionData conversion_table[MC_ID_MAX+1][MC_DATA_MAX+1] = {{{false, 0, CONTENT_IGNORE, nullptr}}};
static std::map<std::string, uint16_t> conversion_map;

static void add_conversion(uint16_t mc_id, const char *mc_name,
		const char *datas, const char *name, uint8_t param2, bool tool,
		ConversionCallback cb)
{
	assert(mc_id <= MC_ID_MAX);
	content_t cid = MTMap::getId(name);
	ConversionData cd {tool, param2, cid, cb};

	if (datas == nullptr) {
		for (uint16_t i = 0; i < MC_DATA_MAX+1; ++i) {
			conversion_table[mc_id][i] = cd;
		}
	} else {
		Tokenizer tok(datas);
		std::string data_str;
		while (tok.next(&data_str, ',')) {
			uint8_t data = std::stoi(data_str);
			assert(data <= MC_DATA_MAX);
			conversion_table[mc_id][data] = cd;
		}
	}
	conversion_map[mc_name] = mc_id;
}


bool get_conversion(const ConversionData **cd, uint16_t id, uint16_t data)
{
	if (id > MC_ID_MAX)
		return false;
	const ConversionData *p;
	if (data <= MC_DATA_MAX) {
		p = &(conversion_table[id][data]);
	} else {
		p = &(conversion_table[id][0]);
		assert(p->cid == CONTENT_IGNORE || p->tool);
	}
	*cd = p;
	return p->cid != CONTENT_IGNORE;
}


bool get_conversion(const ConversionData **cd, const std::string &name, uint16_t data)
{
	auto it = conversion_map.find(name);
	if (it == conversion_map.end())
		return false;
	return get_conversion(cd, it->second, data);
}


// Updates lighting, particularly for nodebox nodes like stairs and slabs,
// which MC seems to always store with a light value of 0.
// This simply tries to grab a light value from the adjacent nodes.
// The algorithm is imperfect, but it works very well in almost all cases.
// TODO: Check other blocks in sector.
static void update_node_light(MTSector *, MTBlock *block, uint16_t idx)
{
	static const int16_t adjacent[] = {
		BLOCK_YSTRIDE, -BLOCK_YSTRIDE,
		BLOCK_XSTRIDE, -BLOCK_XSTRIDE,
		BLOCK_ZSTRIDE, -BLOCK_ZSTRIDE,
	};
	// Maximum light values of adjacent nodes with sun/without sun
	uint8_t max_l_s = 0, max_l_n = 0;
	for (unsigned i = 0; i < ARRAY_SIZE(adjacent); ++i) {
		const int16_t adj_idx = idx + adjacent[i];
		if (adj_idx < 0 || adj_idx >= NODES_PER_BLOCK) {
			// Assume that nodes out of range are LIGHT_SUN during the day
			max_l_s = LIGHT_MAX;
			continue;
		}
		const uint8_t l = block->param1[adj_idx];
		// Must be signed or below could underflow
		int8_t l_s = (l & 0x0F) - 1;
		int8_t l_n = ((l & 0xF0) >> 4) - 1;
		if (l_s > max_l_s) max_l_s = l_s;
		if (l_n > max_l_n) max_l_n = l_n;
		if (max_l_s + max_l_n >= 15*2) break;
	}
	block->param1[idx] = (max_l_n << 4) | max_l_s;
}


// As of MC 1.2, the top half of the door doesn't contain
// orientation data, it must be obtained from the bottom half.
static const MTNodeMetaStrings door_ms_right {{"right", "1"}};
static const MTNodeMeta door_meta_right(&door_ms_right, nullptr, false, false);

#if MC_VERSION < 102

static void old_door_set_right(MTSector *, MTBlock *block, uint16_t idx)
{
	block->meta.emplace_back(idx, &door_meta_right);
}

#else // else if MC_VERSION >= 102

static void finish_door(MTSector *sector, MTBlock *block, uint16_t idx)
{
	MTBlock *top_block = block;
	uint16_t top_idx = idx + BLOCK_YSTRIDE;
	if (top_idx >= NODES_PER_BLOCK) {
		top_block = sector->getBlock(block->pos.y + 1);
		assert(top_block != nullptr);
		// Top section is at (bottom_x, 0, bottom_z) in top block
		top_idx = idx & (BLOCK_IDX_MASK_X | BLOCK_IDX_MASK_Z);
	}

	uint8_t top_param2 = top_block->param2[top_idx];
	uint8_t bottom_param2 = block->param2[idx];

	bool open = bottom_param2 & 4;
	int8_t dir = bottom_param2 & 3; // Dir is in MT format
	bool hinge_right = !(top_param2 & 1);

	bool door_type = false;

	if (hinge_right) {
		door_type = !door_type;
		dir += 2;
		block->meta.emplace_back(idx, &door_meta_right);
		block->meta.emplace_back(top_idx, &door_meta_right);
	}

	if (open) {
		door_type = !door_type;
		dir += hinge_right ? -1 : 1;
	}

	if (dir > 3) dir -= 4;
	if (dir < 0) dir += 4;

	std::string door_name = MTMap::getName(block->content[idx]);
	door_name = door_name.substr(0, door_name.size() - 4);  // Remove _t_1/_b_2/etc
	content_t c_top, c_bottom;

	if (door_type) {
		c_top = MTMap::getId(door_name + "_t_2");
		c_bottom = MTMap::getId(door_name + "_b_2");
	} else {
		c_top = MTMap::getId(door_name + "_t_1");
		c_bottom = MTMap::getId(door_name + "_b_1");
	}

	block->content[idx] = c_bottom;
	block->param2[idx] = dir;
	top_block->content[top_idx] = c_top;
	top_block->param2[top_idx] = dir;
}

#endif  // MC_VERSION < 102


void convert_inventory(const NBT::List &be_items, std::vector<MTItemStack> &inv_items)
{
	for (uint32_t i = 0; i < be_items.size; ++i) {
		NBT::Compound & be_item = be_items.value[i];

		uint8_t slot = be_item["Slot"].as<NBT::Byte>();
		assert(slot < inv_items.size());
		MTItemStack & item = inv_items[slot];;

		// Used for wear *and* node data field
		NBT::Short data = be_item["Damage"];

		NBT::Tag id = be_item["id"];
		const ConversionData *cd;

		if (id.type == NBT::TagType::Short) {
			uint16_t id_i = id.as<NBT::Short>();
			if (!get_conversion(&cd, id_i, data)) {
				MTMap::reportUnknown(id_i, data);
				continue;
			}
		} else if (id.type == NBT::TagType::String) {
			NBT::String name(id);
			std::string name_s(name.value, name.size);
			if (!get_conversion(&cd, name_s, data)) {
				MTMap::reportUnknown(name_s, data);
				continue;
			}
		} else {
			std::cerr << "Warning: Unexpected item id type: "
				<< (int)id.type << ' ' << id.dump() << std::endl;
			continue;
		}

		item.item = cd->cid;
		item.count = be_item["Count"].as<NBT::Byte>();
		if (cd->tool)
			item.wear = data;
	}
}


std::pair<bool, MTNodeMeta*> convert_chest(const NBT::Tag &te)
{
	static const MTNodeMetaStrings meta = {
		{"infotext", "Chest"},
		{"formspec", "size[8,9]"
			"list[current_name;main;0,0;8,4;]"
			"list[current_player;main;0,5;8,4;]"},
	};

	NBT::Compound &te_map = te;
	std::vector<MTItemStack> items(32, MTItemStack());
	auto it = te_map.find("Items");
	if (it != te_map.end())
		convert_inventory(it->second, items);

	auto * inv = new MTInventoryList {
		{"main", MTInventory(8, std::move(items))},
	};
	return std::make_pair(true, new MTNodeMeta(&meta, inv, false, true));
}


std::pair<bool, MTNodeMeta*> convert_furnace(const NBT::Tag &)
{
	static MTNodeMetaStrings fields = {
		{"infotext", "Furnace out of fuel"},
		{"formspec", "size[8,9]"
			"image[2,2;1,1;default_furnace_fire_bg.png]"
			"list[current_name;fuel;2,3;1,1;]"
			"list[current_name;src;2,1;1,1;]"
			"list[current_name;dst;5,1;2,2;]"
			"list[current_player;main;0,5;8,4;]"},
		{"src_totaltime", "0"},
		{"src_time", "0"},
		{"fuel_totaltime", "0"},
		{"fuel_time", "0"}
	};
	// TODO: Convert inventory.
	static const std::vector<MTItemStack> inv_fuel(1, MTItemStack());
	static const std::vector<MTItemStack> inv_src(1, MTItemStack());
	static const std::vector<MTItemStack> inv_dst(4, MTItemStack());
	static MTInventoryList inv = {
		{"fuel", MTInventory(1, inv_fuel)},
		{"src", MTInventory(1, inv_src)},
		{"dst", MTInventory(2, inv_dst)},
	};
	static MTNodeMeta meta(&fields, &inv, false, false);
	return std::make_pair(false, &meta);
}


std::pair<bool, MTNodeMeta*> convert_sign(const NBT::Tag &te)
{
	std::string text;
	for (unsigned i = 1; i < 5; ++i) {
		NBT::String line = te["Text" + std::to_string(i)];
		if (!line.size)
			continue;
		std::string sline(line.value, line.size);
		rtrim(sline);
		if (!sline.empty())
			text += sline + " ";
	}
	rtrim(text);
	auto * fields = new MTNodeMetaStrings {
		{"infotext", '"'+text+'"'},
		{"text", text},
		{"formspec", "field[text;;${text}]"},
	};
	return std::make_pair(true, new MTNodeMeta(fields, nullptr, true, false));
}


std::map<std::string, BlockEntityCB> be_convert {
	{"Chest", convert_chest},
	{"Furnace", convert_furnace},
	{"Sign", convert_sign},
};

static void init_conversion_table()
{
	#include "conversions.h"
}

void init_conversions()
{
	static bool conversions_initialized = false;
	if (conversions_initialized)
		return;
	conversions_initialized = true;

	init_conversion_table();
}
