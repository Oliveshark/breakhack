local random = map_random
local pits = {}
local walls = {}
local fences = {}

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

local function getTileStateFor(matrix, i, j, c)
	local above = matrix[i][j-1] == c;
	local below = matrix[i][j+1] == c;
	local left = matrix[i-1][j] == c;
	local right = matrix[i+1][j] == c;

	local above_left = matrix[i-1][j-1] == c;
	local above_right = matrix[i+1][j-1] == c;
	local below_left = matrix[i-1][j+1] == c;
	local below_right = matrix[i+1][j+1] == c;

	return above, below, left, right, above_left, above_right, below_left, below_right
end

local function setBlockTile(room, matrix, i, j, tiles, char, decorTiles)
	local above, below, left, right, above_left, above_right, below_left, below_right = getTileStateFor(matrix, i, j, char);

	room.decor[i][j] = nil
	local tile = nil
	if above and below and left and right then
		tile = tiles.cross
	elseif not above and below and left and right then
		tile = tiles.top_t
	elseif not below and above and left and right then
		tile = tiles.bottom_t
	elseif not left and above and below and right then
		tile = tiles.left_t
	elseif not right and above and below and left then
		tile = tiles.right_t
	elseif not above and not left and right and below then
		tile = tiles.topleft
	elseif not above and not right and left and below then
		tile = tiles.topright
	elseif not below and not left and above and right then
		tile = tiles.bottomleft
	elseif not below and not right and above and left then
		tile = tiles.bottomright
	elseif not left and not right and below then
		tile = tiles.left
	elseif not above and not below and (left or right) then
		tile = tiles.top
	else
		tile = tiles.single
	end
	if decorTiles then
		room.decor[i][j] = tile
	else
		room.tiles[i][j] = tile
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

	yo = 48
	fences = {
		topleft			= { t_fence, nil, 0, yo, true },
		top				= { t_fence, nil, 16, yo, true },
		single			= { t_fence, nil, 16, yo + 16, true },
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
end

function draw_layout_to_room(room, matrix)
	-- Chose a random layout
	matrix = matrix[random(#matrix)]
	for i=2,13 do
		for j=2,10 do
			if matrix[i][j] == "p" then
				setPitTile(room, matrix, i, j);
			elseif matrix[i][j] == "#" then
				setBlockTile(room, matrix, i, j, walls, "#", false)
			elseif matrix[i][j] == "f" then
				setBlockTile(room, matrix, i, j, fences, "f", true)
			end
		end
	end
end

function module.add_walls_to_room(room)
	if random(4) ~= 1 then
		return false
	end

	draw_layout_to_room(room, readLayoutFile("walllayouts.dat"))
	return true
end

function module.add_pits_to_room(room)
	if CURRENT_LEVEL < 2 or random(5) ~= 1 then
		return false
	end

	draw_layout_to_room(room, readLayoutFile("pitlayouts.dat"))
	return true
end

return module
