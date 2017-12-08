-- FUNCTIONS
local random = math.random

-- CONSTANTS
local UP	= 1
local LEFT	= 2
local RIGHT	= 3
local DOWN	= 4

local floor = {
	center = nil,
	top = nil,
	bottom = nil,
	left = nil,
	right = nil,
	topleft = nil,
	topright = nil,
	bottomleft = nil,
	bottomright = nil
}

local wall = {
	topleft = nil,
	topright = nil,
	bottomleft = nil,
	bottomright = nil,
	vertical = nil,
	horizontal = nil
}

local floorDecor = { }
local wallDecor = { }

local function load_decor_textures()
	td0 = add_texture(map, "assets/Objects/Decor0.png")
	td1 = add_texture(map, "assets/Objects/Decor1.png")

	-- Skulls
	table.insert(floorDecor, { td0, td1,      0, 12 * 16, false })
	table.insert(floorDecor, { td0, td1,     32, 12 * 16, false })
	table.insert(floorDecor, { td0, td1,     64, 12 * 16, false })
	table.insert(floorDecor, { td0, td1,      0, 13 * 16, false })
	table.insert(floorDecor, { td0, td1,     32, 13 * 16, false })
	table.insert(floorDecor, { td0, td1,     64, 13 * 16, false })

	-- Bones
	table.insert(floorDecor, { td0, td1,     16, 12 * 16, false })
	table.insert(floorDecor, { td0, td1,     48, 12 * 16, false })
	table.insert(floorDecor, { td0, td1,     16, 13 * 16, false })
	table.insert(floorDecor, { td0, td1,     48, 13 * 16, false })

	-- Urns
	table.insert(floorDecor, { td0, td1, 0 * 16,      48, true })
	table.insert(floorDecor, { td0, td1, 1 * 16,      48, true })
	table.insert(floorDecor, { td0, td1, 2 * 16,      48, true })
	table.insert(floorDecor, { td0, td1, 3 * 16,      48, true })
	table.insert(floorDecor, { td0, td1, 4 * 16,      48, true })
	table.insert(floorDecor, { td0, td1, 5 * 16,      48, true })
	table.insert(floorDecor, { td0, td1, 6 * 16,      48, false })
	table.insert(floorDecor, { td0, td1, 7 * 16,      48, false })

	-- Racks
	table.insert(floorDecor, { td0, td1, 0 * 16, 11 * 16, true })
	table.insert(floorDecor, { td0, td1, 1 * 16, 11 * 16, true })
	table.insert(floorDecor, { td0, td1, 2 * 16, 11 * 16, true })
	table.insert(floorDecor, { td0, td1, 3 * 16, 11 * 16, true })
	table.insert(floorDecor, { td0, td1, 4 * 16, 11 * 16, true })
	table.insert(floorDecor, { td0, td1, 5 * 16, 11 * 16, true })
	table.insert(floorDecor, { td0, td1, 6 * 16, 11 * 16, true })

	-- Headstones
	table.insert(floorDecor, { td0, td1, 0 * 16, 17 * 16, true })
	table.insert(floorDecor, { td0, td1, 1 * 16, 17 * 16, true })
	table.insert(floorDecor, { td0, td1, 2 * 16, 17 * 16, true })
	table.insert(floorDecor, { td0, td1, 3 * 16, 17 * 16, true })
	table.insert(floorDecor, { td0, td1, 4 * 16, 17 * 16, true })
	table.insert(floorDecor, { td0, td1, 0 * 16, 18 * 16, true })
	table.insert(floorDecor, { td0, td1, 1 * 16, 18 * 16, true })
	table.insert(floorDecor, { td0, td1, 2 * 16, 18 * 16, true })
	table.insert(floorDecor, { td0, td1, 3 * 16, 18 * 16, true })
	table.insert(floorDecor, { td0, td1, 4 * 16, 18 * 16, true })

	-- Altars
	table.insert(floorDecor, { td0, td1, 0 * 16, 20 * 16, true })
	table.insert(floorDecor, { td0, td1, 1 * 16, 20 * 16, true })
	table.insert(floorDecor, { td0, td1, 2 * 16, 20 * 16, true })

	-- Webs
	--table.insert(floorDecor, { td0, td1,  0 * 16, 19 * 16, false })
	--table.insert(floorDecor, { td0, td1,  1 * 16, 19 * 16, false })
	--table.insert(floorDecor, { td0, td1,  2 * 16, 19 * 16, false })
	--table.insert(floorDecor, { td0, td1,  3 * 16, 19 * 16, false })
	--table.insert(floorDecor, { td0, td1,  4 * 16, 19 * 16, false })

	-- Statues
	--table.insert(floorDecor, { td0, td1,  3 * 16, 20 * 16, true })
	--table.insert(floorDecor, { td0, td1,  4 * 16, 20 * 16, true })
	
	wallDecor.candle0 = { td0, td1,  0, 8 * 16, false }
	wallDecor.candle1 = { td0, td1, 16, 8 * 16, false }
end

function load_textures(map)
	t_floor = add_texture(map, "assets/Objects/Floor.png")
	t_wall = add_texture(map, "assets/Objects/Wall.png")

	math.randomseed(os.time())
	local xo = (random(3) - 1) * 112
	local yo = (random(7)) * 48

	floor.center		= { t_floor, xo + 16, yo + 16, false }
	floor.top			= { t_floor, xo + 16, yo +  0, false }
	floor.bottom		= { t_floor, xo + 16, yo + 32, false }
	floor.left			= { t_floor, xo +  0, yo + 16, false }
	floor.right			= { t_floor, xo + 32, yo + 16, false }
	floor.topleft		= { t_floor, xo +  0, yo +  0, false }
	floor.topright		= { t_floor, xo + 32, yo +  0, false }
	floor.bottomleft	= { t_floor, xo +  0, yo + 32, false }
	floor.bottomright	= { t_floor, xo + 32, yo + 32, false }
	floor.singletop		= { t_floor, xo + 48, yo +  0, false }
	floor.singlebottom	= { t_floor, xo + 48, yo + 32, false }
	floor.singleleft	= { t_floor, xo + 64, yo + 16, false }
	floor.singleright	= { t_floor, xo + 96, yo + 16, false }

	wall.topleft		= { t_wall, xo +  0, yo +  0, true }
	wall.topright		= { t_wall, xo + 32, yo +  0, true }
	wall.bottomleft		= { t_wall, xo +  0, yo + 32, true }
	wall.bottomright	= { t_wall, xo + 32, yo + 32, true }
	wall.vertical		= { t_wall, xo +  0, yo + 16, true }
	wall.horizontal		= { t_wall, xo + 16, yo +  0, true }

	load_decor_textures()
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

local function add_random_decor_to_room()
	local decor_count = random(4) - 1
	for i=1,decor_count do
		add_decoration(map, random(11)+1, random(8)+1, unpack(floorDecor[random(#floorDecor)]))
	end
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

	add_random_decor_to_room()
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
		add_tile(map, 7, 0, unpack(floor.singleleft))
		add_tile(map, 8, 0, unpack(floor.singleright))
		add_tile(map, 9, 0, unpack(wall.bottomleft))
	elseif direction == LEFT then
		add_tile(map, 0, 4, unpack(wall.bottomright))
		add_tile(map, 0, 5, unpack(floor.singletop))
		add_tile(map, 0, 6, unpack(floor.singlebottom))
		add_tile(map, 0, 7, unpack(wall.topright))
	elseif direction == RIGHT then
		add_tile(map, 15, 4, unpack(wall.bottomleft))
		add_tile(map, 15, 5, unpack(floor.singletop))
		add_tile(map, 15, 6, unpack(floor.singlebottom))
		add_tile(map, 15, 7, unpack(wall.topleft))
	elseif direction == DOWN then
		add_tile(map, 6, 11, unpack(wall.topright))
		add_tile(map, 7, 11, unpack(floor.singleleft))
		add_tile(map, 8, 11, unpack(floor.singleright))
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
