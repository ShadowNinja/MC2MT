#pragma once

#include "Map.hpp"
#include "nbt.hpp"

#define MC_VERSION 109

struct MTNodeMeta;
class MTSector;
class MTBlock;

typedef void (*ConversionCallback)(MTSector *sector, MTBlock *block, uint16_t idx);

struct ConversionData {
	bool tool;
	uint8_t param2;
	content_t cid;
	ConversionCallback cb;
};


bool get_conversion(const ConversionData **cd, uint16_t id, uint16_t data);
bool get_conversion(const ConversionData **cd, const std::string &name, uint16_t data);

typedef std::pair<bool, MTNodeMeta *> (*BlockEntityCB)(const NBT::Tag &);
extern std::map<std::string, BlockEntityCB> be_convert;


void init_conversions();
