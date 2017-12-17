#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

#include "stats.h"
#include "random.h"

unsigned int
stats_fight(Stats *attacker, Stats *defender)
{
	unsigned int atkRoll, defRoll, dmgRoll;
	bool critical = false;

	atkRoll = get_random(19) + 1;
	if (atkRoll == 20)
		critical = true;
	atkRoll += attacker->atk;
	defRoll = (get_random(19) + 1) + defender->def;
	dmgRoll = 0;

	//printf("Attacking: %u, Defending: %u\n", atkRoll, defRoll);
	if (atkRoll > defRoll) {
		dmgRoll = (rand() % 8) + attacker->dmg;
		defender->hp -= dmgRoll;
		if (critical)
			defender->hp -= dmgRoll;
	}
	//printf("Attacker hp: %u, Defender hp: %u\n", attacker->hp, defender->hp);

	return dmgRoll;
}
