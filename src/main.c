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

static SDL_Window	*gWindow	= NULL;
static SDL_Renderer	*gRenderer	= NULL;
static Player		*gPlayer	= NULL;
static LinkedList	*gSpriteList	= NULL;
static Map		*gMap		= NULL;
static RoomMatrix	*gRoomMatrix	= NULL;
static GameState	gameState;
static Camera		gCamera;

static
bool initSDL(void)
{
	int imgFlags = IMG_INIT_PNG;
	Dimension dim = getScreenDimensions();
	//Dimension dim = (Dimension) { 1920, 1080 };
	double scale = 1.0;

	if (dim.height > 1080) {
		printf("[**] Hi resolution screen detected (%u x %u)\n", dim.width, dim.height);
		scale = ((double) dim.height)/1080;
		printf("[**] Scaling by %f\n", scale);
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("[!!] Could not initiate SDL2: %s\n", SDL_GetError());
		return false;
	}

	if ( (IMG_Init(imgFlags) & imgFlags) == 0 ) {
		printf("[!!] Unable to initiate img loading: %s\n",
		       IMG_GetError());
		return false;
	}

	if ( TTF_Init() == -1 ) {
		printf("[!!] Unable to initiate ttf library: %s\n",
		       TTF_GetError());
		return false;
	}

	gWindow = SDL_CreateWindow("Breakhack",
				   SDL_WINDOWPOS_UNDEFINED,
				   SDL_WINDOWPOS_UNDEFINED, 
				   (int)(SCREEN_WIDTH * scale),
				   (int)(SCREEN_HEIGHT * scale),
				   SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
	{
		printf("[!!] Unable to create window: %s\n", SDL_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL)
	{
		printf("[!!] Unable to create renderer: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND) < 0) {
		printf("[!!] Unable to set blend mode: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_RenderSetLogicalSize(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT) < 0)
	{
		printf("[!!] Unable to initiate scaling: %s\n",
		       SDL_GetError());
		return false;
	}
		
	return true;
}

static
bool initGame(void)
{
	gSpriteList = linkedlist_create();
	gMap = map_lua_generator_run(gRenderer);
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
	}

	gameState = PLAYING;

	return result;
}

static
void loadMedia(void)
{
	gPlayer = player_create(ROGUE, gRenderer);
}

static
bool handle_events(void)
{
	static SDL_Event event;
	bool quit = false;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			quit = true;
		} else {
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
		printf("[!!] Looks like we are out of place\n");
		return;
	}
	if (tile->levelExit) {
		printf("[**] Building new map\n");
		map_destroy(gMap);
		gMap = map_lua_generator_run(gRenderer);
		gPlayer->sprite->pos = (Position) {
			TILE_DIMENSION, TILE_DIMENSION };
	}
}

static
void run(void)
{
	bool quit = false;
	Timer* fpsTimer = timer_create();

	while (!quit)
	{
		timer_start(fpsTimer);

		quit = handle_events();
		map_clear_dead_monsters(gMap);
		roommatrix_populate_from_map(gRoomMatrix, gMap);
		roommatrix_add_lightsource(gRoomMatrix,
					   &gPlayer->sprite->pos);
		roommatrix_build_lightmap(gRoomMatrix);

		if (gPlayer->steps == gPlayer->stats.speed) {
			player_reset_steps(gPlayer);
			roommatrix_update_with_player(gRoomMatrix, gPlayer);
			map_move_monsters(gMap, gRoomMatrix);
		}

		SDL_RenderClear(gRenderer);

		map_render(gMap, &gCamera);
		player_render(gPlayer, &gCamera);
		roommatrix_render_lightmap(gRoomMatrix, &gCamera);

		SDL_RenderPresent(gRenderer);

		check_next_level();

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
