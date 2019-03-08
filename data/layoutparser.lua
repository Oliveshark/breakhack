local random = map_random
local pits = {}
local walls = {}
local fences = {}
local lights = {}
local walldecor = {}

local function readLayoutFile(file)
	local layoutfile = read_file(file)

	local cleanData = ""
	for i=1, #layoutfile do
		local c = layoutfile:sub(i+1, i+1)
		if c ~= " " and c ~= "\n" and c ~= "\r" then
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
		matrix[layout][col][row] = c
	end

	return matrix;
end

local function has_value(list, char)
	for _, value in ipairs(list) do
		if value == char then return true end
	end
	return false
end

local function getTileStateFor(matrix, i, j, c)
	local charList
	if type(c) == "string" then
		charList = { c }
	else
		charList = c
	end
	local above = has_value(charList, matrix[i][j-1])
	local below = has_value(charList, matrix[i][j+1])
	local left = has_value(charList, matrix[i-1][j])
	local right = has_value(charList, matrix[i+1][j])

	local above_left = has_value(charList, matrix[i-1][j-1])
	local above_right = has_value(charList, matrix[i+1][j-1])
	local below_left = has_value(charList, matrix[i-1][j+1])
	local below_right = has_value(charList, matrix[i+1][j+1])

	return above, below, left, right, above_left, above_right, below_left, below_right
end

local function getRandomWallDecorFrom(wallDecorations)
	return wallDecorations[random(#wallDecorations)]
end

local function setBlockTile(room, matrix, i, j, tiles, char, decor)
	local above, below, left, right, above_left, above_right, below_left, below_right = getTileStateFor(matrix, i, j, char);

	room.decor[i][j] = nil
	local tile = nil
	local decorTile = nil
	if above and below and left and right then
		tile = tiles.cross
	elseif not above and below and left and right then
		tile = tiles.top_t
	elseif not below and above and left and right then
		tile = tiles.bottom_t
		decorTile = getRandomWallDecorFrom(walldecor.bottom_t)
	elseif not left and above and below and right then
		tile = tiles.left_t
		decorTile = getRandomWallDecorFrom(walldecor.left_t)
	elseif not right and above and below and left then
		tile = tiles.right_t
		decorTile = getRandomWallDecorFrom(walldecor.right_t)
	elseif not above and not left and right and below then
		tile = tiles.topleft
		decorTile = getRandomWallDecorFrom(walldecor.topleft)
	elseif not above and not right and left and below then
		tile = tiles.topright
		decorTile = getRandomWallDecorFrom(walldecor.topright)
	elseif not below and not left and above and right then
		tile = tiles.bottomleft
		decorTile = getRandomWallDecorFrom(walldecor.bottomleft)
	elseif not below and not right and above and left then
		tile = tiles.bottomright
		decorTile = getRandomWallDecorFrom(walldecor.bottomright)
	elseif not left and not right and below then
		tile = tiles.left
		decorTile = getRandomWallDecorFrom(walldecor.left)
	elseif not above and not below and (left or right) then
		tile = tiles.top
		decorTile = getRandomWallDecorFrom(walldecor.top)
	else
		tile = tiles.single
		decorTile = getRandomWallDecorFrom(walldecor.single)
	end
	if decor then
		room.decor[i][j] = tile
	else
		room.tiles[i][j] = tile
		if random(8) == 1 then
			room.decor[i][j] = decorTile
		end
	end
end

local function setPitTile(room, matrix, i, j)
	local above, below, left, right, above_left, above_right, below_left, below_right = getTileStateFor(matrix, i, j, "p");

	room.decor[i][j] = nil
	if not above_left and not above_right and left and right and above then
		room.tiles[i][j] = pits.innermid
	elseif not above_left and left and above then
		room.tiles[i][j] = pits.innerleft
	elseif not above_right and right and above then
		room.tiles[i][j] = pits.innerright
	elseif not left and not above and not right then
		room.tiles[i][j] = pits.topcrevice
	elseif not left and not right then
		room.tiles[i][j] = pits.bottomcrevice
	elseif not left and not above then
		room.tiles[i][j] = pits.topleft
	elseif not right and not above then
		room.tiles[i][j] = pits.topright
	elseif not left then
		room.tiles[i][j] = pits.left
	elseif not right then
		room.tiles[i][j] = pits.right
	elseif not above then
		room.tiles[i][j] = pits.top
	else
		room.tiles[i][j] = pits.center
	end
end

local module = {}
function module.load_textures(map, wall_xoffset, wall_yoffset)
	local t_pit0 = add_texture(map, "Objects/Pit0.png")
	local t_pit1 = add_texture(map, "Objects/Pit1.png")
	local t_wall = add_texture(map, "Objects/Wall.png")
	local t_fence = add_texture(map, "Objects/Fence.png")
	local t_decor0 = add_texture(map, "Objects/Decor0.png")
	local t_decor1 = add_texture(map, "Objects/Decor1.png")

	local yo = (random(5) + random(3)) * (16 * 2)
	pits = {
		center			= { t_pit0, t_pit1, 16, yo + 16, false, false, false, true },
		top				= { t_pit0, t_pit1, 16, yo, false, false, false, true },
		left			= { t_pit0, t_pit1, 0, yo + 16, false, false, false, true },
		right			= { t_pit0, t_pit1, 32, yo + 16, false, false, false, true },
		topleft			= { t_pit0, t_pit1, 0, yo, false, false, false, true },
		topright		= { t_pit0, t_pit1, 32, yo, false, false, false, true },
		innerleft		= { t_pit0, t_pit1, 80, yo, false, false, false, true },
		innermid		= { t_pit0, t_pit1, 96, yo, false, false, false, true },
		innerright		= { t_pit0, t_pit1, 112, yo, false, false, false, true },
		topcrevice		= { t_pit0, t_pit1, 64, yo, false, false, false, true },
		bottomcrevice	= { t_pit0, t_pit1, 64, yo + 16, false, false, false, true },
	}

	local xo = wall_xoffset
	yo = wall_yoffset
	walls = {
		topleft			= { t_wall, nil, xo + 0, yo, true },
		top				= { t_wall, nil, xo + 16, yo, true },
		single			= { t_wall, nil, xo + 16, yo + 16, true },
		topright		= { t_wall, nil, xo + 32, yo, true },
		left			= { t_wall, nil, xo + 0, yo + 16, true },
		bottomleft		= { t_wall, nil, xo + 0, yo + 32, true },
		bottomright		= { t_wall, nil, xo + 32, yo + 32, true },
		center			= { t_wall, nil, xo + 48, yo, true },
		top_t			= { t_wall, nil, xo + 64, yo, true },
		left_t			= { t_wall, nil, xo + 48, yo + 16, true },
		cross			= { t_wall, nil, xo + 64, yo + 16, true },
		right_t			= { t_wall, nil, xo + 80, yo + 16, true },
		bottom_t		= { t_wall, nil, xo + 64, yo + 32, true },
	}

	yo = 48 * random(3)
	fences = {
		topleft			= { t_fence, nil, 0, yo, true },
		top				= { t_fence, nil, 16, yo, true },
		single			= { t_fence, nil, 0, yo + 16, true },
		topright		= { t_fence, nil, 32, yo, true },
		left			= { t_fence, nil, 0, yo + 16, true },
		bottomleft		= { t_fence, nil, 0, yo + 32, true },
		bottomright		= { t_fence, nil, 32, yo + 32, true },
		center			= { t_fence, nil, 48, yo, true },
		top_t			= { t_fence, nil, 64, yo, true },
		left_t			= { t_fence, nil, 48, yo + 16, true },
		cross			= { t_fence, nil, 64, yo + 16, true },
		right_t			= { t_fence, nil, 80, yo + 16, true },
		bottom_t		= { t_fence, nil, 64, yo + 32, true },
	}

	lights = {
		candle0 = { t_decor0, t_decor1,  3 * 16, 8 * 16, true, true },
		candle1 = { t_decor0, t_decor1,  1 * 16, 8 * 16, true, true },
		candle2 = { t_decor0, t_decor1,  5 * 16, 8 * 16, true, false },
	}
	walldecor = {
		topleft	= {
			{ t_decor0, nil,  2 * 16, 2 * 16, false },
			{ t_decor0, nil,  6 * 16, 2 * 16, false },
			{ t_decor0, nil,  7 * 16, 2 * 16, false },
		},
		top	= {
			{ t_decor0, nil,  0 * 16, 2 * 16, false },
			{ t_decor0, nil,  1 * 16, 2 * 16, false },
			{ t_decor0, nil,  4 * 16, 2 * 16, false },
			{ t_decor0, nil,  5 * 16, 2 * 16, false },
		},
		single = {
			{ t_decor0, nil,  0 * 16, 2 * 16, false },
			{ t_decor0, nil,  1 * 16, 2 * 16, false },
			{ t_decor0, nil,  4 * 16, 2 * 16, false },
			{ t_decor0, nil,  5 * 16, 2 * 16, false },
		},
		topright = {
			{ t_decor0, nil,  3 * 16, 2 * 16, false },
		},
		left = {
			{ t_decor0, nil,  2 * 16, 2 * 16, false },
			{ t_decor0, nil,  3 * 16, 2 * 16, false },
			{ t_decor0, nil,  6 * 16, 2 * 16, false },
			{ t_decor0, nil,  7 * 16, 2 * 16, false },
		},
		bottomleft = {
			{ t_decor0, nil,  0 * 16, 2 * 16, false },
			{ t_decor0, nil,  1 * 16, 2 * 16, false },
			{ t_decor0, nil,  2 * 16, 2 * 16, false },
			{ t_decor0, nil,  4 * 16, 2 * 16, false },
			{ t_decor0, nil,  5 * 16, 2 * 16, false },
			{ t_decor0, nil,  6 * 16, 2 * 16, false },
			{ t_decor0, nil,  7 * 16, 2 * 16, false },
		},
		bottomright = {
			{ t_decor0, nil,  0 * 16, 2 * 16, false },
			{ t_decor0, nil,  1 * 16, 2 * 16, false },
			{ t_decor0, nil,  3 * 16, 2 * 16, false },
			{ t_decor0, nil,  4 * 16, 2 * 16, false },
			{ t_decor0, nil,  5 * 16, 2 * 16, false },
		},
		left_t = {
			{ t_decor0, nil,  2 * 16, 2 * 16, false },
			{ t_decor0, nil,  6 * 16, 2 * 16, false },
			{ t_decor0, nil,  7 * 16, 2 * 16, false },
		},
		right_t = {
			{ t_decor0, nil,  3 * 16, 2 * 16, false },
		},
		bottom_t = {
			{ t_decor0, nil,  0 * 16, 2 * 16, false },
			{ t_decor0, nil,  1 * 16, 2 * 16, false },
			{ t_decor0, nil,  4 * 16, 2 * 16, false },
			{ t_decor0, nil,  5 * 16, 2 * 16, false },
		},
	}
end

function createJumbleLayout(matrix)
	local room1 = matrix[random(#matrix)]
	local room2 = matrix[random(#matrix)]
	local room3 = matrix[random(#matrix)]
	local room4 = matrix[random(#matrix)]

	local room = {}
	for i=0,15 do
		for j=0,12 do
			if not room[i] then room[i] = {} end
			if not room[i][j] then room[i][j] = {} end
			if i < 7 then
				if j < 6 then
					room[i][j] = room1[i][j]
				else
					room[i][j] = room2[i][j]
				end
			else
				if j < 6 then
					room[i][j] = room3[i][j]
				else
					room[i][j] = room4[i][j]
				end
			end
		end
	end

	return room
end

function draw_layout_to_room(room, matrix, roomx, roomy)
	for i=2,13 do
		for j=2,10 do
			if matrix[i][j] == "p" then
				setPitTile(room, matrix, i, j);
			elseif matrix[i][j] == "#" then
				setBlockTile(room, matrix, i, j, walls, {"#", "\"", "/"}, false)
			elseif matrix[i][j] == "\"" then
				setBlockTile(room, matrix, i, j, walls, {"#", "\"", "/"}, false)
				room.decor[i][j] = lights.candle1
			elseif matrix[i][j] == "/" then
				setBlockTile(room, matrix, i, j, walls, {"#", "\"", "/"}, false)
				if random(2) == 1 then
					room.decor[i][j] = lights.candle1
				else
					room.decor[i][j] = lights.candle2
				end
			elseif matrix[i][j] == "f" then
				setBlockTile(room, matrix, i, j, fences, "f", true)
			elseif matrix[i][j] == "a" then
				create_shop_artifact(map, (roomx*512) + i*32, (roomy * 384) + j*32)
			elseif matrix[i][j] == "l" then
				room.decor[i][j] = lights.candle0
			end
		end
	end
end

function pickALayout(matrix)
	-- Chose a random layout
	if random(2) == 1 then
		return matrix[random(#matrix)]
	else
		return createJumbleLayout(matrix)
	end
end

function module.add_walls_to_room(room)
	if random(3) ~= 1 then
		return false
	end

	draw_layout_to_room(room, pickALayout(readLayoutFile("walllayouts.dat")))
	return true
end

function module.add_pits_to_room(room)
	if CURRENT_LEVEL < 2 or random(5) ~= 1 then
		return false
	end

	draw_layout_to_room(room, pickALayout(readLayoutFile("pitlayouts.dat")))
	return true
end

function module.add_shop_layout(room, roomx, roomy)
	local matrix = readLayoutFile("shoplayouts.dat")
	draw_layout_to_room(room, matrix[random(#matrix)], roomx, roomy)
	return true
end

return module
