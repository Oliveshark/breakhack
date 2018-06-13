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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <physfs.h>

#include "linkedlist.h"
#include "player.h"
#include "screenresolution.h"
#include "dimension.h"
#include "camera.h"
#include "map.h"
#include "map_lua.h"
#include "timer.h"
#include "roommatrix.h"
#include "gamestate.h"
#include "gui.h"
#include "util.h"
#include "item_builder.h"
#include "pointer.h"
#include "gui_button.h"
#include "particle_engine.h"
#include "menu.h"
#include "keyboard.h"
#include "mixer.h"
#include "random.h"
#include "skillbar.h"
#include "texturecache.h"
#include "update_data.h"
#include "settings.h"
#include "actiontextbuilder.h"
#include "input.h"

typedef enum Turn_t {
	PLAYER,
	MONSTER
} Turn;

static SDL_Window	*gWindow	= NULL;
static SDL_Renderer	*gRenderer	= NULL;
static Player		*gPlayer	= NULL;
static Map		*gMap		= NULL;
static RoomMatrix	*gRoomMatrix	= NULL;
static Gui		*gGui		= NULL;
static SkillBar		*gSkillBar	= NULL;
static Pointer		*gPointer	= NULL;
static unsigned int	cLevel		= 1;
static float		deltaTime	= 1.0;
static double		renderScale	= 1.0;
static Menu		*mainMenu	= NULL;
static Menu		*inGameMenu	= NULL;
static Timer		*menuTimer	= NULL;
static GameState	gGameState;
static Camera		*gCamera;
static SDL_Rect		gameViewport;
static SDL_Rect		skillBarViewport;
static SDL_Rect		bottomGuiViewport;
static SDL_Rect		rightGuiViewport;
static SDL_Rect		menuViewport;
static Turn		currentTurn	= PLAYER;
static Input		input;

static SDL_Color C_MENU_DEFAULT		= { 255, 255, 0, 255 };
static SDL_Color C_MENU_OUTLINE_DEFAULT	= { 0,	0, 0, 255 };
static SDL_Color C_MENU_HOVER		= { 255, 0, 0, 255 };

struct MENU_ITEM {
	char label[20];
	void (*callback)(void*);
};

static void resetGame(void);
static void initMainMenu(void);
static bool is_player_dead(void);

static
bool initSDL(void)
{
	int imgFlags = IMG_INIT_PNG;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		error("Could not initiate SDL2: %s", SDL_GetError());
		return false;
	}

	Dimension dim = getScreenDimensions();

	if (dim.height > 1080) {
		info("Hi resolution screen detected (%u x %u)", dim.width, dim.height);
		renderScale = ((double) dim.height)/1080;
		if (renderScale > 2)
			renderScale = 3;
		else if (renderScale > 1)
			renderScale = 2;
		info("Scaling by %f", renderScale);
	}

	if ( (IMG_Init(imgFlags) & imgFlags) == 0 ) {
		error("Unable to initiate img loading: %s",
		       IMG_GetError());
		return false;
	}

	if ( TTF_Init() == -1 ) {
		error("Unable to initiate ttf library: %s",
		       TTF_GetError());
		return false;
	}

	mixer_init();

	char title_buffer[100];
	m_sprintf(title_buffer, 100, "%s %d.%d.%d %s", GAME_TITLE, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, RELEASE_TYPE);
	gWindow = SDL_CreateWindow(title_buffer,
				   SDL_WINDOWPOS_UNDEFINED,
				   SDL_WINDOWPOS_UNDEFINED, 
				   (int)(SCREEN_WIDTH * renderScale),
				   (int)(SCREEN_HEIGHT * renderScale),
				   SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		error("Unable to create window: %s", SDL_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1,
				       SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (gRenderer == NULL)
	{
		error("Unable to create renderer: %s", SDL_GetError());
		return false;
	}
	if (SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND) < 0) {
		error("Unable to set blend mode: %s", SDL_GetError());
		return false;
	}
	if (SDL_RenderSetLogicalSize(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT) < 0)
	{
		error("Unable to initiate scaling: %s",
		       SDL_GetError());
		return false;
	}
		
	return true;
}

static void
initViewports(void)
{
	gameViewport = (SDL_Rect) { 0, 0,
		GAME_VIEW_WIDTH, GAME_VIEW_HEIGHT };

	skillBarViewport = (SDL_Rect) { 0, GAME_VIEW_HEIGHT,
		SKILL_BAR_WIDTH, SKILL_BAR_HEIGHT };

	bottomGuiViewport = (SDL_Rect) { 0, GAME_VIEW_HEIGHT + SKILL_BAR_HEIGHT,
		BOTTOM_GUI_WIDTH, BOTTOM_GUI_WIDTH };

	rightGuiViewport = (SDL_Rect) { GAME_VIEW_WIDTH, 0,
		RIGHT_GUI_WIDTH, RIGHT_GUI_HEIGHT };

	menuViewport = (SDL_Rect) {
		(SCREEN_WIDTH - GAME_VIEW_WIDTH)/2,
		(SCREEN_HEIGHT - GAME_VIEW_HEIGHT)/2,
		GAME_VIEW_WIDTH,
		GAME_VIEW_HEIGHT
	};
}

static bool
initGame(void)
{
	initViewports();
	input_init(&input);
	texturecache_init(gRenderer);
	gCamera = camera_create(gRenderer);
	gRoomMatrix = roommatrix_create();
	gGui = gui_create(gCamera);
	gSkillBar = skillbar_create(gRenderer);
	item_builder_init(gRenderer);
	gPointer = pointer_create(gRenderer);
	particle_engine_init();
	menuTimer = timer_create();
	actiontextbuilder_init(gRenderer);

	return true;
}

static void
startGame(void *unused)
{
	UNUSED(unused);
	cLevel = 1;
	gGameState = PLAYING;
	if (gPlayer)
		player_destroy(gPlayer);
	gPlayer = player_create(WARRIOR, gRenderer);
	mixer_play_music(GAME_MUSIC0 + get_random(2));
	resetGame();
	gui_clear_message_log();
	gui_log("The Dungeon Crawl begins!");
	gui_event_message("Welcome to the dungeon!");
}

static void
exitGame(void *unused)
{
	UNUSED(unused);
	gGameState = QUIT;
}

static void
toggleInGameMenu(void *unused)
{
	UNUSED(unused);
	if (gGameState == PLAYING || gGameState == GAME_OVER)
		gGameState = IN_GAME_MENU;
	else if (is_player_dead())
		gGameState = GAME_OVER;
	else
		gGameState = PLAYING;
}

static void
goToMainMenu(void *unused)
{
	UNUSED(unused);
	gGameState = MENU;
	menu_destroy(inGameMenu);
	inGameMenu = NULL;
	initMainMenu();
	Position p = { 0, 0 };
	gPlayer->sprite->pos = (Position) { 32, 32 };
	map_set_current_room(gMap, &p);
	camera_follow_position(gCamera, &p);
}

static void
createMenu(Menu **menu, struct MENU_ITEM menu_items[], unsigned int size)
{
	if (*menu == NULL)
		*menu = menu_create();

	for (unsigned int i = 0; i < size; ++i) {
		unsigned int hcenter;

		Sprite *s1 = sprite_create();
		sprite_load_text_texture(s1, "GUI/SDS_8x8.ttf", 0, 25, 2);
		texture_load_from_text(s1->textures[0], menu_items[i].label,
				       C_MENU_DEFAULT, C_MENU_OUTLINE_DEFAULT, gRenderer);

		hcenter = (SCREEN_WIDTH/2) - (s1->textures[0]->dim.width/2);
		s1->pos = (Position) { (int) hcenter, (int) 200 + (i*50) };
		s1->dim = s1->textures[0]->dim;
		s1->fixed = true;

		Sprite *s2 = sprite_create();
		sprite_load_text_texture(s2, "GUI/SDS_8x8.ttf", 0, 25, 2);
		texture_load_from_text(s2->textures[0], menu_items[i].label,
				       C_MENU_HOVER, C_MENU_OUTLINE_DEFAULT, gRenderer);

		s2->pos = (Position) { (int) hcenter, (int) 200 + (i*50) };
		s2->dim = s2->textures[0]->dim;
		s2->fixed = true;

		menu_item_add(*menu, s1, s2, menu_items[i].callback);
	}
}

static void
initInGameMenu(void)
{
	struct MENU_ITEM menu_items[] = {
		{ "RESUME", toggleInGameMenu },
		{ "MAIN MENU", goToMainMenu },
		{ "QUIT", exitGame },
	};

	createMenu(&inGameMenu, menu_items, 3);
}

static void
initMainMenu(void)
{
	struct MENU_ITEM menu_items[] = {
		{ "PLAY", startGame },
		{ "QUIT", exitGame },
	};

	if (gMap)
		map_destroy(gMap);

	gMap = map_lua_generator_single_room__run(cLevel, gRenderer);

	createMenu(&mainMenu, menu_items, 2);
	mixer_play_music(MENU_MUSIC);
}

static void
resetGame(void)
{
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

	if (mainMenu) {
		menu_destroy(mainMenu);
		mainMenu = NULL;
	}

	if (!inGameMenu)
		initInGameMenu();

	if (gMap)
		map_destroy(gMap);

	particle_engine_clear();

	info("Building new map");
	gMap = map_lua_generator_run(cLevel, gRenderer);

	gPlayer->sprite->pos = (Position) {
		TILE_DIMENSION, TILE_DIMENSION };

	map_set_current_room(gMap, &gPlayer->sprite->pos);
	camera_follow_position(gCamera, &gPlayer->sprite->pos);
}

static bool
init(void)
{
	if (!initSDL()) {
		return false;
	} else if (!initGame()) {
		return false;
	}

	settings_init();
	initMainMenu();

	gCamera->pos = (Position) { 0, 0 };

	gGameState = MENU;

	return true;
}

static bool
handle_main_input(void)
{
	if (gGameState == PLAYING
		|| gGameState == IN_GAME_MENU
		|| gGameState == GAME_OVER)
	{
		if (input_key_is_pressed(&input, KEY_ESC)) {
			toggleInGameMenu(NULL);
			return true;
		}
	}

	if (input_modkey_is_pressed(&input, KEY_CTRL_M)) {
		if (mixer_toggle_music(&gGameState))
			gui_log("Music enabled");
		else
			gui_log("Music disabled");
		return true;
	}

	if (input_modkey_is_pressed(&input, KEY_CTRL_S)) {
		if (mixer_toggle_sound())
			gui_log("Sound enabled");
		else
			gui_log("Sound disabled");
		return true;
	}

	return false;
}

static bool
handle_events(void)
{
	static SDL_Event event;
	bool quit = false;
	int handleCount = 0;

	input_reset(&input);
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			quit = true;
			continue;
		}

		input_handle_event(&input, &event);

		handleCount++;
		if (handleCount >= 20) {
			debug("Flushing event queue");
			SDL_PumpEvents();
			SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
			break;
		}
	}

	return quit;
}

static bool
is_player_dead(void)
{
	if (gPlayer->stats.hp <= 0) {
		return true;
	}
	return false;
}

static void
check_next_level(void)
{
	Room *room = gMap->rooms[gMap->currentRoom.x][gMap->currentRoom.y];
	Position pos = position_to_matrix_coords(&gPlayer->sprite->pos);

	MapTile *tile = room->tiles[pos.x][pos.y];
	if (!tile) {
		error("Looks like we are out of place");
		return;
	}
	if (tile->levelExit) {
		mixer_play_effect(NEXT_LEVEL);
		mixer_play_music(GAME_MUSIC0 + get_random(2));
		++cLevel;
		resetGame();
	}
}

static void
populateUpdateData(UpdateData *data, float deltatime)
{
	data->player = gPlayer;
	data->map = gMap;
	data->matrix = gRoomMatrix;
	data->input = &input;
	data->deltatime = deltatime;
}

static void
run_game(void)
{
	static UpdateData updateData;
	static unsigned int playerLevel = 1;

	if (gGameState == IN_GAME_MENU)
		menu_update(inGameMenu, &input);

	map_clear_dead_monsters(gMap, gPlayer);
	map_clear_collected_items(gMap);
	roommatrix_populate_from_map(gRoomMatrix, gMap);
	roommatrix_add_lightsource(gRoomMatrix,
		&gPlayer->sprite->pos);

	roommatrix_build_lightmap(gRoomMatrix);

	populateUpdateData(&updateData, deltaTime);
	if (playerLevel != gPlayer->stats.lvl) {
		playerLevel = gPlayer->stats.lvl;
		skillbar_check_skill_activation(gSkillBar, gPlayer);
	}

	if (gGameState == PLAYING && currentTurn == PLAYER)
		player_update(&updateData);

	gui_update_player_stats(gGui, gPlayer, gMap, gRenderer);
	camera_update(gCamera, updateData.deltatime);
	particle_engine_update(deltaTime);
	roommatrix_update(&updateData);
	actiontextbuilder_update(&updateData);
	skillbar_update(gSkillBar, &updateData);
	camera_follow_position(gCamera, &gPlayer->sprite->pos);
	map_set_current_room(gMap, &gPlayer->sprite->pos);
	map_update(&updateData);

	if (currentTurn == PLAYER) {
		if (player_turn_over(gPlayer)) {
			currentTurn = MONSTER;
			player_reset_steps(gPlayer);
		}
	} else if (currentTurn == MONSTER) {
		if (map_move_monsters(gMap, gRoomMatrix)) {
			currentTurn = PLAYER;
		}
	}

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	SDL_RenderClear(gRenderer);

	SDL_RenderSetViewport(gRenderer, &gameViewport);
	map_render(gMap, gCamera);
	particle_engine_render_game(gCamera);

	if (!is_player_dead())
		player_render(gPlayer, gCamera);

	map_render_top_layer(gMap, gCamera);

	if (gPlayer->class == MAGE || gPlayer->class == PALADIN)
		roommatrix_render_mouse_square(gRoomMatrix, gCamera);

	roommatrix_render_lightmap(gRoomMatrix, gCamera);
	actiontextbuilder_render(gCamera);
	gui_render_event_message(gGui, gCamera);

	SDL_RenderSetViewport(gRenderer, &rightGuiViewport);
	gui_render_panel(gGui, gCamera);

	SDL_RenderSetViewport(gRenderer, &skillBarViewport);
	skillbar_render(gSkillBar, gPlayer, gCamera);

	SDL_RenderSetViewport(gRenderer, &bottomGuiViewport);
	gui_render_log(gGui, gCamera);

	SDL_RenderSetViewport(gRenderer, NULL);
	particle_engine_render_global(gCamera);

	if (gGameState == IN_GAME_MENU) {
		SDL_Rect dimmer = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 150);
		SDL_RenderFillRect(gRenderer, &dimmer);
		menu_render(inGameMenu, gCamera);
	}
	pointer_render(gPointer, gCamera);

	SDL_RenderPresent(gRenderer);

	if (gGameState == PLAYING && is_player_dead()) {
		camera_shake(VECTOR2D_RIGHT, 800);
		gui_log("The dungeon consumed you");
		gui_event_message("You died!");
		mixer_play_effect(SPLAT);
		gGameState = GAME_OVER;
	} else {
		check_next_level();
	}
}

static void
run_menu(void)
{
	if (!timer_started(menuTimer))
		timer_start(menuTimer);

	roommatrix_populate_from_map(gRoomMatrix, gMap);
	roommatrix_build_lightmap(gRoomMatrix);
	if (timer_get_ticks(menuTimer) > 1000) {
		timer_stop(menuTimer);
		timer_start(menuTimer);
		map_move_monsters(gMap, gRoomMatrix);
	}

	menu_update(mainMenu, &input);
	if (gGameState != MENU)
		return;

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	SDL_RenderClear(gRenderer);
	SDL_RenderSetViewport(gRenderer, &menuViewport);
	map_render(gMap, gCamera);
	roommatrix_render_lightmap(gRoomMatrix, gCamera);

	SDL_RenderSetViewport(gRenderer, NULL);
	menu_render(mainMenu, gCamera);
	pointer_render(gPointer, gCamera);

	SDL_RenderPresent(gRenderer);
}

static
void run(void)
{
	static int oldTime = 0;
	static int currentTime = 0;

	bool quit = false;
	Timer* fpsTimer = timer_create();

	while (!quit)
	{
		timer_start(fpsTimer);

		quit = handle_events();
		handle_main_input();
		pointer_handle_input(gPointer, &input);

		switch (gGameState) {
			case PLAYING:
			case IN_GAME_MENU:
			case GAME_OVER:
				run_game();
				break;
			case MENU:
				run_menu();
				break;
			case QUIT:
				quit = true;
				break;
			default:
				break;
		}

		unsigned int ticks = timer_get_ticks(fpsTimer);
		if (ticks < 1000/60)
			SDL_Delay((1000/60) - ticks);
		timer_stop(fpsTimer);

		if (currentTime == 0)
			currentTime = SDL_GetTicks();
		else {
			oldTime = currentTime;
			currentTime = SDL_GetTicks();
			deltaTime = (float) ((currentTime - oldTime) / 1000.0);
		}
	}

	timer_destroy(fpsTimer);
}

static
void close(void)
{
	if (gPlayer)
		player_destroy(gPlayer);
	if (gMap)
		map_destroy(gMap);
	if (mainMenu)
		menu_destroy(mainMenu);
	if (inGameMenu)
		menu_destroy(inGameMenu);

	camera_destroy(gCamera);
	roommatrix_destroy(gRoomMatrix);
	gui_destroy(gGui);
	skillbar_destroy(gSkillBar);
	pointer_destroy(gPointer);
	actiontextbuilder_close();
	item_builder_close();
	particle_engine_close();
	timer_destroy(menuTimer);
	mixer_close();
	texturecache_close();
	settings_close();

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	UNUSED(argc);

	PHYSFS_init(argv[0]);
#ifndef DEBUG
	PHYSFS_mount("assets.pack", NULL, 0);
	PHYSFS_mount("data.pack", NULL, 0);
#else
	PHYSFS_mount("assets", NULL, 0);
	PHYSFS_mount("data", NULL, 0);
#endif // DEBUG

	if (!init())
		return 1;

	run();
	close();
	PHYSFS_deinit();

	return 0;
}
