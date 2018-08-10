local room_builder = require "maproombuilder"
local monster_gen = require "monstergen"
local trap_gen = require "trapgen"
local chest_gen = require "chestgen"

map = create_map(CURRENT_LEVEL) -- 'map' needs to be global

room_builder.load_textures(map)

set_current_room(map, 0, 0)
local room = room_builder.create_empty_room()
room_builder.build_room(room)
room_builder.add_full_lighting(room)
monster_gen.add_monsters_to_room(room, 0, 0)
trap_gen.add_traps_to_room(room, 0, 0)
chest_gen.add_chests_to_room(room, 0, 0)
room_builder.load_room(map, room)
monster_gen.load_monsters(map, room.monsters)
trap_gen.load_traps(map, room.traps)
chest_gen.load_chests(map, room.chests)
