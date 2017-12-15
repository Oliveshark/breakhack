#include "monster.h"
#include "util.h"
#include "player.h"
#include "monster.h"

static void
monster_load_texts(Monster *m, SDL_Renderer *renderer)
{
	ActionText *t = actiontext_create();
	actiontext_load_font(t, "assets/GUI/SDS_6x6.ttf", 14);
	t->color = (SDL_Color) { 255, 100, 0 };
	actiontext_set_text(t, "HIT", renderer);
	t->pos = m->sprite->pos;
	m->hitText = t;

	t = actiontext_create();
	actiontext_load_font(t, "assets/GUI/SDS_6x6.ttf", 14);
	t->color = (SDL_Color) { 255, 255, 0 };
	actiontext_set_text(t, "MISS", renderer);
	t->pos = m->sprite->pos;
	m->missText = t;
}

Monster*
monster_create(SDL_Renderer *renderer)
{
	Monster *m = ec_malloc(sizeof(Monster));
	m->sprite = sprite_create();
	m->sprite->clip = (SDL_Rect) { 0, 0, 16, 16 };
	m->stats = (Stats) { 11, 1, 0, 0, 1 };

	monster_load_texts(m, renderer);

	return m;
}

void
monster_update_pos(Monster *m, Position pos)
{
	m->sprite->pos = pos;

	Position textPos = pos;
	textPos.y += 10;
	m->hitText->pos = textPos;
	m->missText->pos = textPos;
}

void
monster_render(Monster *m, Camera *cam)
{
	sprite_render(m->sprite, cam);
	if (m->hitText)
		actiontext_render(m->hitText, cam);
	if (m->missText)
		actiontext_render(m->missText, cam);
}

void
monster_destroy(Monster *m)
{
	sprite_destroy(m->sprite);
	if (m->hitText)
		actiontext_destroy(m->hitText);
	if (m->missText)
		actiontext_destroy(m->missText);
	free(m);
}
