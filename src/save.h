#ifndef SAVE_H_
#define	SAVE_H_

#include "player.h"

typedef struct Save {
	int seed;
	unsigned int map_level;
	unsigned int player_level;
	unsigned int player_daggers;
	unsigned int player_xp;
	double player_gold;
	class_t class;
} Save;

#endif // SAVE_H_
