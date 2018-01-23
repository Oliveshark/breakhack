#ifndef GUI_H_
#define	GUI_H_

#define LOG_LINES_COUNT	15
#define LOG_FONT_SIZE	8

#include "linkedlist.h"
#include "hashtable.h"
#include "sprite.h"
#include "camera.h"

typedef struct {
	LinkedList *sprites;
	LinkedList *health;
	Hashtable *textures;
	Texture *log_lines[LOG_LINES_COUNT];
} Gui;

Gui* gui_create(void);

void gui_set_max_health(Gui*, int max, SDL_Renderer*);

void gui_set_current_health(Gui*, int current);

Texture* gui_add_texture(Gui*, const char *path, SDL_Renderer*);

void gui_render_panel(Gui*, unsigned int width, unsigned int height, Camera*);

void gui_render_log(Gui*, unsigned int width, unsigned int height, Camera*);

void gui_log(const char *fmt, ...);

void gui_destroy(Gui*);

#endif // GUI_H_
