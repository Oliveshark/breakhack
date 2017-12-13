local module = {}

local black_ant = {
	texturePath1 = "assets/Characters/Pest0.png",
	texturePath2 = "assets/Characters/Pest1.png",
	clipX			= 16,
	clipY			= 64,
}


function module.add_monster_to_room(map)
	add_monster(map, 128, 128, black_ant);
end

return module
