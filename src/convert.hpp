#pragma once

#include "Map.hpp"
#include "nbt.hpp"

#define MC_VERSION 109

struct MTItemStack;
struct MTNodeMeta;
class MTSector;
class MTBlock;

typedef void (*NodeConversionCallback)(MTSector *sector, MTBlock *block, uint16_t idx);
typedef void (*ItemConversionCallback)(MTItemStack &item, NBT::Tag &mc_item);

union ConversionCallback {
	ConversionCallback() = default;
	ConversionCallback(std::nullptr_t cb) : node(cb) {}
	ConversionCallback(NodeConversionCallback cb) : node(cb) {}
	ConversionCallback(ItemConversionCallback cb) : item(cb) {}

	NodeConversionCallback node;
	ItemConversionCallback item;
};

struct ConversionData {
	ConversionData() : cb(nullptr), cid(CONTENT_IGNORE), param2(0), flags(0) {}
	ConversionData(ConversionCallback cb, content_t cid, uint8_t param2, uint8_t flags) :
		cb(cb), cid(cid), param2(param2), flags(flags) {}

	ConversionCallback cb;
	content_t cid;
	uint8_t param2;
	uint8_t flags;
};

#define CD_FLAG_TOOL 1
#define CD_FLAG_ITEM_CB 2


bool get_conversion(const ConversionData **cd, uint16_t id, uint16_t data);
bool get_conversion(const ConversionData **cd, const std::string &name, uint16_t data);

typedef std::pair<bool, MTNodeMeta *> (*BlockEntityCB)(const NBT::Tag &);
extern std::map<std::string, BlockEntityCB> be_convert;


void init_conversions();
