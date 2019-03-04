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

local function setPitTile(room, matrix, i, j)
	if matrix[i][j] ~= "p" then
		return
	end

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
function module.load_textures(map)
	local t_pit0 = add_texture(map, "Objects/Pit0.png")
	local t_pit1 = add_texture(map, "Objects/Pit1.png")
	local t_wall = add_texture(map, "Objects/Wall.png")
	local t_fence = add_texture(map, "Objects/Fence.png")

	local yoffset = (random(5) + random(3)) * (16 * 2)
	pits = {
		center			= { t_pit0, t_pit1, 16, yoffset + 16, false, false, false, true },
		top				= { t_pit0, t_pit1, 16, yoffset, false, false, false, true },
		left			= { t_pit0, t_pit1, 0, yoffset + 16, false, false, false, true },
		right			= { t_pit0, t_pit1, 32, yoffset + 16, false, false, false, true },
		topleft			= { t_pit0, t_pit1, 0, yoffset, false, false, false, true },
		topright		= { t_pit0, t_pit1, 32, yoffset, false, false, false, true },
		innerleft		= { t_pit0, t_pit1, 80, yoffset, false, false, false, true },
		innermid		= { t_pit0, t_pit1, 96, yoffset, false, false, false, true },
		innerright		= { t_pit0, t_pit1, 112, yoffset, false, false, false, true },
		topcrevice		= { t_pit0, t_pit1, 64, yoffset, false, false, false, true },
		bottomcrevice	= { t_pit0, t_pit1, 64, yoffset + 16, false, false, false, true },
	}

	local yoffset = 48
	walls = {
		topleft			= { t_wall, nil, 0, yoffset, true },
		top				= { t_wall, nil, 16, yoffset, true },
		topright		= { t_wall, nil, 32, yoffset, true },
		left			= { t_wall, nil, 0, yoffset + 16, true },
		bottomleft		= { t_wall, nil, 0, yoffset + 32, true },
		bottomright		= { t_wall, nil, 32, yoffset + 32, true },
		top_t			= { t_wall, nil, 64, yoffset, true },
		left_t			= { t_wall, nil, 48, yoffset + 16, true },
		cross			= { t_wall, nil, 64, yoffset + 16, true },
		right_t			= { t_wall, nil, 80, yoffset + 16, true },
		bottom_t		= { t_wall, nil, 64, yoffset + 32, true },
	}

	local yoffset = 48
	fences = {
		topleft			= { t_fence, nil, 0, yoffset, true },
		top				= { t_fence, nil, 16, yoffset, true },
		topright		= { t_fence, nil, 32, yoffset, true },
		left			= { t_fence, nil, 0, yoffset + 16, true },
		bottomleft		= { t_fence, nil, 0, yoffset + 32, true },
		bottomright		= { t_fence, nil, 32, yoffset + 32, true },
		top_t			= { t_fence, nil, 64, yoffset, true },
		left_t			= { t_fence, nil, 48, yoffset + 16, true },
		cross			= { t_fence, nil, 64, yoffset + 16, true },
		right_t			= { t_fence, nil, 80, yoffset + 16, true },
		bottom_t		= { t_fence, nil, 64, yoffset + 32, true },
	}
end

function module.add_pits_to_room(room)
	--if CURRENT_LEVEL < 2 or random(5) ~= 1 then
		--return false
	--end

	local matrix = readLayoutFile("pitlayouts.dat")

	-- Chose a random layout
	matrix = matrix[random(#matrix)]
	for i=2,13 do
		for j=2,10 do
			setPitTile(room, matrix, i, j);
		end
	end

	return true
end

return module
