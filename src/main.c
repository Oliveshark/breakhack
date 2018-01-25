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

static SDL_Window	*gWindow	= NULL;
static SDL_Renderer	*gRenderer	= NULL;
static Player		*gPlayer	= NULL;
static LinkedList	*gSpriteList	= NULL;
static Map		*gMap		= NULL;
static RoomMatrix	*gRoomMatrix	= NULL;
static Gui		*gGui		= NULL;
static unsigned int	cLevel		= 1;
static double		renderScale	= 1.0;
static GameState	gGameState;
static Camera		gCamera;
static SDL_Rect		gameViewport;
static SDL_Rect		bottomGuiViewport;
static SDL_Rect		rightGuiViewport;

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
}

static
bool initGame(void)
{
	gSpriteList = linkedlist_create();
	initViewports();
	gMap = map_lua_generator_run(cLevel, gRenderer);
	return gSpriteList == NULL;
}

static
bool init(void)
{
	bool result = true;
	result = result && initSDL();
	result = result && initGame();
	if (result) {
		gCamera.pos = (Position) { 0, 0 };
		gCamera.renderer = gRenderer;
		gRoomMatrix = roommatrix_create();
		gGui = gui_create();
		item_builder_init(gRenderer);
	}

	gGameState = PLAYING;

	return result;
}

static
void loadMedia(void)
{
	gPlayer = player_create(PALADIN, gRenderer);
}

static
bool handle_events(void)
{
	static SDL_Event event;
	bool quit = false;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			quit = true;
		} else if (gGameState == PLAYING) {
			gPlayer->handle_event(gPlayer,
					      gRoomMatrix,
					      &event);
			camera_follow_position(&gCamera, &gPlayer->sprite->pos);
			map_set_current_room(gMap, &gPlayer->sprite->pos);
		}
	}
	return quit;
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
		info("Building new map");
		map_destroy(gMap);
		gMap = map_lua_generator_run(++cLevel, gRenderer);
		gPlayer->sprite->pos = (Position) {
			TILE_DIMENSION, TILE_DIMENSION };
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

	gui_set_max_health(gGui, gPlayer->stats.maxhp, gRenderer);
	gui_set_current_health(gGui, gPlayer->stats.hp);
	if (gPlayer->steps == gPlayer->stats.speed) {
		player_reset_steps(gPlayer);
		roommatrix_update_with_player(gRoomMatrix, gPlayer);
		map_move_monsters(gMap, gRoomMatrix);
	}

	SDL_RenderClear(gRenderer);

	SDL_RenderSetViewport(gRenderer, &gameViewport);
	map_render(gMap, &gCamera);
	player_render(gPlayer, &gCamera);
	roommatrix_render_lightmap(gRoomMatrix, &gCamera);

	SDL_RenderSetViewport(gRenderer, &rightGuiViewport);
	gui_render_panel(gGui, RIGHT_GUI_WIDTH,
			 RIGHT_GUI_HEIGHT, &gCamera);

	SDL_RenderSetViewport(gRenderer, &bottomGuiViewport);
	gui_render_log(gGui, BOTTOM_GUI_WIDTH,
		       BOTTOM_GUI_HEIGHT, &gCamera);

	SDL_RenderPresent(gRenderer);

	check_next_level();
}

static
void run(void)
{
	bool quit = false;
	Timer* fpsTimer = timer_create();

	gui_log("The Dungeon Crawl begins!");

	while (!quit)
	{
		timer_start(fpsTimer);

		quit = handle_events();

		switch (gGameState) {
			case PLAYING:
				run_game();
				break;
			case MENU:
				error("MENU not implemented");
				break;
			case IN_GAME_MENU:
				error("IN_GAME_MENU not implemented");
				break;
			case GAME_OVER:
				error("GAME_OVER not implemented");
				break;
			default:
				break;
		}

		int ticks = timer_get_ticks(fpsTimer);
		if (ticks < 1000/60)
			SDL_Delay((1000/60) - ticks);
		timer_stop(fpsTimer);
	}

	timer_destroy(fpsTimer);
}

static
void close(void)
{
	player_destroy(gPlayer);
	map_destroy(gMap);
	roommatrix_destroy(gRoomMatrix);
	gui_destroy(gGui);
	item_builder_close();
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
