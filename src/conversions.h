
#define CONV_ALL(id, mcn, datas, mtn, p2, is_tool, cb) \
	static_assert(id <= MC_ID_MAX, "MC ID is out of range! (update MC_ID_MAX if adding new items)"); \
	add_conversion(id, mcn, datas, mtn, p2, is_tool, cb)
#define CONV(id, mcn, mtn) CONV_D(id, mcn, nullptr, mtn)
#define CONV_D(id, mcn, datas, mtn) CONV_DP(id, mcn, datas, mtn, 0)
#define CONV_P(id, mcn, mtn, p2); CONV_DP(id, mcn, nullptr, mtn, p2)
#define CONV_DP(id, mcn, datas, mtn, p2) CONV_DP_CB(id, mcn, datas, mtn, p2, nullptr)
#define CONV_DP_CB(id, mcn, datas, mtn, p2, cb) CONV_ALL(id, mcn, datas, mtn, p2, false, cb)
#define CONV_TOOL(id, mcn, mtn) CONV_ALL(id, mcn, nullptr, mtn, 0, true, nullptr)

#define CONV_STAIR(id, mcn, mtn) \
	CONV_DP_CB(id, mcn, "0", mtn, 3, update_node_light); \
	CONV_DP_CB(id, mcn, "1", mtn, 1, update_node_light); \
	CONV_DP_CB(id, mcn, "2", mtn, 0, update_node_light); \
	CONV_DP_CB(id, mcn, "3", mtn, 2, update_node_light); \
	CONV_DP_CB(id, mcn, "4", mtn, 21, update_node_light); \
	CONV_DP_CB(id, mcn, "5", mtn, 23, update_node_light); \
	CONV_DP_CB(id, mcn, "6", mtn, 20, update_node_light); \
	CONV_DP_CB(id, mcn, "7", mtn, 22, update_node_light)

#define CONV_SLAB(id, mcn, dbottom, dtop, mtn) \
	CONV_DP_CB(id, mcn, #dbottom, mtn, 0, update_node_light); \
	CONV_DP_CB(id, mcn, #dtop, mtn, 22, update_node_light)

#define CONV_TRAPDOOR(id, mcn, mtn) \
	CONV_DP(id, mcn, "0", mtn, 0); \
	CONV_DP(id, mcn, "1", mtn, 2); \
	CONV_DP(id, mcn, "2", mtn, 3); \
	CONV_DP(id, mcn, "3", mtn, 1); \
	CONV_DP(id, mcn, "4", mtn "_open", 0); \
	CONV_DP(id, mcn, "5", mtn "_open", 2); \
	CONV_DP(id, mcn, "6", mtn "_open", 3); \
	CONV_DP(id, mcn, "7", mtn "_open", 1); \
	CONV_DP(id, mcn, "8", mtn, 20); \
	CONV_DP(id, mcn, "9", mtn, 22); \
	CONV_DP(id, mcn, "10", mtn, 23); \
	CONV_DP(id, mcn, "11", mtn, 21); \
	CONV_DP(id, mcn, "12", mtn "_open", 20); \
	CONV_DP(id, mcn, "13", mtn "_open", 22); \
	CONV_DP(id, mcn, "14", mtn "_open", 23); \
	CONV_DP(id, mcn, "15", mtn "_open", 21)

#define CONV_LOG(id, mcn, d, mtn) \
	CONV_DP(id, mcn, #d,                                   mtn, 0); \
	CONV_DP(id, mcn, std::to_string(d + (1 << 2)).c_str(), mtn, 12); \
	CONV_DP(id, mcn, std::to_string(d + (2 << 2)).c_str(), mtn, 4); \
	CONV_DP(id, mcn, std::to_string(d + (3 << 2)).c_str(), mtn, 0)

#define CONV_WALLMOUNTED(id, mcn, mtn, dt, db, dn, de, ds, dw) \
	CONV_DP(id, mcn, #dt, mtn, 0); \
	CONV_DP(id, mcn, #db, mtn, 1); \
	CONV_DP(id, mcn, #dn, mtn, 5); \
	CONV_DP(id, mcn, #de, mtn, 3); \
	CONV_DP(id, mcn, #ds, mtn, 4); \
	CONV_DP(id, mcn, #dw, mtn, 2)

// TODO: Check orientation
#define CONV_GATE(id, mcn, mtn) \
	CONV_DP(id, mcn, "0", mtn "_closed", 0); \
	CONV_DP(id, mcn, "1", mtn "_closed", 2); \
	CONV_DP(id, mcn, "2", mtn "_closed", 3); \
	CONV_DP(id, mcn, "3", mtn "_closed", 1); \
	CONV_DP(id, mcn, "4", mtn "_open", 0); \
	CONV_DP(id, mcn, "5", mtn "_open", 2); \
	CONV_DP(id, mcn, "6", mtn "_open", 3); \
	CONV_DP(id, mcn, "7", mtn "_open", 1)

#if MC_VERSION < 102
// TODO: Check orientation
#define CONV_DOOR(id, mcn, mtn) \
	CONV_DP_CB(id, mcn, "0", mtn "_b_2", 0, old_door_set_right); \
	CONV_DP_CB(id, mcn, "1", mtn "_b_2", 1, old_door_set_right); \
	CONV_DP_CB(id, mcn, "2", mtn "_b_2", 2, old_door_set_right); \
	CONV_DP_CB(id, mcn, "3", mtn "_b_2", 3, old_door_set_right); \
	CONV_DP(id, mcn, "4", mtn "_b_1", 3); \
	CONV_DP(id, mcn, "5", mtn "_b_1", 0); \
	CONV_DP(id, mcn, "6", mtn "_b_1", 1); \
	CONV_DP(id, mcn, "7", mtn "_b_1", 2); \
	CONV_DP_CB(id, mcn, "8", mtn "_t_2", 0, old_door_set_right); \
	CONV_DP_CB(id, mcn, "9", mtn "_t_2", 1, old_door_set_right); \
	CONV_DP_CB(id, mcn, "10", mtn "_t_2", 2, old_door_set_right); \
	CONV_DP_CB(id, mcn, "11", mtn "_t_2", 3, old_door_set_right); \
	CONV_DP(id, mcn, "12", mtn "_t_1", 3); \
	CONV_DP(id, mcn, "13", mtn "_t_1", 0); \
	CONV_DP(id, mcn, "14", mtn "_t_1", 1); \
	CONV_DP(id, mcn, "15", mtn "_t_1", 2);
#else // else if MC_VERSION >= 102
#define CONV_DOOR(id, mcn, mtn) \
	CONV_DP_CB(id, mcn, "0", mtn "_b_1", 1, finish_door); \
	CONV_DP_CB(id, mcn, "1", mtn "_b_1", 2, finish_door); \
	CONV_DP_CB(id, mcn, "2", mtn "_b_1", 3, finish_door); \
	CONV_DP_CB(id, mcn, "3", mtn "_b_1", 0, finish_door); \
	CONV_DP_CB(id, mcn, "4", mtn "_b_2", 5, finish_door); \
	CONV_DP_CB(id, mcn, "5", mtn "_b_2", 6, finish_door); \
	CONV_DP_CB(id, mcn, "6", mtn "_b_2", 7, finish_door); \
	CONV_DP_CB(id, mcn, "7", mtn "_b_2", 4, finish_door); \
	CONV_D(id, mcn, "8,9,10,11,12,13,14,15", mtn "_t_1");
#endif // MC_VERSION < 102


#define USE_MOD_MORETREES 1
#define USE_MOD_NETHER 1
#define USE_MOD_QUARTZ 1
#define USE_MOD_MESECONS 1
#define USE_MOD_TECHNIC 1
#define USE_MOD_STAIRSPLUS 0
#define USE_MOD_FARMINGPLUS 1
#define USE_MOD_BEDROCK 1
#define USE_MOD_FOOD 1
#define USE_MOD_HARDENED_CLAY 1
#define USE_MOD_HOMEDECOR 1
#define USE_MOD_BOWS 1


CONV(0, "minecraft:air", "air");
CONV(1, "minecraft:stone", "default:stone");  // Note: has subtypes of granite, etc
CONV(2, "minecraft:grass", "default:dirt_with_grass");
CONV(3, "minecraft:dirt", "default:dirt"); // Note: has dirt types
CONV(4, "minecraft:cobble", "default:cobble");

CONV_D(5, "minecraft:planks", "3", "default:junglewood");
CONV_D(5, "minecraft:planks", "4", "default:acacia_wood");
// Saplings have a "ready to grow" flag, which we ignore.
CONV_D(6, "minecraft:sapling", "3,11", "default:junglesapling");
CONV_D(6, "minecraft:sapling", "4,12", "default:acacia_sapling");

#if USE_MOD_MORETREES
	CONV_D(5, "minecraft:planks", "0", "default:wood");
	CONV_D(5, "minecraft:planks", "1", "moretrees:spruce_planks");
	CONV_D(5, "minecraft:planks", "2", "moretrees:birch_planks");
	CONV_D(5, "minecraft:planks", "5", "moretrees:oak_planks");

	CONV_D(6, "minecraft:sapling", "0,8", "default:sapling");
	CONV_D(6, "minecraft:sapling", "1,9", "moretrees:spruce_sapling");
	CONV_D(6, "minecraft:sapling", "2,10", "moretrees:birch_sapling");
	CONV_D(6, "minecraft:sapling", "5,13", "moretrees:oak_sapling");
#else
	CONV_D(5, "minecraft:planks", "0,1,2,5", "default:wood");
	CONV_D(6, "minecraft:sapling", "0,1,2,5", "default:sapling");
#endif

#if USE_MOD_BEDROCK
	CONV(7, "minecraft:bedrock", "bedrock:bedrock");
#else
	CONV(7, "minecraft:bedrock", "default:cloud");
#endif

CONV(8, "minecraft:flowing_water", "default:water_flowing");
CONV(9, "minecraft:water", "default:water_source");
CONV(10, "minecraft:flowing_lava", "default:lava_flowing");
CONV(11, "minecraft:lava", "default:lava_source");
CONV_D(12, "minecraft:sand", "0", "default:sand");
CONV_D(12, "minecraft:sand", "1", "default:desert_sand");
CONV(13, "minecraft:gravel", "default:gravel");
CONV(14, "minecraft:gold_ore", "default:stone_with_gold");
CONV(15, "minecraft:iron_ore", "default:stone_with_iron");
CONV(16, "minecraft:coal_ore", "default:stone_with_coal");

CONV_LOG(17, "minecraft:log", 0, "default:tree");
CONV_LOG(17, "minecraft:log", 3, "default:jungletree");
CONV_D(18, "minecraft:leaves", "0,8", "default:leaves");
CONV_DP(18, "minecraft:leaves", "4,12", "default:leaves", 1);
CONV_D(18, "minecraft:leaves", "3,11", "default:jungleleaves");
CONV_DP(18, "minecraft:leaves", "7,15", "default:jungleleaves", 1);
#if USE_MOD_MORETREES
	CONV_LOG(17, "minecraft:log", 1, "moretrees:spruce_trunk");
	CONV_LOG(17, "minecraft:log", 2, "moretrees:birch_trunk");
	CONV_D(18, "minecraft:leaves", "1,9", "moretrees:spruce_leaves");
	CONV_DP(18, "minecraft:leaves", "5,13", "moretrees:spruce_leaves", 1);
	CONV_D(18, "minecraft:leaves", "2,10", "moretrees:birch_leaves");
	CONV_DP(18, "minecraft:leaves", "6,14", "moretrees:birch_leaves", 1);
#else
	CONV_LOG(17, "minecraft:log", 1, "default:tree");
	CONV_LOG(17, "minecraft:log", 2, "default:tree");
	CONV_D(18, "minecraft:leaves", "1,2,9,10", "default:leaves");
	CONV_DP(18, "minecraft:leaves", "5,6,13,14", "default:leaves", 1);
#endif

CONV(19, "minecraft:sponge", "default:nyan_cat_rainbow");  // Sponge/wet sponge
CONV(20, "minecraft:glass", "default:glass");
CONV(21, "minecraft:lapis_ore", "default:stone_with_mese");
CONV(22, "minecraft:lapis_block", "default:mese");
// 23: Dispenser
CONV_D(24, "minecraft:sandstone", "0", "default:sandstone");
CONV_D(24, "minecraft:sandstone", "1,2", "default:sandstonebrick"); // Chiseled/smooth sandstone

#if USE_MOD_MESECONS
	CONV(25, "minecraft:noteblock", "mesecons_noteblock:noteblock");
#endif

CONV_DP(26, "minecraft:bed", "0,4", "beds:bed_bottom", 0); // S
CONV_DP(26, "minecraft:bed", "1,5", "beds:bed_bottom", 1); // W
CONV_DP(26, "minecraft:bed", "2,6", "beds:bed_bottom", 2); // N
CONV_DP(26, "minecraft:bed", "3,7", "beds:bed_bottom", 3); // E
CONV_DP(26, "minecraft:bed", "8,12", "beds:bed_top", 0);  // S
CONV_DP(26, "minecraft:bed", "9,13", "beds:bed_top", 1);  // W
CONV_DP(26, "minecraft:bed", "10,14", "beds:bed_top", 2); // N
CONV_DP(26, "minecraft:bed", "11,15", "beds:bed_top", 3); // E

CONV(27, "minecraft:golden_rail", "default:rail");  // Powered rail
CONV(28, "minecraft:detector_rail", "default:rail");

#if USE_MOD_MESECONS
	// TODO: Check orientation
	CONV_D(29, "minecraft:sticky_piston", "0", "mesecons_piston:piston_down_sticky_off");
	CONV_D(29, "minecraft:sticky_piston", "1", "mesecons_piston:piston_up_sticky_off");
	CONV_DP(29, "minecraft:sticky_piston", "2", "mesecons_piston:piston_sticky_off", 0);  // N
	CONV_DP(29, "minecraft:sticky_piston", "3", "mesecons_piston:piston_sticky_off", 2);  // S
	CONV_DP(29, "minecraft:sticky_piston", "4", "mesecons_piston:piston_sticky_off", 3);  // W
	CONV_DP(29, "minecraft:sticky_piston", "5", "mesecons_piston:piston_sticky_off", 1);  // E
	CONV_D(29, "minecraft:sticky_piston", "8", "mesecons_piston:piston_down_sticky_on");
	CONV_D(29, "minecraft:sticky_piston", "9", "mesecons_piston:piston_up_sticky_on");
	CONV_DP(29, "minecraft:sticky_piston", "10", "mesecons_piston:piston_sticky_on", 0);  // N
	CONV_DP(29, "minecraft:sticky_piston", "11", "mesecons_piston:piston_sticky_on", 2);  // S
	CONV_DP(29, "minecraft:sticky_piston", "12", "mesecons_piston:piston_sticky_on", 3);  // W
	CONV_DP(29, "minecraft:sticky_piston", "13", "mesecons_piston:piston_sticky_on", 1);  // E
#endif

#if USE_MOD_HOMEDECOR
	CONV(30, "minecraft:web", "homedecor:cobweb_plantlike");
#endif

CONV_D(31, "minecraft:tallgrass", "0", "default:dry_shrub");
CONV_D(31, "minecraft:tallgrass", "1,2", "default:grass_5");  // Tall grass/Fern
CONV(32, "minecraft:deadbush", "default:dry_shrub");

#if USE_MOD_MESECONS
	// TODO: Check orientation
	CONV_D(33, "minecraft:piston", "0", "mesecons_piston:piston_down_normal_off");
	CONV_D(33, "minecraft:piston", "1", "mesecons_piston:piston_up_normal_off");
	CONV_DP(33, "minecraft:piston", "2", "mesecons_piston:piston_normal_off", 0);  // N
	CONV_DP(33, "minecraft:piston", "3", "mesecons_piston:piston_normal_off", 2);  // S
	CONV_DP(33, "minecraft:piston", "4", "mesecons_piston:piston_normal_off", 3);  // W
	CONV_DP(33, "minecraft:piston", "5", "mesecons_piston:piston_normal_off", 1);  // E
	CONV_D(33, "minecraft:piston", "8", "mesecons_piston:piston_down_normal_on");
	CONV_D(33, "minecraft:piston", "9", "mesecons_piston:piston_up_normal_on");
	CONV_DP(33, "minecraft:piston", "10", "mesecons_piston:piston_normal_on", 0);  // N
	CONV_DP(33, "minecraft:piston", "11", "mesecons_piston:piston_normal_on", 2);  // S
	CONV_DP(33, "minecraft:piston", "12", "mesecons_piston:piston_normal_on", 3);  // W
	CONV_DP(33, "minecraft:piston", "13", "mesecons_piston:piston_normal_on", 1);  // E

	CONV_D(34, "minecraft:piston_head", "0", "mesecons_piston:piston_down_pusher_normal");
	CONV_D(34, "minecraft:piston_head", "1", "mesecons_piston:piston_up_pusher_normal");
	CONV_DP(34, "minecraft:piston_head", "2", "mesecons_piston:piston_pusher_normal", 0);  // N
	CONV_DP(34, "minecraft:piston_head", "3", "mesecons_piston:piston_pusher_normal", 2);  // S
	CONV_DP(34, "minecraft:piston_head", "4", "mesecons_piston:piston_pusher_normal", 3);  // W
	CONV_DP(34, "minecraft:piston_head", "5", "mesecons_piston:piston_pusher_normal", 1);  // E
	CONV_D(34, "minecraft:piston_head", "8", "mesecons_piston:piston_down_pusher_sticky");
	CONV_D(34, "minecraft:piston_head", "9", "mesecons_piston:piston_up_pusher_sticky");
	CONV_DP(34, "minecraft:piston_head", "10", "mesecons_piston:piston_pusher_sticky", 0);  // N
	CONV_DP(34, "minecraft:piston_head", "11", "mesecons_piston:piston_pusher_sticky", 2);  // S
	CONV_DP(34, "minecraft:piston_head", "12", "mesecons_piston:piston_pusher_sticky", 3);  // W
	CONV_DP(34, "minecraft:piston_head", "13", "mesecons_piston:piston_pusher_sticky", 1);  // E
#endif

CONV_D(35, "minecraft:wool", "0", "wool:white");
CONV_D(35, "minecraft:wool", "1", "wool:orange");
CONV_D(35, "minecraft:wool", "2", "wool:magenta");
CONV_D(35, "minecraft:wool", "3", "wool:blue");
CONV_D(35, "minecraft:wool", "4", "wool:yellow");
CONV_D(35, "minecraft:wool", "5", "wool:green");
CONV_D(35, "minecraft:wool", "6", "wool:pink");
CONV_D(35, "minecraft:wool", "7", "wool:dark_grey");
CONV_D(35, "minecraft:wool", "8", "wool:grey");
CONV_D(35, "minecraft:wool", "9", "wool:cyan");
CONV_D(35, "minecraft:wool", "10", "wool:violet");
CONV_D(35, "minecraft:wool", "11", "wool:blue");
CONV_D(35, "minecraft:wool", "12", "wool:brown");
CONV_D(35, "minecraft:wool", "13", "wool:dark_green");
CONV_D(35, "minecraft:wool", "14", "wool:red");
CONV_D(35, "minecraft:wool", "15", "wool:black");

// 36: Piston extension (block being moved by piston)

CONV(37, "minecraft:yellow_flower", "flowers:dandelion_yellow");
CONV_D(38, "minecraft:red_flower", "0,4", "flowers:rose");
CONV_D(38, "minecraft:red_flower", "1", "flowers:geranium");
CONV_D(38, "minecraft:red_flower", "2", "flowers:viola");
CONV_D(38, "minecraft:red_flower", "3,6,8", "flowers:dandelion_white");
CONV_D(38, "minecraft:red_flower", "5,7", "flowers:tulip");

CONV(39, "minecraft:brown_mushroom", "flowers:mushroom_brown");
CONV(40, "minecraft:red_mushroom", "flowers:mushroom_red");

CONV(41, "minecraft:gold_block", "default:goldblock");
CONV(42, "minecraft:iron_block", "default:steelblock");

// Double slabs
CONV_D(43, "minecraft:double_stone_slab", "0,8", "default:stone");
CONV_D(43, "minecraft:double_stone_slab", "1,9", "default:sandstone");
CONV_D(43, "minecraft:double_stone_slab", "2", "default:wood");
CONV_D(43, "minecraft:double_stone_slab", "3", "default:cobble");
CONV_D(43, "minecraft:double_stone_slab", "4", "default:brick");
CONV_D(43, "minecraft:double_stone_slab", "5", "default:stonebrick");
#if USE_MOD_NETHER
	CONV_D(43, "minecraft:double_stone_slab", "6", "nether:brick");
#else
	CONV_D(43, "minecraft:double_stone_slab", "6", "default:brick");
#endif
#if USE_MOD_QUARTZ
	CONV_D(43, "minecraft:double_stone_slab", "7,10", "quartz:chiseled");
#else
	CONV_D(43, "minecraft:double_stone_slab", "7,10", "default:sandstonebrick");
#endif

CONV_SLAB(44, "minecraft:stone_slab", 0, 8, "stairs:slab_stone");
CONV_SLAB(44, "minecraft:stone_slab", 1, 9, "stairs:slab_sandstone");
CONV_SLAB(44, "minecraft:stone_slab", 2, 10, "stairs:slab_wood");
CONV_SLAB(44, "minecraft:stone_slab", 3, 11, "stairs:slab_cobble");
CONV_SLAB(44, "minecraft:stone_slab", 4, 12, "stairs:slab_brick");
CONV_SLAB(44, "minecraft:stone_slab", 5, 13, "stairs:slab_stonebrick");
#if USE_MOD_NETHER
	CONV_SLAB(44, "minecraft:stone_slab", 6, 14, "stairs:slab_nether_brick");
#else
	CONV_SLAB(44, "minecraft:stone_slab", 6, 14, "stairs:slab_stonebrick");
#endif
#if USE_MOD_QUARTZ
	CONV_SLAB(44, "minecraft:stone_slab", 7, 15, "stairs:slab_quartzblock");
#else
	CONV_SLAB(44, "minecraft:stone_slab", 7, 15, "stairs:slab_stonebrick");
#endif

CONV(45, "minecraft:brick_block", "default:brick");
CONV(46, "minecraft:tnt", "tnt:tnt");
CONV(47, "minecraft:bookshelf", "default:bookshelf");
CONV(48, "minecraft:mossy_cobblestone", "default:mossycobble");
CONV(49, "minecraft:obsidian", "default:obsidian");

CONV_WALLMOUNTED(50, "minecraft:torch", "default:torch", 0, 5, 3, 2, 4, 1);

CONV(51, "minecraft:fire", "fire:basic_flame");

// TODO: Real conversion
CONV(52, "minecraft:mob_spawner", "air");

CONV_STAIR(53, "minecraft:oak_stairs", "stairs:stair_wood");

CONV_DP(54, "minecraft:chest", "2", "default:chest", 0);  // N
CONV_DP(54, "minecraft:chest", "3", "default:chest", 2);  // S
CONV_DP(54, "minecraft:chest", "4", "default:chest", 3);  // W
CONV_DP(54, "minecraft:chest", "5", "default:chest", 1);  // E

#if USE_MOD_MESECONS
	// XXX: Connections aren't saved, just power level
	CONV_D(55, "minecraft:redstone_wire", "0", "mesecons:wire_0_off");
	CONV_D(55, "minecraft:redstone_wire", "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15", "mesecons:wire_0_on");
#endif

CONV(56, "minecraft:diamond_ore", "default:stone_with_diamond");
CONV(57, "minecraft:diamond_block", "default:diamondblock");

CONV(58, "minecraft:crafting_table", "default:wood");

CONV_D(59, "minecraft:wheat", "0", "farming:wheat_1");
CONV_D(59, "minecraft:wheat", "1", "farming:wheat_2");
CONV_D(59, "minecraft:wheat", "2", "farming:wheat_3");
CONV_D(59, "minecraft:wheat", "3", "farming:wheat_4");
CONV_D(59, "minecraft:wheat", "4", "farming:wheat_5");
CONV_D(59, "minecraft:wheat", "5", "farming:wheat_6");
CONV_D(59, "minecraft:wheat", "6", "farming:wheat_7");
CONV_D(59, "minecraft:wheat", "7", "farming:wheat_8");

// Data is counter, only wet when counter reaches 7
CONV(60, "minecraft:farmland", "farming:soil");
CONV_D(60, "minecraft:farmland", "7", "farming:soil_wet");

CONV_DP(61, "minecraft:furnace", "2", "default:furnace", 0);  // N
CONV_DP(61, "minecraft:furnace", "3", "default:furnace", 2);  // S
CONV_DP(61, "minecraft:furnace", "4", "default:furnace", 3);  // W
CONV_DP(61, "minecraft:furnace", "5", "default:furnace", 1);  // E
CONV_DP(62, "minecraft:lit_furnace", "2", "default:furnace_active", 0);
CONV_DP(62, "minecraft:lit_furnace", "3", "default:furnace_active", 2);
CONV_DP(62, "minecraft:lit_furnace", "4", "default:furnace_active", 3);
CONV_DP(62, "minecraft:lit_furnace", "5", "default:furnace_active", 1);

CONV_P(63, "minecraft:standing_sign", "default:sign_wall", 1);

CONV_DOOR(64, "minecraft:wooden_door", "doors:door_wood");

CONV_DP(65, "minecraft:ladder", "2", "default:ladder", 4);
CONV_DP(65, "minecraft:ladder", "3", "default:ladder", 5);
CONV_DP(65, "minecraft:ladder", "4", "default:ladder", 3);
CONV_DP(65, "minecraft:ladder", "5", "default:ladder", 2);

CONV(66, "minecraft:rail", "default:rail");

CONV_STAIR(67, "minecraft:stone_stairs", "stairs:stair_cobble");

CONV_DP(68, "minecraft:wall_sign", "2", "default:sign_wall", 4); // N
CONV_DP(68, "minecraft:wall_sign", "3", "default:sign_wall", 5); // S
CONV_DP(68, "minecraft:wall_sign", "4", "default:sign_wall", 3); // W
CONV_DP(68, "minecraft:wall_sign", "5", "default:sign_wall", 2); // E

#if USE_MOD_MESECONS
	// XXX: Support up/down levers
	CONV_DP(69, "minecraft:lever", "1", "mesecons_walllever:wall_lever_off", 1);  // E
	CONV_DP(69, "minecraft:lever", "2", "mesecons_walllever:wall_lever_off", 3);  // W
	CONV_DP(69, "minecraft:lever", "3", "mesecons_walllever:wall_lever_off", 2);  // S
	CONV_DP(69, "minecraft:lever", "4,0,5,6,7", "mesecons_walllever:wall_lever_off", 0);  // N, up, down
	CONV_DP(69, "minecraft:lever", "9", "mesecons_walllever:wall_lever_on", 1);  // E
	CONV_DP(69, "minecraft:lever", "10", "mesecons_walllever:wall_lever_on", 3);  // W
	CONV_DP(69, "minecraft:lever", "11", "mesecons_walllever:wall_lever_on", 2);  // S
	CONV_DP(69, "minecraft:lever", "12,8,13,14,15", "mesecons_walllever:wall_lever_on", 0);  // N, up, down

	CONV(70, "minecraft:stone_pressure_plate", "mesecons_pressureplates:pressure_plate_stone");
	CONV(72, "minecraft:wooden_pressure_plate", "mesecons_pressureplates:pressure_plate_wood");
#endif

CONV_DOOR(71, "minecraft:iron_door", "doors:door_steel");

CONV(73, "minecraft:redstone_ore", "default:stone_with_mese");
CONV(74, "minecraft:lit_redstone_ore", "default:stone_with_mese");

#if USE_MOD_MESECONS
	CONV_WALLMOUNTED(75, "minecraft:unlit_redstone_torch", "mesecons_torch:mesecon_torch_off", 0, 5, 3, 2, 4, 1);
	CONV_WALLMOUNTED(76, "minecraft:lit_redstone_torch", "mesecons_torch:mesecon_torch_on", 0, 5, 3, 2, 4, 1);

	// TODO: Up/Down orienations (requires mesecons support)
	// Note: power state is ignored, since it resets quickly.  See also: wooden button
	CONV_DP(77, "minecraft:stone_button", "1,9,0,8,5,13", "mesecons_button:button_off", 1);
	CONV_DP(77, "minecraft:stone_button", "2,10", "mesecons_button:button_off", 3);
	CONV_DP(77, "minecraft:stone_button", "3,11", "mesecons_button:button_off", 2);
	CONV_DP(77, "minecraft:stone_button", "4,12", "mesecons_button:button_off", 0);
#endif

// Round snow height to nearest node
CONV_D(78, "minecraft:snow_layer", "0,1,2,3", "default:snow");
CONV_D(78, "minecraft:snow_layer", "4,5,6,7", "default:snowblock");

CONV(79, "minecraft:ice", "default:ice");
CONV(80, "minecraft:snow", "default:snowblock");
CONV(81, "minecraft:cactus", "default:cactus");
CONV(82, "minecraft:clay", "default:clay");
CONV(83, "minecraft:reeds", "default:papyrus");

#if USE_MOD_TECHNIC
	CONV(84, "minecraft:jukebox", "technic:music_player");
#endif

CONV(85, "minecraft:fence", "default:fence_wood");

#if USE_MOD_FARMINGPLUS
	// TODO: Check orientation
	CONV_DP(86, "minecraft:pumpkin", "0", "farming:pumpkin_face", 2);
	CONV_DP(86, "minecraft:pumpkin", "1", "farming:pumpkin_face", 3);
	CONV_DP(86, "minecraft:pumpkin", "2", "farming:pumpkin_face", 0);
	CONV_DP(86, "minecraft:pumpkin", "3", "farming:pumpkin_face", 1);

	CONV_DP(86, "minecraft:pumpkin", "4", "farming:pumpkin", 2);
	CONV_DP(86, "minecraft:pumpkin", "5", "farming:pumpkin", 3);
	CONV_DP(86, "minecraft:pumpkin", "6", "farming:pumpkin", 0);
	CONV_DP(86, "minecraft:pumpkin", "7", "farming:pumpkin", 1);
#endif

#if USE_MOD_NETHER
	CONV(87, "minecraft:netherrack", "nether:rack");
	CONV(88, "minecraft:soul_sand", "nether:sand");
	CONV(89, "minecraft:glowstone", "nether:glowstone");
	// XXX: Orientation not stored
	CONV(90, "minecraft:portal", "nether:portal");
#endif

// Jack o'lantern
#if USE_MOD_FARMINGPLUS
	// TODO: Check orientation
	// Ignore "no face" flag
	CONV_DP(91, "minecraft:lit_pumpkin", "0,4", "farming:pumpkin_face_light", 2);
	CONV_DP(91, "minecraft:lit_pumpkin", "1,5", "farming:pumpkin_face_light", 3);
	CONV_DP(91, "minecraft:lit_pumpkin", "2,6", "farming:pumpkin_face_light", 0);
	CONV_DP(91, "minecraft:lit_pumpkin", "3,7", "farming:pumpkin_face_light", 1);
#endif

#if USE_MOD_FOOD
	CONV(92, "minecraft:cake", "food:cake");
#endif

#if USE_MOD_MESECONS
	// TODO: Check orientation
	CONV_DP(93, "minecraft:unpowered_repeater", "0", "mesecons_delayer:delayer_off_1", 1);
	CONV_DP(93, "minecraft:unpowered_repeater", "1", "mesecons_delayer:delayer_off_1", 2);
	CONV_DP(93, "minecraft:unpowered_repeater", "2", "mesecons_delayer:delayer_off_1", 3);
	CONV_DP(93, "minecraft:unpowered_repeater", "3", "mesecons_delayer:delayer_off_1", 0);
	CONV_DP(93, "minecraft:unpowered_repeater", "4", "mesecons_delayer:delayer_off_2", 1);
	CONV_DP(93, "minecraft:unpowered_repeater", "5", "mesecons_delayer:delayer_off_2", 2);
	CONV_DP(93, "minecraft:unpowered_repeater", "6", "mesecons_delayer:delayer_off_2", 3);
	CONV_DP(93, "minecraft:unpowered_repeater", "7", "mesecons_delayer:delayer_off_2", 0);
	CONV_DP(93, "minecraft:unpowered_repeater", "8", "mesecons_delayer:delayer_off_3", 1);
	CONV_DP(93, "minecraft:unpowered_repeater", "9", "mesecons_delayer:delayer_off_3", 2);
	CONV_DP(93, "minecraft:unpowered_repeater", "10", "mesecons_delayer:delayer_off_3", 3);
	CONV_DP(93, "minecraft:unpowered_repeater", "11", "mesecons_delayer:delayer_off_3", 0);
	CONV_DP(93, "minecraft:unpowered_repeater", "12", "mesecons_delayer:delayer_off_4", 1);
	CONV_DP(93, "minecraft:unpowered_repeater", "13", "mesecons_delayer:delayer_off_4", 2);
	CONV_DP(93, "minecraft:unpowered_repeater", "14", "mesecons_delayer:delayer_off_4", 3);
	CONV_DP(93, "minecraft:unpowered_repeater", "15", "mesecons_delayer:delayer_off_4", 0);

	CONV_DP(94, "minecraft:powered_repeater", "0", "mesecons_delayer:delayer_on_1", 1);
	CONV_DP(94, "minecraft:powered_repeater", "1", "mesecons_delayer:delayer_on_1", 2);
	CONV_DP(94, "minecraft:powered_repeater", "2", "mesecons_delayer:delayer_on_1", 3);
	CONV_DP(94, "minecraft:powered_repeater", "3", "mesecons_delayer:delayer_on_1", 0);
	CONV_DP(94, "minecraft:powered_repeater", "4", "mesecons_delayer:delayer_on_2", 1);
	CONV_DP(94, "minecraft:powered_repeater", "5", "mesecons_delayer:delayer_on_2", 2);
	CONV_DP(94, "minecraft:powered_repeater", "6", "mesecons_delayer:delayer_on_2", 3);
	CONV_DP(94, "minecraft:powered_repeater", "7", "mesecons_delayer:delayer_on_2", 0);
	CONV_DP(94, "minecraft:powered_repeater", "8", "mesecons_delayer:delayer_on_3", 1);
	CONV_DP(94, "minecraft:powered_repeater", "9", "mesecons_delayer:delayer_on_3", 2);
	CONV_DP(94, "minecraft:powered_repeater", "10", "mesecons_delayer:delayer_on_3", 3);
	CONV_DP(94, "minecraft:powered_repeater", "11", "mesecons_delayer:delayer_on_3", 0);
	CONV_DP(94, "minecraft:powered_repeater", "12", "mesecons_delayer:delayer_on_4", 1);
	CONV_DP(94, "minecraft:powered_repeater", "13", "mesecons_delayer:delayer_on_4", 2);
	CONV_DP(94, "minecraft:powered_repeater", "14", "mesecons_delayer:delayer_on_4", 3);
	CONV_DP(94, "minecraft:powered_repeater", "15", "mesecons_delayer:delayer_on_4", 0);
#endif

CONV(95, "minecraft:stained_glass", "default:glass");  // Stained glass

CONV_TRAPDOOR(96, "minecraft:trapdoor", "doors:trapdoor");

// Silverfish blocks
CONV_D(97, "minecraft:monster_egg", "0", "default:stone");
CONV_D(97, "minecraft:monster_egg", "1", "default:cobble");
CONV_D(97, "minecraft:monster_egg", "2,3,4,5", "default:stonebrick");

CONV(98, "minecraft:stonebrick", "default:stonebrick");

// 99: Brown mushroom block
// 100: Red mushroom block

CONV(101, "minecraft:iron_bars", "xpanes:bar");
CONV(102, "minecraft:glass_pane", "xpanes:pane");

#if USE_MOD_FARMINGPLUS
	CONV(103, "minecraft:melon_block", "farming:pumpkin");
#endif

// 104: Pumpkin stem
// 105: Melon stem

CONV_DP(106, "minecraft:vine", nullptr, "default:leaves", 1);  // TODO: Use real vines

// 107: Fence gate

CONV_STAIR(108, "minecraft:brick_stairs", "stairs:stair_brick");

CONV_STAIR(109, "minecraft:stone_brick_stairs", "stairs:stair_stonebrick");

CONV(110, "minecraft:mycelium", "default:dirt_with_dry_grass");

CONV(111, "minecraft:waterlily", "flowers:waterlily");  // TODO: facedir?

#if USE_MOD_NETHER
	CONV(112, "minecraft:nether_brick", "nether:brick");
	CONV_STAIR(114, "minecraft:nether_brick_stairs", "stairs:stair_nether_brick");
#else
	CONV(112, "minecraft:nether_brick", "default:stonebrick");
	CONV_STAIR(114, "minecraft:nether_brick_stairs", "stairs:stair_stonebrick");
#endif

CONV(113, "minecraft:nether_brick_fence", "default:fence_wood");

// 115: Nether wart
// 116: Enchantment table
// 117: Brewing stand
// 118: Cauldron
// 119: End portal
// 120: End portal frame
// 121: End stone
// 122: Dragon egg

#if USE_MOD_MESECONS
	CONV(123, "mesecons:redstone_lamp", "mesecons_lightstone:lightstone_darkgray_off");
	CONV(124, "mesecons:lit_redstone_lamp", "mesecons_lightstone:lightstone_darkgray_on");
#endif

CONV_D(125, "minecraft:double_wooden_slab", "0", "default:wood");
CONV_D(125, "minecraft:double_wooden_slab", "3", "default:junglewood");
CONV_D(125, "minecraft:double_wooden_slab", "4", "default:acacia_wood");
CONV_SLAB(126, "minecraft:wooden_slab", 0, 8, "stairs:slab_wood");
CONV_SLAB(126, "minecraft:wooden_slab", 3, 11, "stairs:slab_junglewood");
CONV_SLAB(126, "minecraft:wooden_slab", 4, 12, "stairs:slab_acacia_wood");
#if USE_MOD_MORETREES
	CONV_D(125, "minecraft:double_wooden_slab", "1", "moretrees:spruce_planks");
	CONV_D(125, "minecraft:double_wooden_slab", "2", "moretrees:birch_planks");
	CONV_D(125, "minecraft:double_wooden_slab", "5", "moretrees:oak_planks");
	#if USE_MOD_STAIRSPLUS
		CONV_SLAB(126, "minecraft:wooden_slab", 1, 9, "moretrees:slab_spruce_planks");
		CONV_SLAB(126, "minecraft:wooden_slab", 2, 10, "moretrees:slab_birch_planks");
		CONV_SLAB(126, "minecraft:wooden_slab", 5, 13, "moretrees:slab_oak_planks");
	#else
		CONV_SLAB(126, "minecraft:wooden_slab", 1, 9, "stairs:slab_moretrees_spruce_planks");
		CONV_SLAB(126, "minecraft:wooden_slab", 2, 10, "stairs:slab_moretrees_birch_planks");
		CONV_SLAB(126, "minecraft:wooden_slab", 5, 13, "stairs:slab_moretrees_oak_planks");
	#endif
#else
	CONV_D(125, "minecraft:double_wooden_slab", "1,2,5", "default:wood");
	CONV_SLAB(126, "minecraft:wooden_slab", 1, 9, "stairs:slab_wood");
	CONV_SLAB(126, "minecraft:wooden_slab", 2, 10, "stairs:slab_wood");
	CONV_SLAB(126, "minecraft:wooden_slab", 5, 13, "stairs:slab_wood");
#endif

// 127: Cocoa

CONV_STAIR(128, "minecraft:sandstone_stairs", "stairs:stair_sandstone");

CONV(129, "minecraft:emerald_ore", "default:stone_with_diamond");

// 130: Ender chest
// 131: Tripwire hook
// 132: Tripwire
// 133: Emerald block

#if USE_MOD_MORETREES
	#if USE_MOD_STAIRSPLUS
		CONV_STAIR(134, "minecraft:spruce_stairs", "moretrees:stair_spruce_planks");
		CONV_STAIR(135, "minecraft:birch_stairs", "moretrees:stair_birch_planks");
	#else
		CONV_STAIR(134, "minecraft:spruce_stairs", "stairs:stair_moretrees_spruce_planks");
		CONV_STAIR(135, "minecraft:birch_stairs", "stairs:stair_moretrees_birch_planks");
	#endif
#else
	CONV_STAIR(134, "minecraft:spruce_stairs", "stairs:stair_wood");
	CONV_STAIR(135, "minecraft:birch_stairs", "stairs:stair_wood");
#endif

CONV_STAIR(136, "minecraft:jungle_stairs", "stairs:stair_junglewood");

#if USE_MOD_MESECONS
	// TODO: Convert commands?
	CONV(137, "minecraft:command_block", "mesecons_commandblock:commandblock_off");
#endif

// 138: Beacon

CONV_D(139, "minecraft:cobblestone_wall", "0", "walls:cobble");
CONV_D(139, "minecraft:cobblestone_wall", "1", "walls:mossycobble");

// 140: Flower pot

#if USE_MOD_FARMINGPLUS
	CONV_D(141, "minecraft:carrots", "0,1", "farming:carrots_1");
	CONV_D(141, "minecraft:carrots", "2,3", "farming:carrots_2");
	CONV_D(141, "minecraft:carrots", "4,5,6", "farming:carrots_3");
	CONV_D(141, "minecraft:carrots", "7", "farming:carrots");

	CONV_D(142, "minecraft:potatoes", "0,1,2", "farming:potato_1");
	CONV_D(142, "minecraft:potatoes", "3,4,5,6", "farming:potato_2");
	CONV_D(142, "minecraft:potatoes", "7", "farming:potato");
#endif

#if USE_MOD_MESECONS
	// TODO: Up/Down orienations (requires mesecons support)
	// Note: power state is ignored, since it resets quickly.  See also: stone button
	CONV_DP(143, "minecraft:wooden_button", "1,9,0,8,5,13", "mesecons_button:button_off", 1);
	CONV_DP(143, "minecraft:wooden_button", "2,10", "mesecons_button:button_off", 3);
	CONV_DP(143, "minecraft:wooden_button", "3,11", "mesecons_button:button_off", 2);
	CONV_DP(143, "minecraft:wooden_button", "4,12", "mesecons_button:button_off", 0);
#endif

// 144: Mob head

CONV(145, "minecraft:anvil", "default:steelblock");

// Trapped chest
CONV_DP(146, "minecraft:trapped_chest", "2", "default:chest", 0);
CONV_DP(146, "minecraft:trapped_chest", "3", "default:chest", 2);
CONV_DP(146, "minecraft:trapped_chest", "4", "default:chest", 3);
CONV_DP(146, "minecraft:trapped_chest", "5", "default:chest", 1);


#if USE_MOD_MESECONS
	CONV(147, "minecraft:light_weighted_pressure_plate", "mesecons_pressureplates:pressure_plate_stone");
	CONV(148, "minecraft:heavy_weighted_pressure_plate", "mesecons_pressureplates:pressure_plate_stone");
#endif

// TODO: 149: Redstone comparator (off) (logic gates)
// 150: Redstone comparator (on)

#if USE_MOD_MESECONS
	CONV(151, "minecraft:daylight_detector", "mesecons_solarpanel:solar_panel_off");
#endif

// 152: Redstone block
// 153: Nether quartz ore
// 154: Hopper

#if USE_MOD_QUARTZ
	CONV_D(155, "minecraft:quartz_block", "0", "quartz:block");
	CONV_D(155, "minecraft:quartz_block", "1", "quartz:chiseled");
	CONV_D(155, "minecraft:quartz_block", "2", "quartz:pillar");
	CONV_DP(155, "minecraft:quartz_block", "3", "quartz:pillar", 4);  // North/South pilar
	CONV_DP(155, "minecraft:quartz_block", "4", "quartz:pillar", 12);  // East/West pillar

	CONV_STAIR(156, "minecraft:quartz_stairs", "stairs:stair_quartzblock");
#endif

CONV(157, "minecraft:activator_rail", "default:rail");

// 158: Dropper

#if USE_MOD_HARDENED_CLAY
	CONV_D(159, "minecraft:stained_hardened_clay", "0", "hardened_clay:hardened_clay_white");
	CONV_D(159, "minecraft:stained_hardened_clay", "1", "hardened_clay:hardened_clay_orange");
	CONV_D(159, "minecraft:stained_hardened_clay", "2", "hardened_clay:hardened_clay_magenta");
	CONV_D(159, "minecraft:stained_hardened_clay", "3", "hardened_clay:hardened_clay_light_blue");
	CONV_D(159, "minecraft:stained_hardened_clay", "4", "hardened_clay:hardened_clay_yellow");
	CONV_D(159, "minecraft:stained_hardened_clay", "5", "hardened_clay:hardened_clay_lime");
	CONV_D(159, "minecraft:stained_hardened_clay", "6", "hardened_clay:hardened_clay_pink");
	CONV_D(159, "minecraft:stained_hardened_clay", "7", "hardened_clay:hardened_clay_gray");
	CONV_D(159, "minecraft:stained_hardened_clay", "8", "hardened_clay:hardened_clay_light_gray");
	CONV_D(159, "minecraft:stained_hardened_clay", "9", "hardened_clay:hardened_clay_cyan");
	CONV_D(159, "minecraft:stained_hardened_clay", "10", "hardened_clay:hardened_clay_purple");
	CONV_D(159, "minecraft:stained_hardened_clay", "11", "hardened_clay:hardened_clay_blue");
	CONV_D(159, "minecraft:stained_hardened_clay", "12", "hardened_clay:hardened_clay_brown");
	CONV_D(159, "minecraft:stained_hardened_clay", "13", "hardened_clay:hardened_clay_green");
	CONV_D(159, "minecraft:stained_hardened_clay", "14", "hardened_clay:hardened_clay_red");
	CONV_D(159, "minecraft:stained_hardened_clay", "15", "hardened_clay:hardened_clay_black");
#else
	CONV(159, "minecraft:stained_hardened_clay", "default:clay");
#endif

CONV(160, "minecraft:stained_glass_pane", "xpanes:pane");

CONV_D(161, "minecraft:leaves2", "0,8", "default:acacia_leaves");
CONV_DP(161, "minecraft:leaves2", "4,12", "default:acacia_leaves", 1);
CONV_LOG(162, "minecraft:log2", 0, "default:acacia_tree");
#if USE_MOD_MORETREES
	CONV_D(161, "minecraft:leaves2", "1,9", "moretrees:oak_leaves");
	CONV_DP(161, "minecraft:leaves2", "5,13", "moretrees:oak_leaves", 1);
	CONV_LOG(162, "minecraft:log2", 1, "moretrees:oak_trunk");
#else
	CONV_D(161, "minecraft:leaves2", "1,2,3,9,10,11", "default:leaves");
	CONV_DP(161, "minecraft:leaves2", "5,6,7,13,14,15", "default:leaves", 1);
	CONV_LOG(162, "minecraft:log2", 1, "default:tree");
#endif

CONV_STAIR(163, "minecraft:acacia_stairs", "stairs:stair_acacia_wood");
#if USE_MOD_MORETREES
	#if USE_MOD_STAIRSPLUS
		CONV_STAIR(164, "minecraft:dark_oak_stairs", "moretrees:stair_oak_planks");
	#else
		CONV_STAIR(164, "minecraft:dark_oak_stairs", "stairs:stair_moretrees_oak_planks");
	#endif
#else
	CONV_STAIR(164, "minecraft:dark_oak_stairs", "stairs:stair_wood");
#endif

// 165: Slime block

CONV(166, "minecraft:barrier", "default:cloud");

CONV_TRAPDOOR(167, "minecraft:iron_trapdoor", "doors:trapdoor_steel");

// 168: Prismarine
// 169: Sea lantern

CONV(170, "minecraft:hay_block", "farming:straw");

// 171: Carpets

CONV(172, "minecraft:hardened_clay", "default:clay");
CONV(173, "minecraft:coal_block", "default:coalblock");
CONV(174, "minecraft:packed_ice", "default:ice");

CONV_D(175, "minecraft:double_plant", "0", "flowers:dandelion_yellow");  // Sunflower
CONV_D(175, "minecraft:double_plant", "1", "flowers:viola");  // Lilac
CONV_DP_CB(175, "minecraft:double_plant", "2", "default:junglegrass", 0, update_node_light);  // Double-tall grass
CONV_D(175, "minecraft:double_plant", "3", "default:junglegrass");  // Large fern
CONV_D(175, "minecraft:double_plant", "4", "flowers:rose");  // Rose bush
CONV_D(175, "minecraft:double_plant", "5", "flowers:dandelion_white");  // Peony

// 176: Standing banner
// 177: Wall banner

#if USE_MOD_MESECONS
	CONV(178, "minecraft:daylight_detector_inverted", "mesecons_solarpanel:solar_panel_off");
#endif

CONV(179, "minecraft:red_sandstone", "default:desert_cobble");  // Chisled/smooth
CONV_STAIR(180, "minecraft:red_sandstone_stairs", "stairs:stair_desert_cobble");
CONV(181, "minecraft:double_stone_slab2", "default:desert_cobble");  // Double chiseled/smooth
CONV_SLAB(182, "minecraft:stone_slab2", 0, 8, "stairs:slab_desert_cobble");

CONV_GATE(183, "minecraft:spruce_fence_gate", "doors:gate");
CONV_GATE(184, "minecraft:birch_fence_gate", "doors:gate");
CONV_GATE(185, "minecraft:jungle_fence_gate", "doors:gate");
CONV_GATE(186, "minecraft:dark_oak_fence_gate", "doors:gate");
CONV_GATE(187, "minecraft:acacia_fence_gate", "doors:gate");

CONV(188, "minecraft:spruce_fence", "default:fence_wood");
CONV(189, "minecraft:birch_fence", "default:fence_wood");
CONV(190, "minecraft:jungle_fence", "default:fence_wood");
CONV(191, "minecraft:dark_oak_fence", "default:fence_wood");
CONV(192, "minecraft:acacia_fence", "default:fence_wood");

CONV_DOOR(193, "minecraft:spruce_door", "doors:door_wood");
CONV_DOOR(194, "minecraft:birch_door", "doors:door_wood");
CONV_DOOR(195, "minecraft:jungle_door", "doors:door_wood");
CONV_DOOR(196, "minecraft:acacia_door", "doors:door_wood");
CONV_DOOR(197, "minecraft:dark_oak_door", "doors:door_wood");

// TODO: Check orientation
CONV_WALLMOUNTED(198, "minecraft:end_rod", "default:torch", 0, 1, 3, 4, 2, 5);

// 199: Chorus plant
// 200: Chorus flower
// 201: Purpur block
// 202: Purpur pillar
// 203: Purpur stairs
// 204: Purpur double slab
// 205: Purpur slab

CONV(206, "minecraft:end_bricks", "default:sandstonebrick");

// 207: Beetroot seeds

CONV(208, "minecraft:grass_path", "default:dirt_with_dry_grass");

// 209: End gateway
// 210: Repeating command block
// 211: Chain command block

CONV(212, "minecraft:frosted_ice", "default:ice");

// 213-254: Unused
// 255: Structure block

CONV_TOOL(256, "minecraft:iron_shovel", "default:shovel_steel");
CONV_TOOL(257, "minecraft:iron_pickaxe", "default:pick_steel");
CONV_TOOL(258, "minecraft:iron_axe", "default:axe_steel");

CONV(259, "minecraft:flint_and_steel", "fire:basic_flame");  // TODO: Use actual flint and steel
CONV(260, "minecraft:apple", "default:apple");

#ifdef USE_MOD_BOWS
	CONV_TOOL(261, "minecraft:bow", "bows:bow");
	CONV(262, "minecraft:arrow", "bows:arrow");
#endif

CONV(263, "minecraft:coal", "default:coal_lump");
CONV(264, "minecraft:diamond", "default:diamond");
CONV(265, "minecraft:iron_ingot", "default:steel_ingot");
CONV(266, "minecraft:gold_ingot", "default:gold_ingot");

CONV_TOOL(267, "minecraft:iron_sword", "default:sword_steel");

CONV_TOOL(268, "minecraft:wooden_sword", "default:sword_wood");
CONV_TOOL(269, "minecraft:wooden_shovel", "default:shovel_wood");
CONV_TOOL(270, "minecraft:wooden_pickaxe", "default:pick_wood");
CONV_TOOL(271, "minecraft:wooden_axe", "default:axe_wood");

CONV_TOOL(272, "minecraft:stone_sword", "default:sword_stone");
CONV_TOOL(273, "minecraft:stone_shovel", "default:shovel_stone");
CONV_TOOL(274, "minecraft:stone_pickaxe", "default:pick_stone");
CONV_TOOL(275, "minecraft:stone_axe", "default:axe_stone");

CONV_TOOL(276, "minecraft:diamond_sword", "default:sword_diamond");
CONV_TOOL(277, "minecraft:diamond_shovel", "default:shovel_diamond");
CONV_TOOL(278, "minecraft:diamond_pickaxe", "default:pick_diamond");
CONV_TOOL(279, "minecraft:diamond_axe", "default:axe_diamond");

CONV_TOOL(280, "minecraft:stick", "default:stick");

// 281: Bowl
// 282: Mushroom stew

CONV_TOOL(283, "minecraft:golden_sword", "default:sword_gold");
CONV_TOOL(284, "minecraft:golden_shovel", "default:shovel_gold");
CONV_TOOL(285, "minecraft:golden_pickaxe", "default:pick_gold");
CONV_TOOL(286, "minecraft:golden_axe", "default:axe_gold");

CONV(287, "minecraft:string", "farming:cotton");

// 288: Feather

CONV(289, "minecraft:gunpowder", "tnt:gunpowder");

CONV_TOOL(290, "minecraft:wooden_hoe", "farming:hoe_wood");
CONV_TOOL(291, "minecraft:stone_hoe", "farming:hoe_stone");
CONV_TOOL(292, "minecraft:iron_hoe", "farming:hoe_steel");
CONV_TOOL(293, "minecraft:diamond_hoe", "farming:hoe_diamond");
CONV_TOOL(294, "minecraft:golden_hoe", "farming:hoe_gold");

CONV(295, "minecraft:wheat_seeds", "farming:seed_wheat");
CONV(296, "minecraft:wheat", "farming:wheat");
CONV(297, "minecraft:bread", "farming:bread");


// 298-317: Armor
// 318: Flint
// 319-320: Pork
// 321: Painting

CONV(322, "minecraft:golden_apple", "default:apple");
CONV(323, "minecraft:sign", "default:sign_wall");
CONV(324, "minecraft:wooden_door", "doors:door_wood");
CONV(325, "minecraft:bucket", "bucket:bucket_empty");
CONV(326, "minecraft:water_bucket", "bucket:bucket_water");
CONV(327, "minecraft:lava_bucket", "bucket:bucket_water");

// TODO: 328: Minecart
// 329: Saddle

CONV(330, "minecraft:iron_door", "doors:door_steel");

#if USE_MOD_MESECONS
	CONV(331, "minecraft:redstone", "mesecons:wire_00000000_off");
#endif

CONV(332, "minecraft:snowball", "default:");

// 333: Boat
// 334: Leather

CONV(335, "minecraft:milk_bucket", "bucket:bucket_empty");
CONV(336, "minecraft:brick", "default:brick");
CONV(337, "minecraft:clay_ball", "default:clay");
CONV(338, "minecraft:reeds", "default:papyrus");
CONV(339, "minecraft:paper", "default:paper");
CONV(340, "minecraft:book", "default:book");

// 341: Slime ball
// 342: Chest minecart
// 343: Furnace minecart
// 344: Egg
// 345: Compass
// 346: Fishing rod
// 347: Clock

#if USE_MOD_NETHER
	CONV(348, "minecraft:glowstone_dust", "nether:glowstone");
#endif
