require "data/maproombuilder"

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
function matrix_coverage (matrix)
	local cov = 0
	for i=1,10 do
		for j=1,10 do
			if matrix[i][j].active then cov = cov + 1 end
		end
	end
	return cov
end

function reverse_direction(dir)
	if dir == UP then return DOWN
	elseif dir == DOWN then return UP
	elseif dir == LEFT then return RIGHT
	elseif dir == RIGHT then return LEFT
	end
end

function generate_path ()
	local map_matrix = {}
	for i=1,10 do
		map_matrix[i] = {}
		for j=1,10 do
			map_matrix[i][j] = create_room()
		end
	end

	local cx, cy = 1, 1
	local seed = time();
	print("[**] Map generation seed: " .. seed)
	randomseed(seed)
	local direction = 0;
	local lastDirection = 0;
	while matrix_coverage(map_matrix) < 10 do
		local direction = random(4)

		if lastDirection > 0 then
			if random(24) <= 8 then direction = lastDirection end
		end

		map_matrix[cx][cy].active = true
		if direction == UP and cy > 1 then -- UP
			table.insert(map_matrix[cx][cy].exits, direction)
			map_matrix[cx][cy].path_dir = direction
			cy = cy - 1;
			table.insert(map_matrix[cx][cy].exits, reverse_direction(direction))
		elseif direction == LEFT and cx > 1 then -- LEFT
			table.insert(map_matrix[cx][cy].exits, direction)
			map_matrix[cx][cy].path_dir = direction
			cx = cx - 1;
			table.insert(map_matrix[cx][cy].exits, reverse_direction(direction))
		elseif direction == RIGHT and cx < 10 then -- RIGHT
			table.insert(map_matrix[cx][cy].exits, direction)
			map_matrix[cx][cy].path_dir = direction
			cx = cx + 1;
			table.insert(map_matrix[cx][cy].exits, reverse_direction(direction))
		elseif direction == DOWN and cy < 10 then -- DOWN
			table.insert(map_matrix[cx][cy].exits, direction)
			map_matrix[cx][cy].path_dir = direction
			cy = cy + 1;
			table.insert(map_matrix[cx][cy].exits, reverse_direction(direction))
		end
		lastDirection = direction
	end
	map_matrix[cx][cy].active = true -- Last room
	map_matrix[cx][cy].goal = true -- Last room

	return map_matrix;
end
-- END FUNCTIONS

-- BEGIN SCRIPT
map = create_map() -- 'map' needs to be global
load_textures(map)
local map_matrix = generate_path()

-- Print path [Debug]
--for i=1,10 do
	--for j=1,10 do
		--if not map_matrix[j][i].goal then
			--io.write(map_matrix[j][i].path_dir .. " ")
		--else
			--io.write("G ")
		--end
	--end
	--io.write("\n")
--end

for i=1,10 do
	for j=1,10 do
		if map_matrix[i][j].active then
			set_current_room(map, i-1, j-1);
			add_tiles_to_room(map);
			add_walls_to_room(map);
			for exit=1, #map_matrix[i][j].exits do
				add_exit(map, map_matrix[i][j].exits[exit]);
			end
		end
	end
end
-- END SCRIPT
