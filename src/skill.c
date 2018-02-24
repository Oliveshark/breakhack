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

#include <string.h>
#include "texturecache.h"
#include "skill.h"
#include "util.h"

static Skill *
create_default(const char *s_label, Sprite *s)
{
	Skill *skill = ec_malloc(sizeof(Skill));
	m_strcpy(skill->label, 20, s_label);
	skill->icon = s;
	skill->use = NULL;
	return skill;
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
	return skill;
}

Skill*
skill_create(enum SkillType t)
{
	switch (t) {
		case FLURRY:
			return create_flurry();
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
