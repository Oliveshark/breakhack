#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>

#include "stats.h"
#include "random.h"

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

	printf("\n");
	printf("-----------[ FIGHT ]---------\n");
	printf("Attacking: %d   Defending: %d\n", atkRoll, defRoll);

	if (atkRoll > defRoll) {
		dmgRoll = (rand() % attacker->dmg) + 1;
		defender->hp -= dmgRoll;
		if (critical)
			defender->hp -= dmgRoll;
	}

	printf("Attacker hp: %d  Defender hp: %d\n", attacker->hp, defender->hp);
	printf("-----------------------------\n");

	return dmgRoll;
}
