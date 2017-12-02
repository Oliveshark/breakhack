function add_tiles_to_room (map, room_x, room_y, texture)
	for i=0,11 do
		for j=0,15 do
			if (i == 0 and j == 0) then
				add_tile(map, room_x, room_y, i, j, texture, 0, 48)
			elseif (i == 11 and j == 0) then
				add_tile(map, room_x, room_y, i, j, texture, 0, 80)
			elseif (i == 0 and j == 15) then
				add_tile(map, room_x, room_y, i, j, texture, 32, 48)
			elseif (i == 11 and j == 15) then
				add_tile(map, room_x, room_y, i, j, texture, 32, 80)
			elseif (i == 0) then
				add_tile(map, room_x, room_y, i, j, texture, 16, 48)
			elseif (j == 0) then
				add_tile(map, room_x, room_y, i, j, texture, 0, 64)
			elseif (i == 11) then
				add_tile(map, room_x, room_y, i, j, texture, 16, 80)
			elseif (j == 15) then
				add_tile(map, room_x, room_y, i, j, texture, 32, 64)
			else
				add_tile(map, room_x, room_y, i, j, texture, 16, 64)
			end
		end
	end
end

map = create_map()
-- add_texture(map, "path/to/a/fancy/texture")
-- add_tile(map, 1, 2, 3)

local floorTexture = add_texture(map, "assets/Objects/Floor.png")

for i=0,9 do
	for j=0,9 do
		add_tiles_to_room(map, i, j, floorTexture);
	end
end
