/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include "texturecache.h"
#include "skill.h"
#include "util.h"
#include "player.h"
#include "roommatrix.h"
#include "stats.h"
#include "monster.h"
#include "mixer.h"
#include "gui.h"
#include "random.h"

static Skill *
create_default(const char *s_label, Sprite *s)
{
	Skill *skill = ec_malloc(sizeof(Skill));
	m_strcpy(skill->label, 20, s_label);
	skill->resetTime = 5;
	skill->resetCountdown = 0;
	skill->icon = s;
	skill->actionRequired = true;
	skill->instantUse = false;
	skill->active = false;
	skill->use = NULL;
	return skill;
}

static bool
skill_use_flurry(Skill *skill, SkillData *data)
{
	UNUSED (skill);

	Position playerPos = position_to_matrix_coords(&data->player->sprite->pos);
	Position targetPos = playerPos;
	targetPos.x += (int) data->direction.x;
	targetPos.y += (int) data->direction.y;

	Monster *monster = data->matrix->spaces[targetPos.x][targetPos.y].monster;
	mixer_play_effect(TRIPPLE_SWING);
	if (monster) {
		gui_log("You attack %s with a flurry of strikes", monster->lclabel);
		unsigned int hitCount = 0;
		for (size_t i = 0; i < 3; ++i) {
			unsigned int originalHp = monster->stats.hp;
			unsigned int dmg = stats_fight(&data->player->stats, &monster->stats);
			if (dmg > 0 && originalHp > 0) {
				gui_log("You hit for %u damage", dmg);
				monster_hit(monster, dmg);
				hitCount++;
			}
		}
		if (hitCount == 1) {
			mixer_play_effect(SWORD_HIT);
		} else if (hitCount == 2) {
			mixer_play_effect(DOUBLE_SWORD_HIT);
		} else if (hitCount == 3) {
			mixer_play_effect(TRIPPLE_SWORD_HIT);
		}

	} else {
		gui_log("You swing at thin air with a flurry of strikes");
	}
	player_monster_kill_check(data->player, monster);

	return true;
}

static Skill *
create_flurry(void)
{
	Texture *t = texturecache_add("Items/MedWep.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = DEFAULT_DIMENSION;
	s->clip = CLIP16(0, 0);
	s->fixed = true;
	Skill *skill = create_default("Flurry", s);
	skill->use = skill_use_flurry;
	return skill;
}

static bool
skill_sip_health(Skill *skill, SkillData *data)
{
	UNUSED (skill);
	player_sip_health(data->player);
	return true;
}

static Skill *
create_sip_health(void)
{
	Texture *t = texturecache_add("Items/Potion.png");
	Sprite *s = sprite_create();
	sprite_set_texture(s, t, 0);
	s->dim = DEFAULT_DIMENSION;
	s->clip = CLIP16(0, 0);
	s->fixed = true;
	Skill *skill = create_default("Sip health", s);
	skill->instantUse = true;
	skill->use = skill_sip_health;
	skill->resetTime = 0;
	return skill;
}

Skill*
skill_create(enum SkillType t)
{
	switch (t) {
		case FLURRY:
			return create_flurry();
		case SIP_HEALTH:
			return create_sip_health();
		default:
			fatal("Unknown SkillType %u", (unsigned int) t);
			return NULL;
	}
}

void
skill_destroy(Skill *skill)
{
	sprite_destroy(skill->icon);
	free(skill);
}
