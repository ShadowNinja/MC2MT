#include "MCMap.hpp"
#include "util.hpp"

#include "nbt.hpp"
#include "serialization.hpp"
#include "compression.hpp"
#include "Map.hpp"

#include <cstring>
#include <fstream>
#include <iostream>
#include <csignal>
#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;


#define CHUNK_OFS_POS(x, z) ((x & 0x1F) + (z & 0x1F) * 32) * 4


MCMap::MCMap(const std::string & path) :
	path(path)
{
	std::ifstream f(path + DIR_DELIM "level.dat",
			std::ios::in | std::ios::binary);
	std::string data;
	char buf[4096];
	while (f.good()) {
		auto count = f.readsome(buf, 4096);
		if (count == 0) break;
		data.append(buf, count);
	}
	std::string decomp;
	if (!NBT::decompress(&decomp, data.data(), data.size())) {
		std::cerr << decomp << std::endl;
		return;
	}
	meta.read(reinterpret_cast<const NBT::UByte *>(decomp.data()));
	std::cerr << "Level data: " << meta[""]["Data"].dump() << std::endl;
}


void MCMap::listGroups(std::vector<MCGroup*> & v)
{
	MCFormat format = MCFormat::Regions;
	using dir_iter = fs::directory_iterator;
	for (dir_iter it(path + DIR_DELIM "region"); it != dir_iter(); ++it) {
		std::string filename = it->path().filename().string();

		Tokenizer tok(filename);
		std::string x_s, z_s, ext;
		if (!tok.next(&ext, '.') || ext != "r") continue;
		if (!tok.next(&x_s, '.')) continue;
		if (!tok.next(&z_s, '.')) continue;
		if (!tok.next(&ext, '.')) continue;
		if (tok.next(&ext, '.')) continue;

		MCFormat this_format;
		if (ext == "mca") this_format = MCFormat::Anvil;
		else if (ext == "mcr") this_format = MCFormat::Regions;
		else continue;

		// Select latest format found
		if (this_format > format) {
			format = this_format;
			v.clear();
		} else if (this_format != format) {
			continue;
		}

		int32_t x = std::stoi(x_s), z = std::stoi(z_s);
		int32_t x_nodes = x * 32 * MAP_BLOCK_SIZE;
		int32_t z_nodes = z * 32 * MAP_BLOCK_SIZE;

		if (x_nodes > MAX_DISTANCE || x_nodes < -MAX_DISTANCE ||
				z_nodes > MAX_DISTANCE || z_nodes < -MAX_DISTANCE) {
			std::cerr << "Skiping group (" << x << ", " << z
					<< ") (Too far from origin)" << std::endl;
			continue;
		}

		v.push_back(new MCGroup(filename, x, z, format));
	}
}


void MCMap::listChunks(MCGroup * gid, MCChunkList * blocks)
{
	assert(gid->f == nullptr);

	gid->f = new std::ifstream(path + DIR_DELIM "region" DIR_DELIM +
		gid->name, std::ios::in | std::ios::binary);

	int32_t chunk_x = gid->x * 32,
		chunk_z = gid->z * 32;

	const char tmp_nul[3] = {0};
	char tmp[3];
	for (int32_t cx = chunk_x; cx < chunk_x + 32; ++cx)
	for (int32_t cz = chunk_z; cz < chunk_z + 32; ++cz) {
		gid->f->seekg(CHUNK_OFS_POS(cx, cz));
		gid->f->read(tmp, 3);
		if (memcmp(tmp, tmp_nul, 3) != 0) {
			blocks->push_back(MCChunkPos {cx, cz});
		}
	}
}


bool MCMap::loadChunk(MCChunk * chunk, const MCGroup & gid, MCChunkPos cp)
{
	assert(gid.f != nullptr);
	return readChunk(chunk, gid.f, cp, gid.format);
}


bool MCMap::readChunk(MCChunk * chunk, std::ifstream * f, MCChunkPos cp, MCFormat format)
{
	char tmp[4] = {0};
	std::streamoff ofs = CHUNK_OFS_POS(cp.x, cp.z);

	f->seekg(ofs);
	f->read(tmp+1, 3);
	// Offset in 4KiB sectors
	ofs = NBT::readInt(reinterpret_cast<const NBT::UByte *>(tmp)) << 12;
	if (ofs == 0)  // This chunk wasn't saved.
		return false;

	// Length in 4KiB sectors (rounded up)
	f->read(tmp, 1);
	uint16_t length_sectors = tmp[0];
	assert(length_sectors != 0);

	f->seekg(ofs);

	f->read(tmp, 4);
	uint32_t length = NBT::readInt(reinterpret_cast<const NBT::UByte *>(tmp));

	// This should be true, but sometimes it isn't...
	if ((length + 4) > ((uint32_t)length_sectors << 12)) {
		std::cerr << "WARNING: Chunk length mismatch.  "
			"length_sectors=" << length_sectors
			<< " length=" << length << std::endl;
	}

	f->read(tmp, 1);
	uint8_t compression_type = static_cast<uint8_t>(tmp[0]);

	// Only GZip and Zlib compression are specified, and in practice only ZLib is used
	if (compression_type > 2) {
		std::cerr << "WARNING: Ignoring chunk with unknown compression type." << std::endl;
		return false;
	}

	std::string compressed, data;
	compressed.resize(length - 1);
	f->read(&compressed[0], length - 1);

	NBT::decompress(&data, compressed.data(), compressed.size());

	NBT::Tag nbt_data(reinterpret_cast<const NBT::UByte *>(data.data()));
	NBT::Tag level = nbt_data[""]["Level"];

	switch (format) {
	case MCFormat::Anvil: {
		const NBT::List secs = level["Sections"];
		for (uint32_t i = 0; i < secs.size; ++i) {
			NBT::Tag & sec = secs.value[i];
			chunk->push_back(new MCBlock(level, cp,
				sec["Y"].as<NBT::Byte>(), format, sec));
		}
		break;
	}
	case MCFormat::Regions:
		for (unsigned y_slice = 0; y_slice < 8; ++y_slice) {
			chunk->push_back(new MCBlock(level, cp,
				y_slice, format));
		}
		break;
	}
	return true;
}


/***********
 * MCBlock *
 ***********/

MCBlock::MCBlock(const NBT::Tag & chunk, MCChunkPos cp,
		 uint8_t y_slice, MCFormat format, const NBT::Tag &sec)
{
	switch (format) {
	case MCFormat::Anvil:
		/* The X axis has to be inverted to convert to Minetest
		 * (the chunk location is at the L lower corner, so subtract
		 * one or there would be 2 chunks at 0).
		 */
		pos = {-cp.x-1, y_slice, cp.z};
		fromSection(sec);
		break;
	case MCFormat::Regions:
		pos = {cp.x, y_slice, cp.z};
		// No luck, we have to convert
		fromChunk(chunk, y_slice);
		break;
	}
	NBT::List tentities = chunk["TileEntities"];
	for (unsigned i = 0; i < tentities.size; ++i) {
		const NBT::Tag te = tentities.value[i];
		if ((te["y"].as<NBT::Int>() >> 4) == y_slice) {
			NBT::Tag t(te);  // Copy
			// Entity data stores it's own position information,
			// so has to be modified independently in addition
			// to other blocks.
			t["y"] = (t["y"].as<NBT::Int>() & 0xF) - 16;
			// Within the chunk X position has to be inverted
			// to convert to Minetest.
			if (format == MCFormat::Anvil)
				t["x"] = pos.x * MAP_BLOCK_SIZE + (MAP_BLOCK_SIZE-1) -
					t["x"].as<NBT::Int>() % 16;
			tile_entities.push_back(std::move(t));
		}
	}
}

// Short for GET_NBT_RAW_BYTE_ARRAY
#define GNBTRBA(val) reinterpret_cast<const uint8_t *>((val).as<const NBT::ByteArray>().value)

void MCBlock::fromSection(const NBT::Tag & section)
{
	/* Anvil format is YZX ((y * 16 + z) * 16 + x).
	 * Block data is actually 16-bits per data point (i.e., per node),
	 * but is split into 8-bit 'blocks', 4-bit 'add' (added to blocks to
	 * obtain full block ID), and 4-bit 'data' (like MT's param2).
	 *
	 * To convert Minecraft to Minetest coordinates you must invert
	 * the X order while leaving Y and Z the same.
	 */
	const NBT::Compound & sec = section;

	reverseXAxis(blocks, GNBTRBA(sec.at("Blocks")));

	// "Add" array is optional
	auto it = sec.find("Add");
	if (it != sec.end()) {
		uint8_t blocks_add[NODES_PER_BLOCK];
		expandHalfBytes(blocks_add, GNBTRBA(it->second));

		for (size_t i = 0; i < NODES_PER_BLOCK; ++i) {
			blocks[i] |= static_cast<uint16_t>(blocks_add[i]) << 8;
		}
	}

	// Data, sky light, and block light are 4-bit
	expandHalfBytes(data,        GNBTRBA(sec.at("Data")));
	expandHalfBytes(sky_light,   GNBTRBA(sec.at("SkyLight")));
	expandHalfBytes(block_light, GNBTRBA(sec.at("BlockLight")));
}


void MCBlock::fromChunk(const NBT::Tag & chunk, uint8_t y_slice)
{
	extractSlice         (blocks,      GNBTRBA(chunk["Blocks"]),     y_slice);
	extractSliceHalfBytes(data,        GNBTRBA(chunk["Data"]),       y_slice);
	extractSliceHalfBytes(sky_light,   GNBTRBA(chunk["SkyLight"]),   y_slice);
	extractSliceHalfBytes(block_light, GNBTRBA(chunk["BlockLight"]), y_slice);
}


/// Reverses X axis node order within each slice.
void MCBlock::reverseXAxis(uint16_t * data, const uint8_t * l)
{
	uint16_t data_key = 0;
	for (uint16_t y = 0; y < MAP_BLOCK_SIZE; ++y)
	for (uint16_t z = 0; z < MAP_BLOCK_SIZE; ++z)
	for (uint16_t x = 0; x < MAP_BLOCK_SIZE; ++x) {
		uint16_t i = (y << 8) | (z << 4) | ((MAP_BLOCK_SIZE-1) - x);
		data[data_key++] = l[i];
	}
}


/// Reverses X axis node order, and expands 4-bit sequences into 8-bit sequences.
void MCBlock::expandHalfBytes(uint8_t * data, const uint8_t * l)
{
	uint16_t data_key = 0;
	for (uint16_t y = 0; y < MAP_BLOCK_SIZE; ++y)
	for (uint16_t z = 0; z < MAP_BLOCK_SIZE; ++z)
	for (uint16_t x = 0; x < MAP_BLOCK_SIZE/2;  ++x) {
		int16_t i = (y << 7) | (z << 3) | (7 - x);
		uint8_t b = l[i];
		data[data_key++] = (b >> 4) & 0xF;
		data[data_key++] = b & 0xF;
	}
}


/// Changes order from XZY to YZX.
void MCBlock::extractSlice(uint16_t * data, const uint8_t * l, uint8_t y_slice)
{
	uint16_t key = y_slice << 4;
	uint16_t data_key = 0;
	// Change order from XZY to YZX
	for (uint8_t y = 0; y < MAP_BLOCK_SIZE; ++y) {
		for (uint8_t z = 0; z < MAP_BLOCK_SIZE; ++z) {
			for (uint8_t x = 0; x < MAP_BLOCK_SIZE; ++x) {
				data[data_key++] = l[key];
				key += 2048;
			}
			key = (key & 0x7FF) + 128;
		}
		key = (key & 0x7F) + 1;
	}
}


/// Changes order from XZY to YZX and expands 4-bit sequences into 8-bit sequences.
void MCBlock::extractSliceHalfBytes(uint8_t * data, const uint8_t * l,
		uint8_t y_slice)
{
	uint16_t key = y_slice << 3;
	uint16_t data_key_1 = 0;
	uint16_t data_key_2 = 256;  // One layer above the previous one
	// Change order from XZY to YZX
	for (uint8_t y = 0; y < MAP_BLOCK_SIZE; y += 2) {  // Two values of y at a time
		for (uint8_t z = 0; z < MAP_BLOCK_SIZE; ++z) {
			for (uint8_t x = 0; x < MAP_BLOCK_SIZE; ++x) {
				uint8_t b = l[key];
				data[data_key_1++] = b & 0xF;
				data[data_key_2++] = (b >> 4) & 0xF;
				key += 1024;
			}
			key = (key & 0x3FF) + 64;
		}
		key = (key & 0x3F) + 1;
		data_key_1 += BLOCK_YSTRIDE;  // Skip a layer
		data_key_2 += BLOCK_YSTRIDE;
	}
}
