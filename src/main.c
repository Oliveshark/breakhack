#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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

static SDL_Window	*gWindow	= NULL;
static SDL_Renderer	*gRenderer	= NULL;
static Player		*gPlayer	= NULL;
static Map		*gMap		= NULL;
static RoomMatrix	*gRoomMatrix	= NULL;
static Gui		*gGui		= NULL;
static Pointer		*gPointer	= NULL;
static unsigned int	cLevel		= 1;
static float		deltaTime	= 1.0;
static double		renderScale	= 1.0;
static Menu		*mainMenu	= NULL;
static Menu		*inGameMenu	= NULL;
static Timer		*menuTimer	= NULL;
static GameState	gGameState;
static Camera		gCamera;
static SDL_Rect		gameViewport;
static SDL_Rect		bottomGuiViewport;
static SDL_Rect		rightGuiViewport;
static SDL_Rect		menuViewport;

static SDL_Color C_MENU_DEFAULT	= { 255, 255, 0, 0 };
static SDL_Color C_MENU_HOVER	= { 255, 0, 0, 0 };

struct MENU_ITEM {
	char label[20];
	void (*callback)(void*);
};

static void resetGame(void);
static void initMainMenu(void);

static
bool initSDL(void)
{
	int imgFlags = IMG_INIT_PNG;
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

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		error("Could not initiate SDL2: %s", SDL_GetError());
		return false;
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

	gWindow = SDL_CreateWindow("Breakhack",
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

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
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

	bottomGuiViewport = (SDL_Rect) { 0, GAME_VIEW_HEIGHT,
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
	gCamera.renderer = gRenderer;
	gRoomMatrix = roommatrix_create();
	gGui = gui_create(gRenderer);
	item_builder_init(gRenderer);
	gPointer = pointer_create(gRenderer);
	particle_engine_init();
	menuTimer = timer_create();

	return true;
}

static void
startGame(void *unused)
{
	UNUSED(unused);
	cLevel = 1;
	gGameState = PLAYING;
	resetGame();
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
	if (gGameState == PLAYING)
		gGameState = IN_GAME_MENU;
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
}

static void
createMenu(Menu **menu, struct MENU_ITEM menu_items[], unsigned int size)
{
	if (*menu == NULL)
		*menu = menu_create();

	for (unsigned int i = 0; i < size; ++i) {
		int hcenter;

		Sprite *s1 = sprite_create();
		sprite_load_text_texture(s1, "assets/GUI/SDS_8x8.ttf", 0, 25);
		texture_load_from_text(s1->textures[0], menu_items[i].label,
				       C_MENU_DEFAULT, gRenderer);

		hcenter = (SCREEN_WIDTH/2) - (s1->textures[0]->dim.width/2);
		s1->pos = (Position) { hcenter, 200 + (i*50) };
		s1->fixed = true;

		Sprite *s2 = sprite_create();
		sprite_load_text_texture(s2, "assets/GUI/SDS_8x8.ttf", 0, 25);
		texture_load_from_text(s2->textures[0], menu_items[i].label,
				       C_MENU_HOVER, gRenderer);

		s2->pos = (Position) { hcenter, 200 + (i*50) };
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

	info("Building new map");
	gMap = map_lua_generator_run(cLevel, gRenderer);
	gPlayer->sprite->pos = (Position) {
		TILE_DIMENSION, TILE_DIMENSION };

	map_set_current_room(gMap, &gPlayer->sprite->pos);
	camera_follow_position(&gCamera, &gPlayer->sprite->pos);
}

static bool
init(void)
{
	bool result = true;
	result = result && initSDL();
	result = result && initGame();
	initMainMenu();

	gCamera.pos = (Position) { 0, 0 };

	gGameState = MENU;

	return result;
}

static void
loadMedia(void)
{
	gPlayer = player_create(WARRIOR, gRenderer);
}

static bool
handle_main_events(SDL_Event *event)
{
	if (gGameState == PLAYING || gGameState == IN_GAME_MENU) {
		if (keyboard_press(SDLK_ESCAPE, event)) {
			toggleInGameMenu(NULL);
			return true;
		}
	}
	return false;
}

static bool
handle_events(void)
{
	static SDL_Event event;
	bool quit = false;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			quit = true;
			continue;
		}

		if (handle_main_events(&event))
			continue;

		if (gGameState == PLAYING) {
			gPlayer->handle_event(gPlayer,
					      gRoomMatrix,
					      &event);
			camera_follow_position(&gCamera, &gPlayer->sprite->pos);
			map_set_current_room(gMap, &gPlayer->sprite->pos);
			roommatrix_handle_event(gRoomMatrix, &event);
		} else if (gGameState == MENU) {
			menu_handle_event(mainMenu, &event);
		} else if (gGameState == IN_GAME_MENU) {
			menu_handle_event(inGameMenu, &event);
		}
		pointer_handle_event(gPointer, &event);
	}

	return quit;
}

static bool
check_if_dead(void)
{
	if (gPlayer->stats.hp <= 0) {
		gui_log("The dungeon consumed you");
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
		++cLevel;
		resetGame();
	}
}

static void
run_game(void)
{
	map_clear_dead_monsters(gMap);
	map_clear_collected_items(gMap);
	roommatrix_populate_from_map(gRoomMatrix, gMap);
	roommatrix_add_lightsource(gRoomMatrix,
		&gPlayer->sprite->pos);

	roommatrix_build_lightmap(gRoomMatrix);

	gui_update_player_stats(gGui, gPlayer, gMap, gRenderer);
	particle_engine_update(deltaTime);

	if (gPlayer->steps >= gPlayer->stats.speed) {
		player_reset_steps(gPlayer);
		roommatrix_update_with_player(gRoomMatrix, gPlayer);
		map_move_monsters(gMap, gRoomMatrix);
	}

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	SDL_RenderClear(gRenderer);

	SDL_RenderSetViewport(gRenderer, &gameViewport);
	map_render(gMap, &gCamera);
	particle_engine_render(&gCamera);
	player_render(gPlayer, &gCamera);

	if (gPlayer->class == MAGE || gPlayer->class == PALADIN)
		roommatrix_render_mouse_square(gRoomMatrix, &gCamera);
	roommatrix_render_lightmap(gRoomMatrix, &gCamera);

	SDL_RenderSetViewport(gRenderer, &rightGuiViewport);
	gui_render_panel(gGui, RIGHT_GUI_WIDTH,
			 RIGHT_GUI_HEIGHT, &gCamera);

	SDL_RenderSetViewport(gRenderer, &bottomGuiViewport);
	gui_render_log(gGui, BOTTOM_GUI_WIDTH,
		       BOTTOM_GUI_HEIGHT, &gCamera);

	SDL_RenderSetViewport(gRenderer, NULL);
	if (gGameState == IN_GAME_MENU) {
		SDL_Rect dimmer = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 150);
		SDL_RenderFillRect(gRenderer, &dimmer);
		menu_render(inGameMenu, &gCamera);
	}
	pointer_render(gPointer, &gCamera);

	SDL_RenderPresent(gRenderer);

	if (check_if_dead())
		gGameState = GAME_OVER;

	check_next_level();
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

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	SDL_RenderClear(gRenderer);
	SDL_RenderSetViewport(gRenderer, &menuViewport);
	map_render(gMap, &gCamera);
	roommatrix_render_lightmap(gRoomMatrix, &gCamera);

	SDL_RenderSetViewport(gRenderer, NULL);
	menu_render(mainMenu, &gCamera);
	pointer_render(gPointer, &gCamera);

	SDL_RenderPresent(gRenderer);
}

static
void run(void)
{
	static int oldTime = 0;
	static int currentTime = 0;

	bool quit = false;
	Timer* fpsTimer = timer_create();

	gui_log("The Dungeon Crawl begins!");

	while (!quit)
	{
		timer_start(fpsTimer);

		quit = handle_events();

		switch (gGameState) {
			case PLAYING:
			case IN_GAME_MENU:
				run_game();
				break;
			case MENU:
				run_menu();
				break;
			case GAME_OVER:
				fatal("GAME_OVER not implemented");
				break;
			case QUIT:
				quit = true;
				break;
			default:
				break;
		}

		int ticks = timer_get_ticks(fpsTimer);
		if (ticks < 1000/60)
			SDL_Delay((1000/60) - ticks);
		timer_stop(fpsTimer);

		if (currentTime == 0)
			currentTime = SDL_GetTicks();
		else {
			oldTime = currentTime;
			currentTime = SDL_GetTicks();
			deltaTime = (currentTime - oldTime) / 1000.0;
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

	roommatrix_destroy(gRoomMatrix);
	gui_destroy(gGui);
	pointer_destroy(gPointer);
	item_builder_close();
	particle_engine_close();
	timer_destroy(menuTimer);

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
	UNUSED(argv);

	if (!init())
		return 1;

	loadMedia();
	run();
	close();

	return 0;
}
