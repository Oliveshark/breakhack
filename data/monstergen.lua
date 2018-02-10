local module = {}
local random = math.random

local texturePaths = {
	aquatic0 = "assets/Characters/Aquatic0.png",
	aquatic1 = "assets/Characters/Aquatic1.png",
	avian0 = "assets/Characters/Avian0.png",
	avian1 = "assets/Characters/Avian1.png",
	cat0 = "assets/Characters/Cat0.png",
	cat1 = "assets/Characters/Cat1.png",
	demon0 = "assets/Characters/Demon0.png",
	demon1 = "assets/Characters/Demon1.png",
	dog0 = "assets/Characters/Dog0.png",
	dog1 = "assets/Characters/Dog1.png",
	elemental0 = "assets/Characters/Elemental0.png",
	elemental1 = "assets/Characters/Elemental1.png",
	humanoid0 = "assets/Characters/Humanoid0.png",
	humanoid1 = "assets/Characters/Humanoid1.png",
	misc0 = "assets/Characters/Misc0.png",
	misc1 = "assets/Characters/Misc1.png",
	pest0 = "assets/Characters/Pest0.png",
	pest1 = "assets/Characters/Pest1.png",
	pant0 = "assets/Characters/Plant0.png",
	plant1 = "assets/Characters/Plant1.png",
	player0 = "assets/Characters/Player0.png",
	player1 = "assets/Characters/Player1.png",
	quadroped0 = "assets/Characters/Quadraped0.png",
	quadroped1 = "assets/Characters/Quadraped1.png",
	reptile0 = "assets/Characters/Reptile0.png",
	reptile1 = "assets/Characters/Reptile1.png",
	rodent0 = "assets/Characters/Rodent0.png",
	rodent1 = "assets/Characters/Rodent1.png",
	slime0 = "assets/Characters/Slime0.png",
	slime1 = "assets/Characters/Slime1.png",
	undead0 = "assets/Characters/Undead0.png",
	undead1 = "assets/Characters/Undead1.png",
}


local state = {
	passive = 0,
	agressive = 1,
	scared = 2,
}

local pests = {
	-- PESTS
	{ texturePaths.pest0, texturePaths.pest1,   0, 0, "A Beetle" },
	{ texturePaths.pest0, texturePaths.pest1,  16, 0, "A Beetle" },
	{ texturePaths.pest0, texturePaths.pest1,  32, 0, "A Beetle" },
	{ texturePaths.pest0, texturePaths.pest1,  48, 0, "A Beetle" },
	{ texturePaths.pest0, texturePaths.pest1,  64, 0, "A Large Grub" },
	{ texturePaths.pest0, texturePaths.pest1,  80, 0, "A Small Grub" },
	{ texturePaths.pest0, texturePaths.pest1,  96, 0, "A Slim Worm" },
	{ texturePaths.pest0, texturePaths.pest1, 112, 0, "A Fat Worm" },

	--{ texturePaths.pest0, texturePaths.pest1,  0, 16, "Female Dragonfly" },
	{ texturePaths.pest0, texturePaths.pest1, 16, 16, "A Fly" },
	{ texturePaths.pest0, texturePaths.pest1, 32, 16, "A Larva" },
	{ texturePaths.pest0, texturePaths.pest1, 48, 16, "A Moth" },
	--{ texturePaths.pest0, texturePaths.pest1, 64, 16 },
	{ texturePaths.pest0, texturePaths.pest1, 80, 16, "A Gnat" },

	{ texturePaths.pest0, texturePaths.pest1,  0, 32, "A Small Spider" },
	{ texturePaths.pest0, texturePaths.pest1, 16, 32, "A Medium Spider" },
	{ texturePaths.pest0, texturePaths.pest1, 32, 32, "A Large Spider" },
	{ texturePaths.pest0, texturePaths.pest1, 48, 32, "A Small Scorpion" },
	{ texturePaths.pest0, texturePaths.pest1, 64, 32, "A Medium Scorpion" },
	{ texturePaths.pest0, texturePaths.pest1, 80, 32, "A Large Scorpion" },

	{ texturePaths.pest0, texturePaths.pest1,  0, 48, "A Slug" },
	{ texturePaths.pest0, texturePaths.pest1, 16, 48, "A Large Slug" },
	--{ texturePaths.pest0, texturePaths.pest1, 32, 48 },
	{ texturePaths.pest0, texturePaths.pest1, 48, 48, "A Red Slug" },
	{ texturePaths.pest0, texturePaths.pest1, 64, 48, "A Large Red Slug" },

	{ texturePaths.pest0, texturePaths.pest1,  0, 64, "A Giant Brown Ant" },
	{ texturePaths.pest0, texturePaths.pest1, 16, 64, "A Giant Black Ant" },
	{ texturePaths.pest0, texturePaths.pest1, 32, 64, "A Giant Gold Ant" },
	{ texturePaths.pest0, texturePaths.pest1, 48, 64, "A Giant Silver Ant" },

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

local undead = {
	-- UNDEAD
	--{ texturePaths.undead0, texturePaths.undead1,   0,  0, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  16,  0, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  32,  0, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  48,  0, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  64,  0, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  80,  0, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  96,  0, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1, 112,  0, "", state.passive, state.agressive };

	--{ texturePaths.undead0, texturePaths.undead1,   0, 16, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  16, 16, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  32, 16, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  48, 16, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  64, 16, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  80, 16, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  96, 16, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1, 112, 16, "", state.passive, state.agressive };

	{ texturePaths.undead0, texturePaths.undead1,   0, 32, "A Skeleton", state.passive, state.agressive };
	{ texturePaths.undead0, texturePaths.undead1,  16, 32, "An Umber Skeleton", state.passive, state.agressive };
	{ texturePaths.undead0, texturePaths.undead1,  32, 32, "A Caustic Skeleton", state.passive, state.agressive };
	{ texturePaths.undead0, texturePaths.undead1,  48, 32, "A Black Skeleton", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  64, 32, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  80, 32, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1,  96, 32, "", state.passive, state.agressive };
	--{ texturePaths.undead0, texturePaths.undead1, 112, 32, "", state.passive, state.scared };
}

local demon = {
	{ texturePaths.demon0, texturePaths.demon1,   0,  0, "A Demon", state.agressive, state.agressive };
	{ texturePaths.demon0, texturePaths.demon1,  16,  0, "A Demon", state.agressive, state.agressive };
	{ texturePaths.demon0, texturePaths.demon1,  32,  0, "A Demon", state.agressive, state.agressive };
	{ texturePaths.demon0, texturePaths.demon1,  48,  0, "A Demon", state.agressive, state.agressive };
	{ texturePaths.demon0, texturePaths.demon1,  64,  0, "A Demon", state.agressive, state.agressive };
	{ texturePaths.demon0, texturePaths.demon1,  80,  0, "A Demon", state.agressive, state.agressive };
	{ texturePaths.demon0, texturePaths.demon1,  96,  0, "A Demon", state.agressive, state.agressive };
	{ texturePaths.demon0, texturePaths.demon1, 112,  0, "A Demon", state.agressive, state.agressive };
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
		clipX			= data[3],
		clipY			= data[4],
		label			= data[5] or "",
		nstate			= data[6] or state.passive,
		cstate			= data[7] or state.scared,
	}
end

-- Begin script
local enemies = {}
if(CURRENT_LEVEL > 0 and CURRENT_LEVEL < 10) then
	if (CURRENT_LEVEL == 1) then
		enemies = concat(enemies, pests)
		enemies = concat(enemies, undead)
	elseif (CURRENT_LEVEL > 2) then
		enemies = {}
		enemies = concat(enemies, undead)
		enemies = concat(enemies, demon)
	elseif (CURRENT_LEVEL > 1) then
		enemies = {}
		enemies = concat(enemies, undead)
	end
end

function module.add_monster_to_room(map, roomx, roomy)
	local count = random(3)
	for i=0,count do
		local x = (roomx * 512) + (random(13) + 1) * 32
		local y = (roomy * 384) + (random(9) + 1) * 32
		add_monster(map, x, y, repack(enemies[random(#enemies)]));
	end
end

return module
