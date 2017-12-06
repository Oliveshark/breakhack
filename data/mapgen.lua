-- Setting up some functions
local time = os.time
local random = math.random
local randomseed = math.randomseed

-- CONSTANTS
UP		= 1
LEFT	= 2
RIGHT	= 3
DOWN	= 4

-- BEGIN FUNCTIONS
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

function matrix_coverage (matrix)
	local cov = 0
	for i=1,10 do
		for j=1,10 do
			if matrix[i][j] > 0 then cov = cov + 1 end
		end
	end
	return cov
end

function create_room ()
	local room = {}
	room.exits = {}
end

function generate_path ()
	local map_matrix = {}
	for i=1,10 do
		map_matrix[i] = {}
		for j=1,10 do
			map_matrix[i][j] = 0
		end
	end

	local cx, cy = 1, 1
	local seed = time();
	print("[**] Map generation seed: " .. seed)
	randomseed(seed)
	local direction = 0;
	local lastDirection = 0;
	while matrix_coverage(map_matrix) < 30 do
		local direction = random(4)

		if lastDirection > 0 then
			if random(24) <= 8 then direction = lastDirection end
		end

		if direction == UP and cy > 1 then -- UP
			map_matrix[cx][cy] = direction
			cy = cy - 1;
		elseif direction == LEFT and cx > 1 then -- LEFT
			map_matrix[cx][cy] = direction
			cx = cx - 1;
		elseif direction == RIGHT and cx < 10 then -- RIGHT
			map_matrix[cx][cy] = direction
			cx = cx + 1;
		elseif direction == DOWN and cy < 10 then -- DOWN
			map_matrix[cx][cy] = direction
			cy = cy + 1;
		end
		lastDirection = direction
	end
	map_matrix[cx][cy] = 5 -- The final room

	return map_matrix;
end

function add_exit(map, direction, floorTexture, wallTexture)
	if direction > 4 then return end

	if direction == UP then
		add_tile(map, 7, 0, floorTexture, 16, 64, false)
	elseif direction == LEFT then
		add_tile(map, 0, 5, floorTexture, 16, 64, false)
	elseif direction == RIGHT then
		add_tile(map, 15, 5, floorTexture, 16, 64, false)
	elseif direction == DOWN then
		add_tile(map, 7, 11, floorTexture, 16, 64, false)
	end
end

function add_reverse_exit(map, direction, floorTexture, wallTexture)
	local reverseDirection

	if direction == UP then reverseDirection = DOWN
	elseif direction == DOWN then reverseDirection = UP
	elseif direction == LEFT then reverseDirection = RIGHT
	elseif direction == RIGHT then reverseDirection = LEFT
	else reverseDirection = 5 end

	add_exit(map, reverseDirection, floorTexture, wallTexture)
end
-- END FUNCTIONS

-- BEGIN SCRIPT
map = create_map() -- 'map' needs to be global
local floorTexture = add_texture(map, "assets/Objects/Floor.png")
local wallTexture = add_texture(map, "assets/Objects/Wall.png")
local map_matrix = generate_path()

-- Print path [Debug]
for i=1,10 do
	for j=1,10 do
		io.write(map_matrix[j][i] .. " ")
	end
	io.write("\n")
end

for i=1,10 do
	for j=1,10 do
		if map_matrix[i][j] > 0 then
			set_current_room(map, i-1, j-1);
			add_tiles_to_room(map, floorTexture);
			add_walls_to_room(map, wallTexture);
		end
	end
end
for i=1,10 do
	for j=1,10 do
		if map_matrix[i][j] > 0 then
			set_current_room(map, i-1, j-1);
			add_exit(map, map_matrix[i][j], floorTexture, wallTexture);
			if map_matrix[i][j] == UP then
				set_current_room(map, i-1, j-2)
			elseif map_matrix[i][j] == LEFT then
				set_current_room(map, i-2, j-1)
			elseif map_matrix[i][j] == RIGHT then
				set_current_room(map, i, j-1)
			elseif map_matrix[i][j] == DOWN then
				set_current_room(map, i-1, j)
			end
			add_reverse_exit(map, map_matrix[i][j], floorTexture, wallTexture);
		end
	end
end
-- END SCRIPT
