#pragma once
#include <cstdint>

#define MAX_DISTANCE (30900)
#define MAP_BLOCK_SIZE 16
#define MC_MAP_HEIGHT 256
#define MC_GROUP_CHUNKS (32*32)
#define NODES_PER_BLOCK (MAP_BLOCK_SIZE*MAP_BLOCK_SIZE*MAP_BLOCK_SIZE)

// We currently use YZX order, even though MTBlocks are serialized in ZYX order.
#define BLOCK_NODE_IDX(x, y, z) (((static_cast<uint16_t>(y) & 0xF) << 8) | \
		(((z) & 0xF) << 4) | ((x) & 0xF))
#define BLOCK_IDX_MASK_X 0x00F
#define BLOCK_IDX_MASK_Y 0xF00
#define BLOCK_IDX_MASK_Z 0x0F0
#define BLOCK_IDX_X(idx) static_cast<uint8_t>( (idx) & BLOCK_IDX_MASK_X)
#define BLOCK_IDX_Y(idx) static_cast<uint8_t>(((idx) & BLOCK_IDX_MASK_Y) >> 8)
#define BLOCK_IDX_Z(idx) static_cast<uint8_t>(((idx) & BLOCK_IDX_MASK_Z) >> 4)
#define BLOCK_NODE_POS(idx) \
		BLOCK_IDX_X(idx), \
		BLOCK_IDX_Y(idx), \
		BLOCK_IDX_Z(idx)
#define BLOCK_XSTRIDE 1
#define BLOCK_YSTRIDE (MAP_BLOCK_SIZE*MAP_BLOCK_SIZE)
#define BLOCK_ZSTRIDE (MAP_BLOCK_SIZE)

// Offset between MC and MT y positions.  Used to align water levels.
#define BLOCK_Y_OFFSET 4

struct P {int x, y, z;};
static_assert(BLOCK_NODE_IDX(P{BLOCK_NODE_POS(1234)}.x, P{BLOCK_NODE_POS(1234)}.y, P{BLOCK_NODE_POS(1234)}.z) == 1234, "");
static_assert(P{BLOCK_NODE_POS(1234)}.x == 2 && P{BLOCK_NODE_POS(1234)}.y == 4 && P{BLOCK_NODE_POS(1234)}.z == 0xD, "");
static_assert(BLOCK_NODE_IDX(2, 4, 0xD) == 1234, "");

#define LIGHT_SUN (0xF)
#define LIGHT_MAX (LIGHT_SUN-1)

typedef uint16_t content_t;

enum : content_t {
	CONTENT_IGNORE,
	CONTENT_AIR,
	CONTENT_FIRST,
};
