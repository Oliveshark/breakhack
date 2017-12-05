function add_tiles_to_room (map, texture)
	for i=0,15 do
		for j=0,11 do
			if (i >= 1 and i <= 14 and j >= 1 and j <= 10) then
				if (i == 1 and j == 1) then
					add_tile(map, i, j, texture, 0, 48, false)
				elseif (i == 1 and j == 10) then
					add_tile(map, i, j, texture, 0, 80, false)
				elseif (i == 14 and j == 1) then
					add_tile(map, i, j, texture, 32, 48, false)
				elseif (i == 14 and j == 10) then
					add_tile(map, i, j, texture, 32, 80, false)
				elseif (i == 1) then
					add_tile(map, i, j, texture, 0, 64, false)
				elseif (j == 1) then
					add_tile(map, i, j, texture, 16, 48, false)
				elseif (i == 14) then
					add_tile(map, i, j, texture, 32, 64, false)
				elseif (j == 10) then
					add_tile(map, i, j, texture, 16, 80, false)
				else
					add_tile(map, i, j, texture, 16, 64, false)
				end
			end
		end
	end
end

function add_walls_to_room (map, texture)
	for i=0,15 do
		for j=0,11 do
			if (i == 0 and j == 0) then
				add_tile(map, i, j, texture, 0, 48, true)
			elseif (i == 15 and j == 0) then
				add_tile(map, i, j, texture, 32, 48, true)
			elseif (i == 0 and j == 11) then
				add_tile(map, i, j, texture, 0, 80, true)
			elseif (i == 15 and j == 11) then
				add_tile(map, i, j, texture, 32, 80, true)
			elseif (i == 0 or i == 15) then
				add_tile(map, i, j, texture, 0, 64, true)
			elseif (j == 0 or j == 11) then
				add_tile(map, i, j, texture, 16, 48, true)
			end
		end
	end
end

map = create_map()
-- add_texture(map, "path/to/a/fancy/texture")
-- add_tile(map, 1, 2, 3)

local floorTexture = add_texture(map, "assets/Objects/Floor.png")
local wallTexture = add_texture(map, "assets/Objects/Wall.png")

for i=0,9 do
	for j=0,9 do
		set_current_room(map, i, j);
		add_tiles_to_room(map, floorTexture);
		add_walls_to_room(map, wallTexture);
	end
end
