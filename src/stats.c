#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

#include "stats.h"
#include "random.h"
#include "util.h"

unsigned int
stats_fight(Stats *attacker, Stats *defender)
{
	int atkRoll, defRoll, dmgRoll;
	bool critical = false;

	atkRoll = get_random(19) + 1;
	if (atkRoll == 20)
		critical = true;
	atkRoll += attacker->atk;
	defRoll = (get_random(19) + 1) + defender->def;
	dmgRoll = 0;

	debug("");
	debug("-----------[ FIGHT ]---------");
	debug("Attacking: %d   Defending: %d", atkRoll, defRoll);

	if (atkRoll > defRoll) {
		dmgRoll = get_random(attacker->dmg - 1) + 1;
		if (critical)
			dmgRoll = dmgRoll * 2;
		defender->hp -= dmgRoll;
	}

	debug("Attacker hp: %d  Defender hp: %d", attacker->hp, defender->hp);
	debug("-----------------------------");

	return dmgRoll;
}
