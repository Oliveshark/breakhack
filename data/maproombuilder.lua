-- CONSTANTS
UP		= 1
LEFT	= 2
RIGHT	= 3
DOWN	= 4

-- Textures
local floorTexture = nil
local wallTexture = nil

local floor_center
local floor_top
local floor_bottom
local floor_left
local floor_right
local floor_topleft
local floor_topright
local floor_bottomleft
local floor_bottomright

local wall_topleft
local wall_topright
local wall_bottomleft
local wall_bottomright
local wall_vertical
local wall_horizontal

function load_textures(map)
	floorTexture = add_texture(map, "assets/Objects/Floor.png")
	wallTexture = add_texture(map, "assets/Objects/Wall.png")

	local xo = 0
	local yo = 48

	floor_center		= { floorTexture, xo + 16, yo + 16, false }
	floor_top			= { floorTexture, xo + 16, yo +  0, false }
	floor_bottom		= { floorTexture, xo + 16, yo + 32, false }
	floor_left			= { floorTexture, xo +  0, yo + 16, false }
	floor_right			= { floorTexture, xo + 32, yo + 16, false }
	floor_topleft		= { floorTexture, xo +  0, yo +  0, false }
	floor_topright		= { floorTexture, xo + 32, yo +  0, false }
	floor_bottomleft	= { floorTexture, xo +  0, yo + 32, false }
	floor_bottomright	= { floorTexture, xo + 32, yo + 32, false }

	wall_topleft		= { wallTexture, xo +  0, yo +  0, true }
	wall_topright		= { wallTexture, xo + 32, yo +  0, true }
	wall_bottomleft		= { wallTexture, xo +  0, yo + 32, true }
	wall_bottomright	= { wallTexture, xo + 32, yo + 32, true }
	wall_vertical		= { wallTexture, xo +  0, yo + 16, true }
	wall_horizontal		= { wallTexture, xo + 16, yo +  0, true }
end

function create_room ()
	local room = {}
	room.exits = {}
	room.active = false
	room.goal = false
	room.path_dir = 0
	return room;
end

function add_tiles_to_room (map)
	for i=0,15 do
		for j=0,11 do
			if (i >= 1 and i <= 14 and j >= 1 and j <= 10) then
				if (i == 1 and j == 1) then
					add_tile(map, i, j, unpack(floor_topleft))
				elseif (i == 14 and j == 1) then
					add_tile(map, i, j, unpack(floor_topright))
				elseif (i == 1 and j == 10) then
					add_tile(map, i, j, unpack(floor_bottomleft))
				elseif (i == 14 and j == 10) then
					add_tile(map, i, j, unpack(floor_bottomright))
				elseif (i == 1) then
					add_tile(map, i, j, unpack(floor_left))
				elseif (i == 14) then
					add_tile(map, i, j, unpack(floor_right))
				elseif (j == 1) then
					add_tile(map, i, j, unpack(floor_top))
				elseif (j == 10) then
					add_tile(map, i, j, unpack(floor_bottom))
				else
					add_tile(map, i, j, unpack(floor_center))
				end
			end
		end
	end
end

function add_walls_to_room (map)
	for i=0,15 do
		for j=0,11 do
			if (i == 0 and j == 0) then
				add_tile(map, i, j, unpack(wall_topleft))
			elseif (i == 15 and j == 0) then
				add_tile(map, i, j, unpack(wall_topright))
			elseif (i == 0 and j == 11) then
				add_tile(map, i, j, unpack(wall_bottomleft))
			elseif (i == 15 and j == 11) then
				add_tile(map, i, j, unpack(wall_bottomright))
			elseif (i == 0 or i == 15) then
				add_tile(map, i, j, unpack(wall_vertical))
			elseif (j == 0 or j == 11) then
				add_tile(map, i, j, unpack(wall_horizontal))
			end
		end
	end
end

function add_exit(map, direction)
	if direction > 4 then return end

	if direction == UP then
		add_tile(map, 6, 0, unpack(wall_bottomright))
		add_tile(map, 7, 0, unpack(floor_center))
		add_tile(map, 8, 0, unpack(floor_center))
		add_tile(map, 9, 0, unpack(wall_bottomleft))
	elseif direction == LEFT then
		add_tile(map, 0, 4, unpack(wall_bottomright))
		add_tile(map, 0, 5, unpack(floor_center))
		add_tile(map, 0, 6, unpack(floor_center))
		add_tile(map, 0, 7, unpack(wall_topright))
	elseif direction == RIGHT then
		add_tile(map, 15, 4, unpack(wall_bottomleft))
		add_tile(map, 15, 5, unpack(floor_center))
		add_tile(map, 15, 6, unpack(floor_center))
		add_tile(map, 15, 7, unpack(wall_topleft))
	elseif direction == DOWN then
		add_tile(map, 6, 11, unpack(wall_topright))
		add_tile(map, 7, 11, unpack(floor_center))
		add_tile(map, 8, 11, unpack(floor_center))
		add_tile(map, 9, 11, unpack(wall_topleft))
	end
end
