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

local special = { level_exit = nil }

local floorDecor = { }
local lightDecor = { }

local function load_decor_textures()
	td0 = add_texture(map, "Objects/Decor0.png")
	td1 = add_texture(map, "Objects/Decor1.png")

	-- Skulls
	table.insert(floorDecor, { td0, td1,      0, 12 * 16, false, false })
	table.insert(floorDecor, { td0, td1,     32, 12 * 16, false, false })
	table.insert(floorDecor, { td0, td1,     64, 12 * 16, false, true })
	table.insert(floorDecor, { td0, td1,      0, 13 * 16, false, false })
	table.insert(floorDecor, { td0, td1,     32, 13 * 16, false, false })
	table.insert(floorDecor, { td0, td1,     64, 13 * 16, false, true })

	-- Bones
	table.insert(floorDecor, { td0, td1,     16, 12 * 16, false, false })
	table.insert(floorDecor, { td0, td1,     48, 12 * 16, false, false })
	table.insert(floorDecor, { td0, td1,     16, 13 * 16, false, false })
	table.insert(floorDecor, { td0, td1,     48, 13 * 16, false, false })

	-- Urns
	table.insert(floorDecor, { td0, td1, 0 * 16,      48, true, false })
	table.insert(floorDecor, { td0, td1, 1 * 16,      48, true, false })
	table.insert(floorDecor, { td0, td1, 2 * 16,      48, true, false })
	table.insert(floorDecor, { td0, td1, 3 * 16,      48, true, false })
	table.insert(floorDecor, { td0, td1, 4 * 16,      48, true, false })
	table.insert(floorDecor, { td0, td1, 5 * 16,      48, true, false })
	table.insert(floorDecor, { td0, td1, 6 * 16,      48, false, false })
	table.insert(floorDecor, { td0, td1, 7 * 16,      48, false, false })

	-- Racks
	table.insert(floorDecor, { td0, td1, 0 * 16, 11 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 1 * 16, 11 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 2 * 16, 11 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 3 * 16, 11 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 4 * 16, 11 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 5 * 16, 11 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 6 * 16, 11 * 16, true, false })

	-- Headstones
	table.insert(floorDecor, { td0, td1, 0 * 16, 17 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 1 * 16, 17 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 2 * 16, 17 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 3 * 16, 17 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 4 * 16, 17 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 0 * 16, 18 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 1 * 16, 18 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 2 * 16, 18 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 3 * 16, 18 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 4 * 16, 18 * 16, true, false })

	-- Altars
	table.insert(floorDecor, { td0, td1, 0 * 16, 20 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 1 * 16, 20 * 16, true, false })
	table.insert(floorDecor, { td0, td1, 2 * 16, 20 * 16, true, false })

	-- Webs
	--table.insert(floorDecor, { td0, td1,  0 * 16, 19 * 16, false })
	--table.insert(floorDecor, { td0, td1,  1 * 16, 19 * 16, false })
	--table.insert(floorDecor, { td0, td1,  2 * 16, 19 * 16, false })
	--table.insert(floorDecor, { td0, td1,  3 * 16, 19 * 16, false })
	--table.insert(floorDecor, { td0, td1,  4 * 16, 19 * 16, false })

	-- Statues
	--table.insert(floorDecor, { td0, td1,  3 * 16, 20 * 16, true })
	--table.insert(floorDecor, { td0, td1,  4 * 16, 20 * 16, true })

	lightDecor.candle0 = { td0, td1,  0, 8 * 16, false, true }
	lightDecor.candle1 = { td0, td1, 16, 8 * 16, false, true }
	lightDecor.candle2 = { td0, td1, 32, 8 * 16, true, true }
end

local function load_special_tiles()
	tt = add_texture(map, "Objects/Tile.png")
	special.level_exit = { tt, -1, 16, 16, false, true, true }
end

local function repack(data)
	return {
		textureIndex0	= data[1],
		textureIndex1	= data[2] or -1,
		tileClipX		= data[3],
		tileClipY		= data[4],
		isCollider		= data[5] or false,
		isLightSource	= data[6] or false,
		isLevelExit		= data[7] or false,
	}
end

local function check_add_decoration(map, x, y, data)
	if tile_occupied(map, x, y) then
		return false
	end
	add_decoration(map, x, y, repack(data))
	return true
end

local function check_add_tile(map, x, y, data)
	if tile_occupied(map, x, y) then
		return false
	end
	add_tile(map, x, y, repack(data))
	return true
end

local function add_random_decor_to_room()
	local decor_count = random(4) - 1
	for i=1,decor_count do
		check_add_decoration(map, random(11)+1, random(8)+1, floorDecor[random(#floorDecor)])
	end
end

local function add_tiles_to_room (map)
	for i=0,15 do
		for j=0,11 do
			if (i >= 1 and i <= 14 and j >= 1 and j <= 10) then
				if (i == 1 and j == 1) then
					add_tile(map, i, j, repack(floor.topleft))
				elseif (i == 14 and j == 1) then
					add_tile(map, i, j, repack(floor.topright))
				elseif (i == 1 and j == 10) then
					add_tile(map, i, j, repack(floor.bottomleft))
				elseif (i == 14 and j == 10) then
					add_tile(map, i, j, repack(floor.bottomright))
				elseif (i == 1) then
					add_tile(map, i, j, repack(floor.left))
				elseif (i == 14) then
					add_tile(map, i, j, repack(floor.right))
				elseif (j == 1) then
					add_tile(map, i, j, repack(floor.top))
				elseif (j == 10) then
					add_tile(map, i, j, repack(floor.bottom))
				else
					add_tile(map, i, j, repack(floor.center))
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
				add_tile(map, i, j, repack(wall.topleft))
			elseif (i == 15 and j == 0) then
				add_tile(map, i, j, repack(wall.topright))
			elseif (i == 0 and j == 11) then
				add_tile(map, i, j, repack(wall.bottomleft))
			elseif (i == 15 and j == 11) then
				add_tile(map, i, j, repack(wall.bottomright))
			elseif (i == 0 or i == 15) then
				add_tile(map, i, j, repack(wall.vertical))
			elseif (j == 0 or j == 11) then
				add_tile(map, i, j, repack(wall.horizontal))
			end
		end
	end

	if random(2) == 1 then
		check_add_decoration(map, 4, 3, lightDecor.candle2)
	end
	if random(2) == 1 then
		check_add_decoration(map, 11, 3, lightDecor.candle2)
	end
	if random(2) == 1 then
		check_add_decoration(map, 4, 9, lightDecor.candle2)
	end
	if random(2) == 1 then
		check_add_decoration(map, 11, 9, lightDecor.candle2)
	end
end

local function add_exit(map, direction)
	if direction > 4 then return end

	if direction == UP then
		add_tile(map, 6, 0, repack(wall.bottomright))
		add_tile(map, 7, 0, repack(floor.singleleft))
		add_tile(map, 8, 0, repack(floor.singleright))
		add_tile(map, 9, 0, repack(wall.bottomleft))
	elseif direction == LEFT then
		add_tile(map, 0, 4, repack(wall.bottomright))
		add_tile(map, 0, 5, repack(floor.singletop))
		add_tile(map, 0, 6, repack(floor.singlebottom))
		add_tile(map, 0, 7, repack(wall.topright))
	elseif direction == RIGHT then
		add_tile(map, 15, 4, repack(wall.bottomleft))
		add_tile(map, 15, 5, repack(floor.singletop))
		add_tile(map, 15, 6, repack(floor.singlebottom))
		add_tile(map, 15, 7, repack(wall.topleft))
	elseif direction == DOWN then
		add_tile(map, 6, 11, repack(wall.topright))
		add_tile(map, 7, 11, repack(floor.singleleft))
		add_tile(map, 8, 11, repack(floor.singleright))
		add_tile(map, 9, 11, repack(wall.topleft))
	end
end

local function add_level_exit(map)
	success = false
	while not success do
		x = random(14)
		y = random(10)
		success = check_add_tile(map, x, y, special.level_exit)
	end
end

local function build_vert_center_coridoor(map, offset)
	for j=0,4 do
		add_tile(map, 6, offset+j, repack(wall.vertical));
		add_tile(map, 7, offset+j, repack(floor.center));
		add_tile(map, 8, offset+j, repack(floor.center));
		add_tile(map, 9, offset+j, repack(wall.vertical));
	end
	if random(2) == 1 then
		add_decoration(map, 6, offset + 2, repack(lightDecor.candle1))
	end
	if random(2) == 1 then 
		add_decoration(map, 9, offset + 2, repack(lightDecor.candle1))
	end
end

local function build_horiz_center_coridoor(map, offset)
	for i=0,6 do
		add_tile(map, offset+i, 4, repack(wall.horizontal));
		add_tile(map, offset+i, 5, repack(floor.center));
		add_tile(map, offset+i, 6, repack(floor.center));
		add_tile(map, offset+i, 7, repack(wall.horizontal));
	end
	if random(2) == 1 then
		check_add_decoration(map, offset+3, 4, lightDecor.candle1)
	end
	if random(2) == 1 then 
		check_add_decoration(map, offset+3, 7, lightDecor.candle1)
	end
end

local function build_center_corner_walls(map, exits)
	if exits.down then
		if exits.left then
			add_tile(map, 6, 7, repack(wall.topright))
		end
		if exits.right then
			add_tile(map, 9, 7, repack(wall.topleft))
		end
	else
		if not exits.left then
			add_tile(map, 6, 7, repack(wall.bottomleft))
		end
		if not exits.right then
			add_tile(map, 9, 7, repack(wall.bottomright))
		end
	end
	if exits.up then
		if exits.left then
			add_tile(map, 6, 4, repack(wall.bottomright))
		end
		if exits.right then
			add_tile(map, 9, 4, repack(wall.bottomleft))
		end
	else
		if not exits.left then
			add_tile(map, 6, 4, repack(wall.topleft))
		end
		if not exits.right then
			add_tile(map, 9, 4, repack(wall.topright))
		end
	end
end

local module = {}

function module.add_full_lighting(map)
		check_add_decoration(map, 4, 3, lightDecor.candle2)
		check_add_decoration(map, 11, 3, lightDecor.candle2)
		check_add_decoration(map, 4, 9, lightDecor.candle2)
		check_add_decoration(map, 11, 9, lightDecor.candle2)
end

function module.build_coridoor_room(map, room)
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
	add_tile(map, 6, 5, repack(wall.vertical))
	add_tile(map, 6, 6, repack(wall.vertical))
	add_tile(map, 7, 4, repack(wall.horizontal))
	add_tile(map, 7, 5, repack(floor.center))
	add_tile(map, 7, 6, repack(floor.center))
	add_tile(map, 7, 7, repack(wall.horizontal))
	add_tile(map, 8, 4, repack(wall.horizontal))
	add_tile(map, 8, 5, repack(floor.center))
	add_tile(map, 8, 6, repack(floor.center))
	add_tile(map, 8, 7, repack(wall.horizontal))
	add_tile(map, 9, 5, repack(wall.vertical))
	add_tile(map, 9, 6, repack(wall.vertical))

	-- Build the coridoors
	if exits.down then build_vert_center_coridoor(map, 7) end
	if exits.up then build_vert_center_coridoor(map, 0) end
	if exits.left then build_horiz_center_coridoor(map, 0) end
	if exits.right then build_horiz_center_coridoor(map, 9) end

	build_center_corner_walls(map, exits)
end

function module.create_room ()
	return {
		exits = {},
		active = false,
		goal = false,
		path_dir = 0,
		type = "room"
	}
end

function module.build_square_room(map, room)
	add_tiles_to_room(map);
	add_walls_to_room(map);
	for exit=1, #room.exits do
		add_exit(map, room.exits[exit]);
	end
	if room.goal then
		add_level_exit(map);
	end

	if CURRENT_LEVEL > 2 and random(10) == 1 then
		set_modifier(map, "WINDY");
	end
end

function module.load_textures(map)
	t_floor = add_texture(map, "Objects/Floor.png")
	t_wall = add_texture(map, "Objects/Wall.png")

	math.randomseed(os.time())
	local xo = (random(3) - 1) * 112
	local yo = (random(7)) * 48

	floor.center		= { t_floor, -1, xo + 16, yo + 16, false }
	floor.top			= { t_floor, -1, xo + 16, yo +  0, false }
	floor.bottom		= { t_floor, -1, xo + 16, yo + 32, false }
	floor.left			= { t_floor, -1, xo +  0, yo + 16, false }
	floor.right			= { t_floor, -1, xo + 32, yo + 16, false }
	floor.topleft		= { t_floor, -1, xo +  0, yo +  0, false }
	floor.topright		= { t_floor, -1, xo + 32, yo +  0, false }
	floor.bottomleft	= { t_floor, -1, xo +  0, yo + 32, false }
	floor.bottomright	= { t_floor, -1, xo + 32, yo + 32, false }
	floor.singletop		= { t_floor, -1, xo + 48, yo +  0, false }
	floor.singlebottom	= { t_floor, -1, xo + 48, yo + 32, false }
	floor.singleleft	= { t_floor, -1, xo + 64, yo + 16, false }
	floor.singleright	= { t_floor, -1, xo + 96, yo + 16, false }

	wall.topleft		= { t_wall, -1, xo +  0, yo +  0, true }
	wall.topright		= { t_wall, -1, xo + 32, yo +  0, true }
	wall.bottomleft		= { t_wall, -1, xo +  0, yo + 32, true }
	wall.bottomright	= { t_wall, -1, xo + 32, yo + 32, true }
	wall.vertical		= { t_wall, -1, xo +  0, yo + 16, true }
	wall.horizontal		= { t_wall, -1, xo + 16, yo +  0, true }

	load_decor_textures()
	load_special_tiles()
end

return module
