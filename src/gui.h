#ifndef GUI_H_
#define	GUI_H_

#define LOG_LINES_COUNT	15
#define LOG_FONT_SIZE	8
#define LABEL_FONT_SIZE	8

#include "linkedlist.h"
#include "hashtable.h"
#include "sprite.h"
#include "camera.h"
#include "player.h"

typedef enum Label_e {
	LEVEL_LABEL,
	CURRENT_XP_LABEL,
	GOLD_LABEL,
	DUNGEON_LEVEL_LABEL,
	LABEL_COUNT
} LabelIndex;

typedef struct {
	LinkedList *sprites;
	LinkedList *health;
	LinkedList *xp_bar;
	Hashtable *textures;
	Sprite *labels[LABEL_COUNT];
	Texture *log_lines[LOG_LINES_COUNT];
} Gui;

Gui*
gui_create(SDL_Renderer *renderer);

void
gui_update_player_stats(Gui*, Player*, Map*, SDL_Renderer*);

void
gui_render_panel(Gui*, unsigned int width, unsigned int height, Camera*);

void
gui_render_log(Gui*, unsigned int width, unsigned int height, Camera*);

void
gui_log(const char *fmt, ...);

void
gui_destroy(Gui*);

#endif // GUI_H_
