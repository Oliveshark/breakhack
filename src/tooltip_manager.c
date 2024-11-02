/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2024  Linus Probert <linus.probert@gmail.com>
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
#include "tooltip_manager.h"
#include "tooltip.h"
#include "sprite.h"

static char *artifacts_tooltip[] = {
	"CONGRATULATIONS!", "",
	"",
	"   You just picked up your first artifact!", "",
	"",
	"   Your current artifacts and corresponding level are", "",
	"   listed next to your skills.", "",
	"",
	"",
	"   Artifacts have mystical effects that improve your offensive", "",
	"   or defensive advantage in the dungeon. However it is sometimes", "",
	"   hard to know what effect an artifact has.", "",
	"",
	"",
	"   Perhaps an experienced dungeoner will know more?", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *skills_tooltip[] = {
	"CONGRATULATIONS!", "",
	"",
	"   You have aquired a new level and a new skill!", "",
	"",
	"   Skills are listed in the bar below the game screen.", "",
	"",
	"",
	"   SKILL INFO:            SHIFT + <N>", "",
	"                          Where <N> is the number corresponding to the skill", "",
	"                          Eg. 1, 2, 3, 4, 5", "",
	"",
	"   DISABLE TOOLTIPS:      CTRL + D", "",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static char *how_to_play_tooltip[] = {
	"HOW TO PLAY", "",
	"",
	"   NAVIGATION:        Use ARROWS or WASD or HJKL to move", "",
	"                      Controller: LEFT STICK or D-PAD", "",
	"",
	"   ATTACK:            Walk into a monster to attack it", "",
	"",
	"   HOLD TURN:         Press ", "SPACE", "",
	"",
	"   THROW DAGGER:      Press ", "4", " then chose a direction (nav keys)", "",
	"",
	"   DRINK HEALTH:      Press ", "5", " (if you need health and have potions)", "",
	"",
	"   TOGGLE MUSIC:      CTRL + M", "",
	"",
	"   TOGGLE SOUND:      CTRL + S", "",
	"",
	"   TOGGLE FULLSCREEN: CTRL + F", "",
	"",
	"   TOGGLE MENU:       ", "ESC", "",
	"",
	"   Your stats and inventory are listed in the right panel", "",
	"",
	"",
	"   GOOD LUCK!", "",
	"   May your death be quick and painless...", "",
	"",
	"",
	"",
	"Press ", "ESC", " to close", "",
	NULL
};

static Tooltip	*new_skill_tooltip	= NULL;
static Tooltip	*howto_tooltip		= NULL;
static Tooltip	*new_artifact_tooltip	= NULL;

static GamepadType	controller_type	= GAMEPAD_TYPE_NONE;

void tooltip_manager_init(Camera *gCamera)
{
	howto_tooltip = tooltip_create(how_to_play_tooltip, gCamera);
	new_skill_tooltip = tooltip_create(skills_tooltip, gCamera);
	new_artifact_tooltip = tooltip_create(artifacts_tooltip, gCamera);
}

void tooltip_manager_set_controller_mode(GamepadType mode)
{
	controller_type = mode;
}

Sprite* tooltip_manager_get_tooltip(TooltipType type)
{
	switch (type) {
		case TOOLTIP_TYPE_HOWTO:
			return howto_tooltip->sprite[controller_type];
			break;
		case TOOLTIP_TYPE_SKILL:
			return new_skill_tooltip->sprite[controller_type];
		case TOOLTIP_TYPE_ARTIFACT:
			return new_artifact_tooltip->sprite[controller_type];
		default:
			break;
	}

	return NULL;
}

void tooltip_manager_close()
{
	tooltip_destroy(new_skill_tooltip);
	tooltip_destroy(howto_tooltip);
	tooltip_destroy(new_artifact_tooltip);
}
