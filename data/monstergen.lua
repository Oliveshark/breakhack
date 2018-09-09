local room_builder = require "maproombuilder"
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
	sentinel = 5,
	fire_demon = 6,
	sorcerer = 7,
	assassin = 8
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
	orc = {
		hp = 20,
		dmg = 2,
		atk = 0,
		def = 0,
		speed = 1
	},
	boss = {
		hp = 60,
		dmg = 3,
		atk = 1,
		def = 0,
		speed = 1
	},
	platino = {
		hp = 90,
		dmg = 1,
		atk = 0,
		def = 0,
		speed = 1
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

local avian = {
	{ stats.default, 0, 11*16, "A Small Brown Bat", behaviour.pacifist },
	{ stats.default, 16, 11*16, "A Big Brown Bat", behaviour.normal },
	{ stats.default, 32, 11*16, "A Vampire Bat", behaviour.guerilla },
	{ stats.default, 48, 11*16, "A Rabid Bat", behaviour.hostile },
}
for i=1,#avian do
	avian[i] = concat({ texturePaths.avian0, texturePaths.avian1 }, avian[i])
end

local misc = {
	{ stats.misc,	0,	0, "A Giant Black Rat", behaviour.sentinel },
	{ stats.misc,	16,	0, "A Giant White Rat", behaviour.sentinel },
	{ stats.misc,	64,	0, "An Owl Bear", behaviour.sentinel },
}
for i=1,#misc do
	misc[i] = concat({ texturePaths.misc0, texturePaths.misc1 }, misc[i])
end

local reanimated = {
	{ stats.undead,   0, 32, "A Skeleton", behaviour.normal },
	{ stats.undead,  48, 32, "A Black Skeleton", behaviour.normal },
	{ stats.undead,  64, 32, "A Zombie", behaviour.normal },
	{ stats.undead,  80, 32, "A Zombie", behaviour.normal }
}
for i=1,#reanimated do
	reanimated[i] = concat({ texturePaths.undead0, texturePaths.undead1 }, reanimated[i])
end

local undead = {
	{ stats.undead,  5*16, 16, "A Mummy", behaviour.normal },
	{ stats.undead,  6*16, 16, "A Two Headed Mummy", behaviour.sentinel },
	{ stats.undead,  0*16, 32, "A Skeleton", behaviour.normal },
	{ stats.misc,  1*16, 32, "A Burning Skeleton", behaviour.fire_demon },
	{ stats.misc,  2*16, 32, "An Eldritch Skeleton", behaviour.sorcerer },
	{ stats.misc,  3*16, 32, "A Black Skeleton", behaviour.guerilla },
	{ stats.misc,  4*16, 32, "A Zombie", behaviour.coward },
	{ stats.misc,  5*16, 32, "A Pale Zombie", behaviour.coward },
	{ stats.misc,  7*16, 32, "A Scorched Zombie", behaviour.fire_demon },
	{ stats.undead,  0*16, 4*16, "A Whight", behaviour.coward },
	{ stats.undead,  1*16, 4*16, "A Ghast", behaviour.sentinel },
	{ stats.misc,  1*16, 4*16, "A Ghost", behaviour.guerilla },
	{ stats.misc,  0*16, 5*16, "A Spectre", behaviour.sentinel },
	{ stats.undead,  1*16, 5*16, "An Eldritch Spectre", behaviour.sorcerer },
	{ stats.undead,  2*16, 5*16, "A Scorched Spectre", behaviour.fire_demon },
}
for i=1,#undead do
	undead[i] = concat({ texturePaths.undead0, texturePaths.undead1 }, undead[i])
end

local dogs = {
	{ stats.dog,  0, 16, "A Rabid Dog", behaviour.normal },
	{ stats.dog,  16, 16, "An Angry Rabid Dog", behaviour.hostile }
}
for i=1,#dogs do
	dogs[i] = concat({ texturePaths.dog0, texturePaths.dog1 }, dogs[i])
end

local reptile = {
	{ stats.default,  0, 64, "A Small Brown Snake", behaviour.coward },
	{ stats.default,  16, 64, "A Medium Brown Snake", behaviour.guerilla },
	{ stats.default,  32, 64, "A Large Brown Snake", behaviour.hostile },
	{ stats.default,  48, 64, "A Small Black Snake", behaviour.coward },
	{ stats.default,  64, 64, "A Medium Black Snake", behaviour.guerilla },
	{ stats.default,  80, 64, "A Large Black Snake", behaviour.hostile },
}
for i=1,#reptile do
	reptile[i] = concat({ texturePaths.reptile0, texturePaths.reptile1 }, reptile[i])
end

local demon = {
	{ stats.default,   0,  0, "A Demon", behaviour.hostile },
	{ stats.default,  16,  0, "A Demon", behaviour.hostile },
	{ stats.default,  32,  0, "A Demon", behaviour.hostile },
	{ stats.default,  48,  0, "A Demon", behaviour.hostile },
	{ stats.default,  64,  0, "A Demon", behaviour.hostile },
	{ stats.default,  80,  0, "A Demon", behaviour.hostile },
	{ stats.default,  96,  0, "A Demon", behaviour.hostile },
	{ stats.default, 112,  0, "A Demon", behaviour.hostile },
}
for i=1,#demon do
	demon[i] = concat({ texturePaths.demon0, texturePaths.demon1 }, demon[i])
end

local orcs = {
	{ stats.orc,   0,  4*16, "An Orc Guard", behaviour.normal },
	{ stats.orc,   16,  4*16, "An Orc Seargeant", behaviour.coward },
	{ stats.orc,   32,  4*16, "An Orc Militia", behaviour.hostile },
	{ stats.orc,   48,  4*16, "An Orc Sentry", behaviour.sentinel },
	{ stats.orc,   64,  4*16, "An Orc Brute", behaviour.guerilla },
	{ stats.orc,   80,  4*16, "An Orc Captain", behaviour.hostile },
	{ stats.orc,   96,  4*16, "An Orc Pyro", behaviour.fire_demon },
}
for i=1,#orcs do
	orcs[i] = concat({ texturePaths.humanoid0, texturePaths.humanoid1 }, orcs[i])
end

local assassins = {
	{ stats.misc,  1*16, 6*16, "A Reaper", behaviour.assassin },
	{ stats.misc,  0*16, 7*16, "An Assassin", behaviour.assassin },
	{ stats.misc,  1*16, 7*16, "A Royal Assassin", behaviour.assassin },
}
for i=1,#assassins do
	assassins[i] = concat({ texturePaths.undead0, texturePaths.undead1 }, assassins[i])
end

local bosses = {
	{ stats.boss, 16,  5*16, "The Hell Hound", behaviour.fire_demon, true },
	{ stats.boss, 16,  23*16, "The Cleric", behaviour.sorcerer, true },
	{ stats.boss, 16,  8*16, "The Shadow", behaviour.assassin, true },
}
bosses[1] = concat({ texturePaths.dog0, texturePaths.dog1 }, bosses[1])
bosses[2] = concat({ texturePaths.humanoid0, texturePaths.humanoid1 }, bosses[2])
bosses[3] = concat({ texturePaths.undead0, texturePaths.undead1 }, bosses[3])

local eastereggs = {
	{ stats.misc,  6*16, 1*16, "Linus, the Developer", behaviour.passive },
	{ stats.misc,  2*16, 3*16, "Lialynn, the Ranger", behaviour.passive },
	{ stats.misc,  4*16, 3*16, "Miniel, the Paladin", behaviour.passive },
	{ stats.misc,  2*16,12*16, "Feng, the Fighter", behaviour.passive },
	{ stats.misc,  0*16, 7*16, "Adnis, the Ranger", behaviour.passive },
	{ stats.misc,  7*16, 8*16, "Ti, the Mage", behaviour.passive },
}
for i=1,#eastereggs do
	eastereggs[i] = concat({ texturePaths.player0, texturePaths.player1 }, eastereggs[i])
end

-- Add Platino
table.insert(eastereggs, {
		texturePaths.reptile0,
		texturePaths.reptile1,
		stats.platino,
		48,
		12*16,
		"Platino",
		behaviour.sentinel,
		true
	})

local function repack(data)
	return {
		texturePath1 	= data[1],
		texturePath2 	= data[2],
		stats			= data[3],
		clipX			= data[4],
		clipY			= data[5],
		label			= data[6] or "",
		behaviour		= data[7] or behaviour.normal,
		boss			= data[8] or false,
	}
end

-- Begin script
local enemies = {}
if(CURRENT_LEVEL > 0) then
	if (CURRENT_LEVEL == 1) then
		enemies = concat(enemies, pests)
		enemies = concat(enemies, avian)
		enemies = concat(enemies, misc)
		enemies = concat(enemies, dogs)
	elseif (CURRENT_LEVEL > 15) then
		enemies = {}
		enemies = concat(enemies, undead)
		enemies = concat(enemies, orcs)
		enemies = concat(enemies, assassins)
	elseif (CURRENT_LEVEL > 10) then
		enemies = {}
		enemies = concat(enemies, undead)
		enemies = concat(enemies, avian)
	elseif (CURRENT_LEVEL > 5) then
		enemies = {}
		enemies = concat(enemies, orcs)
		enemies = concat(enemies, avian)
	elseif (CURRENT_LEVEL > 1) then
		enemies = {}
		enemies = concat(enemies, reanimated)
		enemies = concat(enemies, reptile)
		enemies = concat(enemies, avian)
		enemies = concat(enemies, misc)
		enemies = concat(enemies, dogs)
	end
end

local addSpecialInLevel = random(100) == 1

local function add_monster_to_tile(room, roomx, roomy, rx, ry, monster)
	local x = (roomx * 512) + rx * 32
	local y = (roomy * 384) + ry * 32
	room.monsters[rx][ry] = {
		x,
		y,
		monster
	}
end

function module.add_monsters_to_room(room, roomx, roomy)
	local addSpecial = addSpecialInLevel and random(5) == 1
	local count = random(3)
	if (CURRENT_LEVEL > 3) then
		count = random(4)
	end
	count = count + 1
	local i = 0
	while i < count do
		local rx = random(13) + 1
		local ry = random(9) + 1
		if room_builder.is_tile_avilable(room, rx, ry) then
			if addSpecial then
				addSpecialInLevel = false
				addSpecial = false
				add_monster_to_tile(room, roomx, roomy, rx, ry, eastereggs[random(#eastereggs)])
			else
				add_monster_to_tile(room, roomx, roomy, rx, ry, enemies[random(#enemies)])
			end
			i = i + 1
		end
	end
end

function module.add_boss_to_room(room, roomx, roomy)
	local boss = bosses[CURRENT_LEVEL / 5]
	local success = false
	while not success do
		local rx = random(13) + 1
		local ry = random(9) + 1
		if room_builder.is_tile_avilable(room, rx, ry) then
			local x = (roomx * 512) + rx * 32
			local y = (roomy * 384) + ry * 32
			room.monsters[rx][ry] = {
				x,
				y,
				boss
			}
			success = true
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

