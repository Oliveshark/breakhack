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

local pests = {
	-- PESTS
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest,   0, 0, "A Beetle" },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest,  16, 0, "A Beetle" },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest,  32, 0, "A Beetle" },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest,  48, 0, "A Beetle" },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest,  64, 0, "A Large Grub" },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest,  80, 0, "A Small Grub" },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest,  96, 0, "A Slim Worm", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 112, 0, "A Fat Worm", behaviour.pacifist },

	--{ texturePaths.pest0, texturePaths.pest1,  0, 16, "Female Dragonfly" },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 16, 16, "A Fly", behaviour.pacifist },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest, 32, 16, "A Larva" },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 48, 16, "A Moth", behaviour.pacifist },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest, 64, 16 },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 80, 16, "A Gnat", behaviour.pacifist },

	{ texturePaths.pest0, texturePaths.pest1, stats.pest,  0, 32, "A Small Spider", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 16, 32, "A Medium Spider", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 32, 32, "A Large Spider", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 48, 32, "A Small Scorpion", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 64, 32, "A Medium Scorpion", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 80, 32, "A Large Scorpion", behaviour.pacifist },

	{ texturePaths.pest0, texturePaths.pest1, stats.pest,  0, 48, "A Slug", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 16, 48, "A Large Slug", behaviour.pacifist },
	--{ texturePaths.pest0, texturePaths.pest1, 32, 48 },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 48, 48, "A Red Slug", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 64, 48, "A Large Red Slug", behaviour.pacifist },

	{ texturePaths.pest0, texturePaths.pest1, stats.pest,  0, 64, "A Giant Brown Ant", behaviour.pacifist },
	{ texturePaths.pest0, texturePaths.pest1, stats.pest, 16, 64, "A Giant Black Ant", behaviour.pacifist },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest, 32, 64, "A Giant Gold Ant" },
	--{ texturePaths.pest0, texturePaths.pest1, stats.pest, 48, 64, "A Giant Silver Ant" },

	--{ texturePaths.pest0, texturePaths.pest1,  0, 80 },
	--{ texturePaths.pest0, texturePaths.pest1, 16, 80 },
	--{ texturePaths.pest0, texturePaths.pest1, 32, 80 },
	--{ texturePaths.pest0, texturePaths.pest1, 48, 80 },

	--{ texturePaths.pest0, texturePaths.pest1,  0, 96 },
	--{ texturePaths.pest0, texturePaths.pest1, 16, 96 },
	--{ texturePaths.pest0, texturePaths.pest1, 32, 96 },

	--{ texturePaths.pest0, texturePaths.pest1,  0, 112 },
	--{ texturePaths.pest0, texturePaths.pest1, 16, 112 },
	--{ texturePaths.pest0, texturePaths.pest1, 32, 112 },
	--{ texturePaths.pest0, texturePaths.pest1, 48, 112 },
}

local misc = {
	{ texturePaths.misc0, texturePaths.misc1, stats.misc,	0,	0, "A Giant Black Rat", behaviour.sentinel },
	{ texturePaths.misc0, texturePaths.misc1, stats.misc,	16,	0, "A Giant White Rat", behaviour.sentinel },
	{ texturePaths.misc0, texturePaths.misc1, stats.misc,	64,	0, "An Owl Bear", behaviour.sentinel },
}

local undead = {
	-- UNDEAD
	--{ texturePaths.undead0, texturePaths.undead1,   0,  0, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  16,  0, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  32,  0, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  48,  0, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  64,  0, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  80,  0, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  96,  0, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1, 112,  0, "", behaviour.normal };

	--{ texturePaths.undead0, texturePaths.undead1,   0, 16, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  16, 16, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  32, 16, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  48, 16, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  64, 16, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  80, 16, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  96, 16, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1, 112, 16, "", behaviour.normal };

	{ texturePaths.undead0, texturePaths.undead1, stats.undead,   0, 32, "A Skeleton", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  16, 32, "An Umber Skeleton", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  32, 32, "A Caustic Skeleton", behaviour.normal };
	{ texturePaths.undead0, texturePaths.undead1, stats.undead,  48, 32, "A Black Skeleton", behaviour.normal };
	{ texturePaths.undead0, texturePaths.undead1, stats.undead,  64, 32, "A Zombie", behaviour.normal };
	{ texturePaths.undead0, texturePaths.undead1, stats.undead,  80, 32, "A Zombie", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1,  96, 32, "", behaviour.normal };
	--{ texturePaths.undead0, texturePaths.undead1, 112, 32, "", behaviour.normal };
}

local dogs = {
	{ texturePaths.dog0, texturePaths.dog1, stats.dog,  0, 16, "A Rabid Dog", behaviour.normal };
	{ texturePaths.dog0, texturePaths.dog1, stats.dog,  16, 16, "An Angry Rabid Dog", behaviour.hostile };
}

local reptile = {
	{ texturePaths.reptile0, texturePaths.reptile1, stats.default,  0, 64, "A Small Brown Snake", behaviour.coward };
	{ texturePaths.reptile0, texturePaths.reptile1, stats.default,  16, 64, "A Medium Brown Snake", behaviour.guerilla };
	{ texturePaths.reptile0, texturePaths.reptile1, stats.default,  32, 64, "A Large Brown Snake", behaviour.hostile };
	{ texturePaths.reptile0, texturePaths.reptile1, stats.default,  48, 64, "A Small Black Snake", behaviour.coward };
	{ texturePaths.reptile0, texturePaths.reptile1, stats.default,  64, 64, "A Medium Black Snake", behaviour.guerilla };
	{ texturePaths.reptile0, texturePaths.reptile1, stats.default,  80, 64, "A Large Black Snake", behaviour.hostile };
}

local platino = {
	{ texturePaths.reptile0, texturePaths.reptile1, stats.platino,  48, 12*16, "Platino", behaviour.sentinel };
}

local demon = {
	{ texturePaths.demon0, texturePaths.demon1, stats.default,   0,  0, "A Demon", behaviour.hostile };
	{ texturePaths.demon0, texturePaths.demon1, stats.default,  16,  0, "A Demon", behaviour.hostile };
	{ texturePaths.demon0, texturePaths.demon1, stats.default,  32,  0, "A Demon", behaviour.hostile };
	{ texturePaths.demon0, texturePaths.demon1, stats.default,  48,  0, "A Demon", behaviour.hostile };
	{ texturePaths.demon0, texturePaths.demon1, stats.default,  64,  0, "A Demon", behaviour.hostile };
	{ texturePaths.demon0, texturePaths.demon1, stats.default,  80,  0, "A Demon", behaviour.hostile };
	{ texturePaths.demon0, texturePaths.demon1, stats.default,  96,  0, "A Demon", behaviour.hostile };
	{ texturePaths.demon0, texturePaths.demon1, stats.default, 112,  0, "A Demon", behaviour.hostile };
}

local function concat(table1, table2)
	for i=1,#table2 do
		table1[#table1+1] = table2[i]
	end
	return table1
end

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
