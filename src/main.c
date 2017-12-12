#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <linkedlist.h>

#include "player.h"
#include "screenresolution.h"
#include "dimension.h"
#include "camera.h"
#include "map.h"
#include "map_lua.h"
#include "timer.h"
#include "roommatrix.h"

static SDL_Window	*gWindow	= NULL;
static SDL_Renderer	*gRenderer	= NULL;
static Player		*gPlayer	= NULL;
static LinkedList	*gSpriteList	= NULL;
static Map		*gMap		= NULL;
static RoomMatrix	*gRoomMatrix	= NULL;

static Camera		gCamera;

static
bool initSDL()
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
	gWindow = SDL_CreateWindow("Breakhack",
				   SDL_WINDOWPOS_UNDEFINED,
				   SDL_WINDOWPOS_UNDEFINED, 
				   SCREEN_WIDTH * scale,
				   SCREEN_HEIGHT * scale,
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
		printf("[!!] Unable to initiate scaling: %s\n", SDL_GetError());
		return false;
	}

	if ( (IMG_Init(imgFlags) & imgFlags) == 0 ) {
		printf("[!!] Unable to initiate img loading: %s\n", IMG_GetError());
		return false;
	}
		
	return true;
}

static
bool initGame()
{
	gSpriteList = linkedlist_create();
	gMap = map_lua_generator_run(gRenderer);
	return gSpriteList == NULL;
}

static
bool init()
{
	bool result = true;
	result = result && initSDL();
	result = result && initGame();
	if (result) {
		gCamera.pos = (Position) { 0, 0 };
		gCamera.renderer = gRenderer;
		gRoomMatrix = roommatrix_create();
	}
	return result;
}

static
void loadMedia()
{
	gPlayer = player_create(WARRIOR, gRenderer);
}

static
bool handle_events()
{
	static SDL_Event event;
	bool quit = false;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			quit = true;
		} else {
			gPlayer->sprite->handle_event(gPlayer->sprite,
						      gRoomMatrix,
						      &event);
			camera_follow_position(&gCamera, &gPlayer->sprite->pos);
			map_set_current_room(gMap, &gPlayer->sprite->pos);
		}
	}
	return quit;
}

static
void run()
{
	bool quit = false;
	Timer* fpsTimer = timer_create();

	while (!quit)
	{
		timer_start(fpsTimer);

		quit = handle_events();
		roommatrix_populate_from_map(gRoomMatrix, gMap);
		roommatrix_add_lightsource(gRoomMatrix,
					   &gPlayer->sprite->pos);
		roommatrix_build_lightmap(gRoomMatrix);

		SDL_RenderClear(gRenderer);

		map_render(gMap, &gCamera);
		sprite_render(gPlayer->sprite, &gCamera);
		roommatrix_render_lightmap(gRoomMatrix, &gCamera);

		SDL_RenderPresent(gRenderer);

		int ticks = timer_get_ticks(fpsTimer);
		if (ticks < 1000/60)
			SDL_Delay((1000/60) - ticks);
		timer_stop(fpsTimer);
	}

	timer_destroy(fpsTimer);
}

static
void close()
{
	player_destroy(gPlayer);
	map_destroy(gMap);
	roommatrix_destroy(gRoomMatrix);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char** argv)
{
	if (!init())
		return 1;

	loadMedia();
	run();
	close();

	return 0;
}
