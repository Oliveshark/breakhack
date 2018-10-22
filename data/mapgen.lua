local room_builder = require "maproombuilder"
local monster_gen = require "monstergen"
local trap_gen = require "trapgen"
local chest_gen = require "chestgen"

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
			if matrix[i][j] then cov = cov + 1 end
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
			map_matrix[i][j] = nil
		end
	end

	local cx, cy = 1, 1
	local seed = time();
	info("Map generation seed: " .. seed)
	randomseed(seed)
	local direction = 0
	local lastDirection = 0
	local coridoor_count = 0
	local bossLevel = CURRENT_LEVEL % 5 == 0
	if QUICK_MODE then
		bossLevel = CURRENT_LEVEL % 3 == 0
	end
	local coverage = 8 + CURRENT_LEVEL
	if bossLevel or CURRENT_LEVEL == 1 then
		coverage = 5
	end

	-- Create the first room
	map_matrix[cx][cy] = room_builder.create_empty_room()

	while matrix_coverage(map_matrix) < coverage do
		local direction = random(4)

		if coridoor_count < coverage/2 then
			if random(3) == 1 and (cx > 1 or cy > 1) then
				map_matrix[cx][cy].type = "coridoor"
				coridoor_count = coridoor_count + 1
			end
		end

		valid_direction = false
		if direction == UP and cy > 1 then -- UP
			room_builder.add_exit(map_matrix[cx][cy], direction)
			map_matrix[cx][cy].path_dir = direction
			cy = cy - 1
			valid_direction = true
		elseif direction == LEFT and cx > 1 then -- LEFT
			room_builder.add_exit(map_matrix[cx][cy], direction)
			map_matrix[cx][cy].path_dir = direction
			cx = cx - 1
			valid_direction = true
		elseif direction == RIGHT and cx < 10 then -- RIGHT
			room_builder.add_exit(map_matrix[cx][cy], direction)
			map_matrix[cx][cy].path_dir = direction
			cx = cx + 1
			valid_direction = true
		elseif direction == DOWN and cy < 10 then -- DOWN
			room_builder.add_exit(map_matrix[cx][cy], direction)
			map_matrix[cx][cy].path_dir = direction
			cy = cy + 1
			valid_direction = true
		end

		-- Create the next room and add the reverse exit
		-- if a valid direction was found
		if valid_direction then
			if not map_matrix[cx][cy] then
				map_matrix[cx][cy] = room_builder.create_empty_room()
			end
			room_builder.add_exit(map_matrix[cx][cy], reverse_direction(direction))
		end
		lastDirection = direction
	end

	-- Last room rules
	map_matrix[cx][cy].goal = true
	map_matrix[cx][cy].type = "room"

	local roomCount = 0
	local bossAdded = false

	-- Build all the rooms
	for i=1,10 do
		for j=1,10 do
			room = map_matrix[i][j]
			if room then
				roomCount = roomCount + 1
				room_builder.build_room(room)
				monster_gen.add_monsters_to_room(room, i-1, j-1)
				trap_gen.add_traps_to_room(room, i-1, j-1)
				chest_gen.add_chests_to_room(room, i-1, j-1)
				if roomCount > 3 and bossLevel and not bossAdded then
					bossAdded = true
					monster_gen.add_boss_to_room(room, i-1, j-1)
				end
			end
		end
	end

	return map_matrix;
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
		if room then
			set_current_room(map, i-1, j-1);
			room_builder.load_room(map, room)
			monster_gen.load_monsters(map, room.monsters)
			trap_gen.load_traps(map, room.traps)
			chest_gen.load_chests(map, room.chests)
		end
	end
end
-- END SCRIPT
