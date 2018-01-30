local room_builder = require "data/maproombuilder"
local monster_gen = require "data/monstergen"

-- Setting up some functions
local time = os.time
local random = math.random
local randomseed = math.randomseed

-- CONSTANTS
local UP	= 1
local LEFT	= 2
local RIGHT	= 3
local DOWN	= 4

-- BEGIN FUNCTIONS
local function matrix_coverage (matrix)
	local cov = 0
	for i=1,10 do
		for j=1,10 do
			if matrix[i][j].active then cov = cov + 1 end
		end
	end
	return cov
end

local function reverse_direction(dir)
	if dir == UP then return DOWN
	elseif dir == DOWN then return UP
	elseif dir == LEFT then return RIGHT
	elseif dir == RIGHT then return LEFT
	end
end

local function generate_path ()
	local map_matrix = {}
	for i=1,10 do
		map_matrix[i] = {}
		for j=1,10 do
			map_matrix[i][j] = room_builder.create_room()
		end
	end

	local cx, cy = 1, 1
	local seed = time();
	print("[**] Map generation seed: " .. seed)
	randomseed(seed)
	local direction = 0
	local lastDirection = 0
	local coridoor_count = 0
	local coverage = 10

	while matrix_coverage(map_matrix) < coverage do
		local direction = random(4)

		if lastDirection > 0 then
			if random(24) <= 6 then direction = lastDirection end
		end

		while lastDirection == reverse_direction(direction) do
			direction = random(4)
		end

		map_matrix[cx][cy].active = true

		if coridoor_count < coverage/3 then
			if random(3) == 1 and (cx > 1 or cy > 1) then
				map_matrix[cx][cy].type = "coridoor"
				coridoor_count = coridoor_count + 1
			end
		end

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

	-- Last room rules
	map_matrix[cx][cy].active = true
	map_matrix[cx][cy].goal = true
	map_matrix[cx][cy].type = "room"

	return map_matrix;
end

local function print_matrix(matrix)
	for i=1,10 do
		for j=1,10 do
			if not map_matrix[j][i].goal then
				io.write(map_matrix[j][i].path_dir .. " ")
			else
				io.write("G ")
			end
		end
		io.write("\n")
	end
end
-- END FUNCTIONS

-- BEGIN SCRIPT
map = create_map(CURRENT_LEVEL) -- 'map' needs to be global
room_builder.load_textures(map)
local map_matrix = generate_path()

-- Print path [Debug]
-- print_matrix(map_matrix)

for i=1,10 do
	for j=1,10 do
		local room = map_matrix[i][j]
		if room.active then
			set_current_room(map, i-1, j-1);
			if room.type == "room" then
				room_builder.build_square_room(map, room)
				monster_gen.add_monster_to_room(map, i-1, j-1);
			elseif room.type == "coridoor" then
				room_builder.build_coridoor_room(map, room)
			end
		end
	end
end
-- END SCRIPT
