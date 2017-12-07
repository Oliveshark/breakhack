-- CONSTANTS
local UP	= 1
local LEFT	= 2
local RIGHT	= 3
local DOWN	= 4

-- Textures
local textures = {
	floorTexture = nil,
	wallTexture = nil
}

local floor = {
	center = nil,
	top = nil,
	bottom = nil,
	left = nil,
	right = nil,
	topleft = nil,
	topright = nil,
	bottomleft = nil,
	bottomright = nil,
}

local wall = {
	topleft = nil,
	topright = nil,
	bottomleft = nil,
	bottomright = nil,
	vertical = nil,
	horizontal = nil
}

function load_textures(map)
	textures.floorTexture = add_texture(map, "assets/Objects/Floor.png")
	textures.wallTexture = add_texture(map, "assets/Objects/Wall.png")

	local xo = 0
	local yo = 48

	floor.center		= { textures.floorTexture, xo + 16, yo + 16, false }
	floor.top			= { textures.floorTexture, xo + 16, yo +  0, false }
	floor.bottom		= { textures.floorTexture, xo + 16, yo + 32, false }
	floor.left			= { textures.floorTexture, xo +  0, yo + 16, false }
	floor.right			= { textures.floorTexture, xo + 32, yo + 16, false }
	floor.topleft		= { textures.floorTexture, xo +  0, yo +  0, false }
	floor.topright		= { textures.floorTexture, xo + 32, yo +  0, false }
	floor.bottomleft	= { textures.floorTexture, xo +  0, yo + 32, false }
	floor.bottomright	= { textures.floorTexture, xo + 32, yo + 32, false }

	wall.topleft		= { textures.wallTexture, xo +  0, yo +  0, true }
	wall.topright		= { textures.wallTexture, xo + 32, yo +  0, true }
	wall.bottomleft		= { textures.wallTexture, xo +  0, yo + 32, true }
	wall.bottomright	= { textures.wallTexture, xo + 32, yo + 32, true }
	wall.vertical		= { textures.wallTexture, xo +  0, yo + 16, true }
	wall.horizontal		= { textures.wallTexture, xo + 16, yo +  0, true }
end

function create_room ()
	return {
		exits = {},
		active = false,
		goal = false,
		path_dir = 0,
		type = "room"
	}
end

local function add_tiles_to_room (map)
	for i=0,15 do
		for j=0,11 do
			if (i >= 1 and i <= 14 and j >= 1 and j <= 10) then
				if (i == 1 and j == 1) then
					add_tile(map, i, j, unpack(floor.topleft))
				elseif (i == 14 and j == 1) then
					add_tile(map, i, j, unpack(floor.topright))
				elseif (i == 1 and j == 10) then
					add_tile(map, i, j, unpack(floor.bottomleft))
				elseif (i == 14 and j == 10) then
					add_tile(map, i, j, unpack(floor.bottomright))
				elseif (i == 1) then
					add_tile(map, i, j, unpack(floor.left))
				elseif (i == 14) then
					add_tile(map, i, j, unpack(floor.right))
				elseif (j == 1) then
					add_tile(map, i, j, unpack(floor.top))
				elseif (j == 10) then
					add_tile(map, i, j, unpack(floor.bottom))
				else
					add_tile(map, i, j, unpack(floor.center))
				end
			end
		end
	end
end

local function add_walls_to_room (map)
	for i=0,15 do
		for j=0,11 do
			if (i == 0 and j == 0) then
				add_tile(map, i, j, unpack(wall.topleft))
			elseif (i == 15 and j == 0) then
				add_tile(map, i, j, unpack(wall.topright))
			elseif (i == 0 and j == 11) then
				add_tile(map, i, j, unpack(wall.bottomleft))
			elseif (i == 15 and j == 11) then
				add_tile(map, i, j, unpack(wall.bottomright))
			elseif (i == 0 or i == 15) then
				add_tile(map, i, j, unpack(wall.vertical))
			elseif (j == 0 or j == 11) then
				add_tile(map, i, j, unpack(wall.horizontal))
			end
		end
	end
end

local function add_exit(map, direction)
	if direction > 4 then return end

	if direction == UP then
		add_tile(map, 6, 0, unpack(wall.bottomright))
		add_tile(map, 7, 0, unpack(floor.center))
		add_tile(map, 8, 0, unpack(floor.center))
		add_tile(map, 9, 0, unpack(wall.bottomleft))
	elseif direction == LEFT then
		add_tile(map, 0, 4, unpack(wall.bottomright))
		add_tile(map, 0, 5, unpack(floor.center))
		add_tile(map, 0, 6, unpack(floor.center))
		add_tile(map, 0, 7, unpack(wall.topright))
	elseif direction == RIGHT then
		add_tile(map, 15, 4, unpack(wall.bottomleft))
		add_tile(map, 15, 5, unpack(floor.center))
		add_tile(map, 15, 6, unpack(floor.center))
		add_tile(map, 15, 7, unpack(wall.topleft))
	elseif direction == DOWN then
		add_tile(map, 6, 11, unpack(wall.topright))
		add_tile(map, 7, 11, unpack(floor.center))
		add_tile(map, 8, 11, unpack(floor.center))
		add_tile(map, 9, 11, unpack(wall.topleft))
	end
end

function build_square_room(map, room)
	add_tiles_to_room(map);
	add_walls_to_room(map);
	for exit=1, #room.exits do
		add_exit(map, room.exits[exit]);
	end
end

local function build_vert_center_coridoor(map, offset)
	for j=0,4 do
		add_tile(map, 6, offset+j, unpack(wall.vertical));
		add_tile(map, 7, offset+j, unpack(floor.center));
		add_tile(map, 8, offset+j, unpack(floor.center));
		add_tile(map, 9, offset+j, unpack(wall.vertical));
	end
end

local function build_horiz_center_coridoor(map, offset)
	for i=0,6 do
		add_tile(map, offset+i, 4, unpack(wall.horizontal));
		add_tile(map, offset+i, 5, unpack(floor.center));
		add_tile(map, offset+i, 6, unpack(floor.center));
		add_tile(map, offset+i, 7, unpack(wall.horizontal));
	end
end

local function build_center_corner_walls(map, exits)
	if exits.down then
		if exits.left then
			add_tile(map, 6, 7, unpack(wall.topright))
		end
		if exits.right then
			add_tile(map, 9, 7, unpack(wall.topleft))
		end
	else
		if not exits.left then
			add_tile(map, 6, 7, unpack(wall.bottomleft))
		end
		if not exits.right then
			add_tile(map, 9, 7, unpack(wall.bottomright))
		end
	end
	if exits.up then
		if exits.left then
			add_tile(map, 6, 4, unpack(wall.bottomright))
		end
		if exits.right then
			add_tile(map, 9, 4, unpack(wall.bottomleft))
		end
	else
		if not exits.left then
			add_tile(map, 6, 4, unpack(wall.topleft))
		end
		if not exits.right then
			add_tile(map, 9, 4, unpack(wall.topright))
		end
	end
end

function build_coridoor_room(map, room)
	local exits = {
		up = false,
		down = false,
		left = false,
		right = false
	}

	for _,exit in ipairs(room.exits) do
		if UP == exit then exits.up = true
		elseif DOWN == exit then exits.down = true
		elseif LEFT == exit then exits.left = true
		elseif RIGHT == exit then exits.right = true
		end
	end

	-- Fill the center
	add_tile(map, 6, 5, unpack(wall.vertical))
	add_tile(map, 6, 6, unpack(wall.vertical))
	add_tile(map, 7, 4, unpack(wall.horizontal))
	add_tile(map, 7, 5, unpack(floor.center))
	add_tile(map, 7, 6, unpack(floor.center))
	add_tile(map, 7, 7, unpack(wall.horizontal))
	add_tile(map, 8, 4, unpack(wall.horizontal))
	add_tile(map, 8, 5, unpack(floor.center))
	add_tile(map, 8, 6, unpack(floor.center))
	add_tile(map, 8, 7, unpack(wall.horizontal))
	add_tile(map, 9, 5, unpack(wall.vertical))
	add_tile(map, 9, 6, unpack(wall.vertical))

	-- Build the coridoors
	if exits.down then build_vert_center_coridoor(map, 7) end
	if exits.up then build_vert_center_coridoor(map, 0) end
	if exits.left then build_horiz_center_coridoor(map, 0) end
	if exits.right then build_horiz_center_coridoor(map, 9) end

	build_center_corner_walls(map, exits)
end
