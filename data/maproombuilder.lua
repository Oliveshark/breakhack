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

local pits = {
	center = nil,
	top = nil,
	left = nil,
	right = nil,
	topleft = nil,
	topright = nil,
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
	--table.insert(floorDecor, { td0, td1, 0 * 16,      48, true, false })
	--table.insert(floorDecor, { td0, td1, 1 * 16,      48, true, false })
	--table.insert(floorDecor, { td0, td1, 2 * 16,      48, true, false })
	--table.insert(floorDecor, { td0, td1, 3 * 16,      48, true, false })
	--table.insert(floorDecor, { td0, td1, 4 * 16,      48, true, false })
	--table.insert(floorDecor, { td0, td1, 5 * 16,      48, true, false })
	--table.insert(floorDecor, { td0, td1, 6 * 16,      48, false, false })
	--table.insert(floorDecor, { td0, td1, 7 * 16,      48, false, false })

	-- Racks
	--table.insert(floorDecor, { td0, td1, 0 * 16, 11 * 16, true, false })
	--table.insert(floorDecor, { td0, td1, 1 * 16, 11 * 16, true, false })
	--table.insert(floorDecor, { td0, td1, 2 * 16, 11 * 16, true, false })
	--table.insert(floorDecor, { td0, td1, 3 * 16, 11 * 16, true, false })
	--table.insert(floorDecor, { td0, td1, 4 * 16, 11 * 16, true, false })
	--table.insert(floorDecor, { td0, td1, 5 * 16, 11 * 16, true, false })
	--table.insert(floorDecor, { td0, td1, 6 * 16, 11 * 16, true, false })

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
		isLethal		= data[8] or false
	}
end

local function add_random_decor_to_room(room)
	local decor_count = random(10) - 1
	for i=1,decor_count do
		x = random(11) + 1
		y = random(8) + 1
		if not room.decor[x][y] then
			room.decor[x][y] = floorDecor[random(#floorDecor)]
		end
	end

	if random(2) == 1 then
		room.decor[4][3] = lightDecor.candle2
	end
	if random(2) == 1 then
		room.decor[11][3] = lightDecor.candle2
	end
	if random(2) == 1 then
		room.decor[4][9] = lightDecor.candle2
	end
	if random(2) == 1 then
		room.decor[11][9] = lightDecor.candle2
	end
end

local function add_pits_to_room(room)

	if CURRENT_LEVEL < 3 then
		return
	elseif random(5) ~= 1 then
		return
	end

	local pitdata = read_file("pitlayouts.dat")

	local cleanData = ""
	for i=1, #pitdata do
		local c = pitdata:sub(i+1, i+1)
		if c == "#" or c == "-" then
			cleanData = cleanData .. c
		end
	end

	local matrix = {}
	for i=0, #cleanData-1 do
		local c = cleanData:sub(i, i)
		local col = i % 16
		local row = (i - col)/16
		local layout = 1 + (row - (row % 12))/12
		local row = row % 12
		if not matrix[layout] then matrix[layout] = {} end
		if not matrix[layout][col] then matrix[layout][col] = {} end
		if c == "#" then
			matrix[layout][col][row] = true
		else
			matrix[layout][col][row] = false
		end
	end
	
	-- Chose a random layout
	matrix = matrix[random(#matrix)]
	for i=2,13 do
		for j=2,10 do
			if matrix[i][j] then
				room.decor[i][j] = nil
				if not matrix[i-1][j-1] and not matrix[i+1][j-1] and matrix[i-1][j] and matrix[i+1][j] and matrix[i][j-1] then
					room.tiles[i][j] = pits.innermid
				elseif not matrix[i-1][j-1] and matrix[i-1][j] and matrix[i][j-1] then
					room.tiles[i][j] = pits.innerleft
				elseif not matrix[i+1][j-1] and matrix[i+1][j] and matrix[i][j-1] then
					room.tiles[i][j] = pits.innerright
				elseif not matrix[i-1][j] and not matrix[i][j-1] and not matrix[i+1][j] then
					room.tiles[i][j] = pits.topcrevice
				elseif not matrix[i-1][j] and not matrix[i+1][j] then
					room.tiles[i][j] = pits.bottomcrevice
				elseif not matrix[i-1][j] and not matrix[i][j-1] then
					room.tiles[i][j] = pits.topleft
				elseif not matrix[i+1][j] and not matrix[i][j-1] then
					room.tiles[i][j] = pits.topright
				elseif not matrix[i-1][j] then
					room.tiles[i][j] = pits.left
				elseif not matrix[i+1][j] then
					room.tiles[i][j] = pits.right
				elseif not matrix[i][j-1] then
					room.tiles[i][j] = pits.top
				else
					room.tiles[i][j] = pits.center
				end
			end
		end
	end
end

local function add_tiles_to_room (room)
	for i=0,15 do
		for j=0,11 do
			if (i >= 1 and i <= 14 and j >= 1 and j <= 10) then
				if (i == 1 and j == 1) then
					room.tiles[i][j] = floor.topleft
				elseif (i == 14 and j == 1) then
					room.tiles[i][j] = floor.topright
				elseif (i == 1 and j == 10) then
					room.tiles[i][j] = floor.bottomleft
				elseif (i == 14 and j == 10) then
					room.tiles[i][j] = floor.bottomright
				elseif (i == 1) then
					room.tiles[i][j] = floor.left
				elseif (i == 14) then
					room.tiles[i][j] = floor.right
				elseif (j == 1) then
					room.tiles[i][j] = floor.top
				elseif (j == 10) then
					room.tiles[i][j] = floor.bottom
				else
					room.tiles[i][j] = floor.center
				end
			end
		end
	end
end

local function add_walls_to_room (room)
	for i=0,15 do
		for j=0,11 do
			if (i == 0 and j == 0) then
				room.tiles[i][j] = wall.topleft
			elseif (i == 15 and j == 0) then
				room.tiles[i][j] = wall.topright
			elseif (i == 0 and j == 11) then
				room.tiles[i][j] = wall.bottomleft
			elseif (i == 15 and j == 11) then
				room.tiles[i][j] = wall.bottomright
			elseif (i == 0 or i == 15) then
				room.tiles[i][j] = wall.vertical
			elseif (j == 0 or j == 11) then
				room.tiles[i][j] = wall.horizontal
			end
		end
	end
end

local function build_vert_center_coridoor(room, offset)
	for i=0,4 do
		room.tiles[6][offset+i] = wall.vertical
		room.tiles[7][offset+i] = floor.center
		room.tiles[8][offset+i] = floor.center
		room.tiles[9][offset+i] = wall.vertical
	end
	if random(2) == 1 then
		room.decor[6][offset+2] = lightDecor.candle1
	end
	if random(2) == 1 then 
		room.decor[6][offset+2] = lightDecor.candle1
	end
end

local function build_horiz_center_coridoor(room, offset)
	for i=0,6 do
		room.tiles[offset+i][4] = wall.horizontal
		room.tiles[offset+i][5] = floor.center
		room.tiles[offset+i][6] = floor.center
		room.tiles[offset+i][7] = wall.horizontal
	end
	if random(2) == 1 then
		room.decor[offset+3][4] = lightDecor.candle1
	end
	if random(2) == 1 then 
		room.decor[offset+3][7] = lightDecor.candle1
	end
end

local function build_center_corner_walls(room, exits)
	if exits.down then
		if exits.left then
			room.tiles[6][7] = wall.topright
		end
		if exits.right then
			room.tiles[9][7] = wall.topleft
		end
	else
		if not exits.left then
			room.tiles[6][7] = wall.bottomleft
		end
		if not exits.right then
			room.tiles[9][7] = wall.bottomright
		end
	end
	if exits.up then
		if exits.left then
			room.tiles[6][4] = wall.bottomright
		end
		if exits.right then
			room.tiles[9][4] = wall.bottomleft
		end
	else
		if not exits.left then
			room.tiles[6][4] = wall.topleft
		end
		if not exits.right then
			room.tiles[9][4] = wall.topright
		end
	end
end

local function add_exits_to_room(room)
	for _,direction in ipairs(room.exits) do
		if direction == UP then
			room.tiles[6][0] = wall.bottomright
			room.tiles[7][0] = floor.singleleft
			room.tiles[8][0] = floor.singleright
			room.tiles[9][0] = wall.bottomleft
		elseif direction == LEFT then
			room.tiles[0][4] = wall.bottomright
			room.tiles[0][5] = floor.singletop
			room.tiles[0][6] = floor.singlebottom
			room.tiles[0][7] = wall.topright
		elseif direction == RIGHT then
			room.tiles[15][4] = wall.bottomleft
			room.tiles[15][5] = floor.singletop
			room.tiles[15][6] = floor.singlebottom
			room.tiles[15][7] = wall.topleft
		elseif direction == DOWN then
			room.tiles[6][11] = wall.topright
			room.tiles[7][11] = floor.singleleft
			room.tiles[8][11] = floor.singleright
			room.tiles[9][11] = wall.topleft
		end
	end
end

local function build_coridoor_room(room)
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
	room.tiles[6][5] = wall.vertical
	room.tiles[6][6] = wall.vertical
	room.tiles[7][4] = wall.horizontal
	room.tiles[7][5] = floor.center
	room.tiles[7][6] = floor.center
	room.tiles[7][7] = wall.horizontal
	room.tiles[8][4] = wall.horizontal
	room.tiles[8][5] = floor.center
	room.tiles[8][6] = floor.center
	room.tiles[8][7] = wall.horizontal
	room.tiles[9][5] = wall.vertical
	room.tiles[9][6] = wall.vertical

	-- Build the coridoors
	if exits.down then build_vert_center_coridoor(room, 7) end
	if exits.up then build_vert_center_coridoor(room, 0) end
	if exits.left then build_horiz_center_coridoor(room, 0) end
	if exits.right then build_horiz_center_coridoor(room, 9) end

	build_center_corner_walls(room, exits)
end

local function add_level_exit(room)
	success = false
	while not success do
		x = random(14)
		y = random(10)
		if not room.decor[x][y]
			and not room.traps[x][y]
			and (room.tiles[x][y]
				and not room.tiles[x][y][5]
				and not room.tiles[x][y][8])
			then
			success = true
			room.tiles[x][y] = special.level_exit
		end
	end
end

local function build_normal_room(room)
	add_tiles_to_room(room)
	add_random_decor_to_room(room)
	add_walls_to_room(room)
	add_exits_to_room(room)
	add_pits_to_room(room)

	if room.goal then
		add_level_exit(room)
	end

	if CURRENT_LEVEL > 3 and random(10) == 1 then
		directions = { "LEFT", "RIGHT", "UP", "DOWN" }
		room.modifier.type = "WINDY"
		room.modifier.arg = directions[random(#directions)]
	end

	return room
end

local module = {}

function module.add_exit(room, direction)
	if direction > 4 then
		return
	end

	table.insert(room.exits, direction)
end

function module.add_full_lighting(room)
		room.decor[4][3] = lightDecor.candle2
		room.decor[11][3] = lightDecor.candle2
		room.decor[4][9] = lightDecor.candle2
		room.decor[11][9] = lightDecor.candle2
end

function module.create_empty_room()
	room = {
		exits = {},
		active = false,
		goal = false,
		path_dir = 0,
		type = "room",
		tiles = {},
		decor = {},
		modifier = {
			type = nil,
			arg = nil
		},
		monsters = {},
		traps = {}
	}
	for i=0,15 do
		room.tiles[i] = {}
		room.decor[i] = {}
		room.monsters[i] = {}
		room.traps[i] = {}
		for j=0,11 do
			room.tiles[i][j] = nil
			room.decor[i][j] = nil
			room.monsters[i][j] = nil
			room.traps[i][j] = nil
		end
	end
	return room
end

function module.build_room(room)
	if room.type == "coridoor" then
		build_coridoor_room(room)
	else
		build_normal_room(room)
	end
end

function module.load_room(map, room)
	for i=0, 15 do
		for j=0, 11 do
			if room.tiles[i][j] then
				add_tile(map, i, j, repack(room.tiles[i][j]))
			end
			if room.decor[i][j] then
				add_decoration(map, i, j, repack(room.decor[i][j]))
			end
		end
	end
	if room.modifier.type then
		set_modifier(map, room.modifier.type, room.modifier.arg)
	end
end

function module.load_textures(map)
	t_floor = add_texture(map, "Objects/Floor.png")
	t_wall = add_texture(map, "Objects/Wall.png")
	t_pit0 = add_texture(map, "Objects/Pit0.png")
	t_pit1 = add_texture(map, "Objects/Pit1.png")

	math.randomseed(os.time())
	local xo = (random(3) - 1) * 112
	local yo = (random(8)) * 48

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

	local pit_yo = (random(5) + random(3)) * (16 * 2)
	pits.topleft		= { t_pit0, t_pit1, 0, pit_yo, false, false, false, true }
	pits.top			= { t_pit0, t_pit1, 16, pit_yo, false, false, false, true }
	pits.topright		= { t_pit0, t_pit1, 32, pit_yo, false, false, false, true }
	pits.left	   		= { t_pit0, t_pit1, 0, pit_yo + 16, false, false, false, true }
	pits.center			= { t_pit0, t_pit1, 16, pit_yo + 16, false, false, false, true }
	pits.right			= { t_pit0, t_pit1, 32, pit_yo + 16, false, false, false, true }
	pits.innerleft		= { t_pit0, t_pit1, 80, pit_yo, false, false, false, true }
	pits.innermid		= { t_pit0, t_pit1, 96, pit_yo, false, false, false, true }
	pits.innerright		= { t_pit0, t_pit1, 112, pit_yo, false, false, false, true }
	pits.topcrevice		= { t_pit0, t_pit1, 64, pit_yo, false, false, false, true }
	pits.bottomcrevice	= { t_pit0, t_pit1, 64, pit_yo + 16, false, false, false, true }

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
