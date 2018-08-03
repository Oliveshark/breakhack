local module = {}
local random = math.random

local texturePaths = {
	aquatic0 = "Characters/Aquatic0.png",
	aquatic1 = "Characters/Aquatic1.png",
	avian0 = "Characters/Avian0.png",
	avian1 = "Characters/Avian1.png",
	cat0 = "Characters/Cat0.png",
	cat1 = "Characters/Cat1.png",
	demon0 = "Characters/Demon0.png",
	demon1 = "Characters/Demon1.png",
	dog0 = "Characters/Dog0.png",
	dog1 = "Characters/Dog1.png",
	elemental0 = "Characters/Elemental0.png",
	elemental1 = "Characters/Elemental1.png",
	humanoid0 = "Characters/Humanoid0.png",
	humanoid1 = "Characters/Humanoid1.png",
	misc0 = "Characters/Misc0.png",
	misc1 = "Characters/Misc1.png",
	pest0 = "Characters/Pest0.png",
	pest1 = "Characters/Pest1.png",
	pant0 = "Characters/Plant0.png",
	plant1 = "Characters/Plant1.png",
	player0 = "Characters/Player0.png",
	player1 = "Characters/Player1.png",
	quadroped0 = "Characters/Quadraped0.png",
	quadroped1 = "Characters/Quadraped1.png",
	reptile0 = "Characters/Reptile0.png",
	reptile1 = "Characters/Reptile1.png",
	rodent0 = "Characters/Rodent0.png",
	rodent1 = "Characters/Rodent1.png",
	slime0 = "Characters/Slime0.png",
	slime1 = "Characters/Slime1.png",
	undead0 = "Characters/Undead0.png",
	undead1 = "Characters/Undead1.png",
}


local behaviour = {
	pacifist = 0,
	normal = 1,
	hostile = 2,
	guerilla = 3,
	coward = 4,
	sentinel = 5
}

local stats = {
	default = {
		hp = 12,
		dmg = 2,
		atk = 0,
		def = 0,
		speed = 1
	},
	pest = {
		hp = 6,
		dmg = 1,
		atk = 0,
		def = 0,
		speed = 1
	},
	misc = {
		hp = 18,
		dmg = 2,
		atk = 0,
		def = 0,
		speed = 1
	},
	dog = {
		hp = 12,
		dmg = 2,
		atk = 0,
		def = 0,
		speed = 1
	},
	undead = {
		hp = 24,
		dmg = 1,
		atk = 0,
		def = 0,
		speed = 1
	},
	platino = {
		hp = 60,
		dmg = 60,
		atk = 60,
		def = 60,
		speed = 3
	}
}

local function concat(table1, table2)
	for i=1,#table2 do
		table1[#table1+1] = table2[i]
	end
	return table1
end

local pests = {
	{ stats.pest,  96, 0, "A Slim Worm", behaviour.pacifist },
	{ stats.pest, 112, 0, "A Fat Worm", behaviour.pacifist },

	{ stats.pest, 16, 16, "A Fly", behaviour.pacifist },
	{ stats.pest, 48, 16, "A Moth", behaviour.pacifist },
	{ stats.pest, 80, 16, "A Gnat", behaviour.pacifist },

	{ stats.pest,  0, 32, "A Small Spider", behaviour.pacifist },
	{ stats.pest, 16, 32, "A Medium Spider", behaviour.pacifist },
	{ stats.pest, 32, 32, "A Large Spider", behaviour.pacifist },
	{ stats.pest, 48, 32, "A Small Scorpion", behaviour.pacifist },
	{ stats.pest, 64, 32, "A Medium Scorpion", behaviour.pacifist },
	{ stats.pest, 80, 32, "A Large Scorpion", behaviour.pacifist },

	{ stats.pest,  0, 48, "A Slug", behaviour.pacifist },
	{ stats.pest, 16, 48, "A Large Slug", behaviour.pacifist },
	{ stats.pest, 48, 48, "A Red Slug", behaviour.pacifist },
	{ stats.pest, 64, 48, "A Large Red Slug", behaviour.pacifist },

	{ stats.pest,  0, 64, "A Giant Brown Ant", behaviour.pacifist },
	{ stats.pest, 16, 64, "A Giant Black Ant", behaviour.pacifist },
}
for i=1,#pests do
	pests[i] = concat({ texturePaths.pest0, texturePaths.pest1 }, pests[i])
end

local misc = {
	{ stats.misc,	0,	0, "A Giant Black Rat", behaviour.sentinel },
	{ stats.misc,	16,	0, "A Giant White Rat", behaviour.sentinel },
	{ stats.misc,	64,	0, "An Owl Bear", behaviour.sentinel },
}
for i=1,#misc do
	misc[i] = concat({ texturePaths.misc0, texturePaths.misc1 }, misc[i])
end

local undead = {
	-- UNDEAD
	{ stats.undead,   0, 32, "A Skeleton", behaviour.normal };
	{ stats.undead,  48, 32, "A Black Skeleton", behaviour.normal };
	{ stats.undead,  64, 32, "A Zombie", behaviour.normal };
	{ stats.undead,  80, 32, "A Zombie", behaviour.normal };
}
for i=1,#undead do
	undead[i] = concat({ texturePaths.undead0, texturePaths.undead1 }, undead[i])
end

local dogs = {
	{ stats.dog,  0, 16, "A Rabid Dog", behaviour.normal };
	{ stats.dog,  16, 16, "An Angry Rabid Dog", behaviour.hostile };
}
for i=1,#dogs do
	dogs[i] = concat({ texturePaths.dog0, texturePaths.dog1 }, dogs[i])
end

local reptile = {
	{ stats.default,  0, 64, "A Small Brown Snake", behaviour.coward };
	{ stats.default,  16, 64, "A Medium Brown Snake", behaviour.guerilla };
	{ stats.default,  32, 64, "A Large Brown Snake", behaviour.hostile };
	{ stats.default,  48, 64, "A Small Black Snake", behaviour.coward };
	{ stats.default,  64, 64, "A Medium Black Snake", behaviour.guerilla };
	{ stats.default,  80, 64, "A Large Black Snake", behaviour.hostile };
}
for i=1,#reptile do
	reptile[i] = concat({ texturePaths.reptile0, texturePaths.reptile1 }, reptile[i])
end

local demon = {
	{ stats.default,   0,  0, "A Demon", behaviour.hostile };
	{ stats.default,  16,  0, "A Demon", behaviour.hostile };
	{ stats.default,  32,  0, "A Demon", behaviour.hostile };
	{ stats.default,  48,  0, "A Demon", behaviour.hostile };
	{ stats.default,  64,  0, "A Demon", behaviour.hostile };
	{ stats.default,  80,  0, "A Demon", behaviour.hostile };
	{ stats.default,  96,  0, "A Demon", behaviour.hostile };
	{ stats.default, 112,  0, "A Demon", behaviour.hostile };
}
for i=1,#demon do
	demon[i] = concat({ texturePaths.demon0, texturePaths.demon1 }, demon[i])
end

local platino = {
	{
		texturePaths.reptile0,
		texturePaths.reptile1,
		stats.platino,
		48,
		12*16,
		"Platino",
		behaviour.sentinel
	}
}

local function repack(data)
	return {
		texturePath1 	= data[1],
		texturePath2 	= data[2],
		stats			= data[3],
		clipX			= data[4],
		clipY			= data[5],
		label			= data[6] or "",
		behaviour		= data[7] or behaviour.normal,
	}
end

-- Begin script
local enemies = {}
if(CURRENT_LEVEL > 0 and CURRENT_LEVEL < 10) then
	if (CURRENT_LEVEL == 1) then
		enemies = concat(enemies, pests)
		enemies = concat(enemies, misc)
		enemies = concat(enemies, dogs)
	elseif (CURRENT_LEVEL > 5) then
		enemies = {}
		enemies = concat(enemies, demon)
		enemies = concat(enemies, undead)
		enemies = concat(enemies, reptile)
		enemies = concat(enemies, misc)
	elseif (CURRENT_LEVEL > 3) then
		enemies = {}
		enemies = concat(enemies, undead)
		enemies = concat(enemies, reptile)
		enemies = concat(enemies, misc)
		enemies = concat(enemies, dogs)
	elseif (CURRENT_LEVEL > 2) then
		enemies = {}
		enemies = concat(enemies, undead)
		enemies = concat(enemies, reptile)
		enemies = concat(enemies, misc)
		enemies = concat(enemies, dogs)
	elseif (CURRENT_LEVEL > 1) then
		enemies = {}
		enemies = concat(enemies, undead)
		enemies = concat(enemies, reptile)
		enemies = concat(enemies, misc)
		enemies = concat(enemies, dogs)
	end
end

if random(100) == 1 then
	enemies = concat(enemies, platino);
end

function module.add_monsters_to_room(room, roomx, roomy)
	local count = random(3)
	if (CURRENT_LEVEL > 3) then
		count = random(4)
	end
	local i = 0
	while i < count do
		local rx = random(13) + 1
		local ry = random(9) + 1
		if not room.decor[rx][ry]
			and not room.monsters[rx][ry]
			and (room.tiles[rx][ry]
				and not room.tiles[rx][ry][5]
				and not room.tiles[rx][ry][8])
			then

			local x = (roomx * 512) + rx * 32
			local y = (roomy * 384) + ry * 32
			room.monsters[rx][ry] = {
				x,
				y,
				enemies[random(#enemies)]
			}
			i = i + 1
		end
	end
end

function module.load_monsters(map, monsters)
	for i=0,15 do
		for j=0,11 do
			monster = monsters[i][j]
			if monster then
				add_monster(map, monster[1], monster[2], repack(monster[3]))
			end
		end
	end
end

return module
