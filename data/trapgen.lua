local room_builder = require "maproombuilder"
local module = {}
local random = math.random

local textures = {
	"Objects/Trap1.png",
	"Objects/Trap0.png"
}

local traps = {}
for i=1,7 do
	trap = { textures[1], textures[2], i*16, 3*16, CURRENT_LEVEL * 4 }
	table.insert(traps, trap)
end

local function repack(data)
	return {
		texturePath1	= data[1],
		texturePath2	= data[2],
		clipX			= data[3],
		clipY			= data[4],
		damage			= data[5]
	}
end

function module.add_traps_to_room(room)
	if room.type == "coridoor" then
		if random(2) ~= 1 then return end
	else
		if random(4) ~= 1 then return end
	end

	local count = random(4)
	local i = 0
	while i < count do
		local rx = random(13) + 1
		local ry = random(9) + 1
		if CURRENT_LEVEL < 4 then
			if room_builder.is_tile_avilable(room, rx, ry)
				and not room.traps[rx+1][ry]
				and not room.traps[rx-1][ry]
				and not room.traps[rx][ry+1]
				and not room.traps[rx][ry-1]
				and not room.traps[rx+1][ry+1]
				and not room.traps[rx+1][ry-1]
				and not room.traps[rx-1][ry+1]
				and not room.traps[rx-1][ry-1]
				then
				room.traps[rx][ry] = traps[random(#traps)]
				i = i + 1
			end
		else
			if room_builder.is_tile_avilable(room, rx, ry) then
				room.traps[rx][ry] = traps[random(#traps)]
				i = i + 1
			end
		end
	end
end

function module.load_traps(map, traps)
	for i=0,15 do
		for j=0,11 do
			trap = traps[i][j]
			if trap then
				add_trap(map, i, j, repack(trap))
			end
		end
	end
end

return module
