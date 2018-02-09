local room_builder = require "data/maproombuilder"
local monster_gen = require "data/monstergen"

map = create_map(CURRENT_LEVEL) -- 'map' needs to be global

room_builder.load_textures(map)
local room = room_builder.create_room()

set_current_room(map, 0, 0)
room_builder.build_square_room(map, room)
monster_gen.add_monster_to_room(map, 0, 0);
room_builder.add_full_lighting(map);
