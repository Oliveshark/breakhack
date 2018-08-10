local room_builder = require "maproombuilder"
local module = {}
local random = math.random

local textures = {
	"Items/Chest0.png",
	"Items/Chest1.png"
}

local chests = {}
for i=0,1 do
	table.insert(chests, {
		textures[1],
		textures[2],
		0,
		i * 16
	});
	table.insert(chests, {
		textures[1],
		textures[2],
		16,
		i * 16
	});
end

local function repack(data)
	return {
		texturePath1	= data[1],
		texturePath2	= data[2],
		clipX			= data[3],
		clipY			= data[4]
	}
end

function module.add_chests_to_room(room)
	if room.type == "coridoor" then
		return
	end

	local count = random(3) - 1
	local i = 0
	while i < count do
		local rx = random(13) + 1
		local ry = random(9) + 1
		if room_builder.is_tile_avilable(room, rx, ry) then
			room.chests[rx][ry] = chests[random(#chests)]
			i = i + 1
		end
	end
end

function module.load_chests(map, chests)
	for i=0,15 do
		for j=0,11 do
			chest = chests[i][j]
			if chest then
				add_chest(map, i, j, CURRENT_LEVEL, repack(chest))
			end
		end
	end
end

return module
