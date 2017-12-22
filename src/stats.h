#ifndef STATS_H_
#define	STATS_H_

typedef struct Stats_t {
	int maxhp;			/* Max hitpoints */
	int hp;				/* Current hit points */
	int dmg;			/* Damage modifier */
	int atk;			/* Attack rating */
	int def;			/* Defence rating */
	unsigned int speed;	/* Speed */
	unsigned int lvl;	/* Level */
} Stats;

unsigned int
stats_fight(Stats *attacker, Stats *defender);

#endif // STATS_H_
