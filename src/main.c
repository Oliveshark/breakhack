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
#include <string.h>
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
#include "screen.h"
#include "hiscore.h"
#include "io_util.h"
#include "tooltip.h"

#ifdef STEAM_BUILD
#include "steam/steamworks_api_wrapper.h"
#endif // STEAM_BUILD

static char *artifacts_tooltip[] = {
	"CONGRATULATIONS!", "",
	"",
	"   You just picked up your first artifact!", "",
	"",
	"   Your current artifacts and corresponding level are", "",
	"   listed next to your skills." "",
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
	"                      Controller: RIGHT STICK or D-PAD", "",
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


typedef enum Turn_t {
	PLAYER,
	MONSTER
} Turn;

static SDL_Window	*gWindow		= NULL;
static SDL_Renderer	*gRenderer		= NULL;
static Player		*gPlayer		= NULL;
static Map		*gMap			= NULL;
static RoomMatrix	*gRoomMatrix		= NULL;
static Gui		*gGui			= NULL;
static SkillBar		*gSkillBar		= NULL;
static Menu		*mainMenu		= NULL;
static Menu		*inGameMenu		= NULL;
static Menu		*charSelectMenu		= NULL;
static Timer		*menuTimer		= NULL;
static Camera		*gCamera		= NULL;
static Screen		*creditsScreen		= NULL;
static Screen		*scoreScreen		= NULL;
static Sprite		*new_skill_tooltip	= NULL;
static Sprite		*howto_tooltip		= NULL;
static Sprite		*new_artifact_tooltip	= NULL;
static SDL_GameController *gController		= NULL;
static unsigned int	cLevel			= 1;
static float		deltaTime		= 1.0;
static double		renderScale		= 1.0;
static Turn		currentTurn		= PLAYER;
static class_t		playerClass		= WARRIOR;
static GameState	gGameState;
static SDL_Rect		mainViewport;
static SDL_Rect		gameViewport;
static SDL_Rect		skillBarViewport;
static SDL_Rect		bottomGuiViewport;
static SDL_Rect		statsGuiViewport;
static SDL_Rect		minimapViewport;
static SDL_Rect		menuViewport;
static Input		input;
static Uint8		controllerMode = 0;

#ifdef DEBUG
static Sprite	*fpsSprite	= NULL;
static Pointer	*gPointer	= NULL;
#endif // DEBUG

static void resetGame(void);
static void initMainMenu(void);
static bool is_player_dead(void);

static
bool initSDL(void)
{
	int imgFlags = IMG_INIT_PNG;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		error("Could not initiate SDL2: %s", SDL_GetError());
		return false;
	}

	Dimension dim = getScreenDimensions();

	if (dim.height > 1080) {
		info("Hi resolution screen detected (%u x %u)", dim.width, dim.height);
		renderScale = ((double) dim.height)/1080;
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

	for (Uint8 i = 0; i < SDL_NumJoysticks(); ++i) {
		if (!SDL_IsGameController(i))
			continue;

		gController = SDL_GameControllerOpen(i);
		if (gController) {
			const char *ctrlName = SDL_GameControllerName(gController);
			info("Game controller connected: %s", ctrlName);
			if (ctrlName[0] == 'P' && ctrlName[1] == 'S' && ctrlName[2] == '4')
				controllerMode = 2;
			else
				controllerMode = 1;

			break;
		}
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

	// Set the window icon
	SDL_SetWindowIcon(gWindow, IMG_Load_RW(io_load_rwops("Extras/icon.png"), true));

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
initViewports(Uint32 offset)
{
	mainViewport = (SDL_Rect) { offset, 0,
		SCREEN_WIDTH, SCREEN_HEIGHT
	};

	gameViewport = (SDL_Rect) { offset, 0,
		GAME_VIEW_WIDTH, GAME_VIEW_HEIGHT };

	skillBarViewport = (SDL_Rect) { offset, GAME_VIEW_HEIGHT,
		SKILL_BAR_WIDTH, SKILL_BAR_HEIGHT };

	bottomGuiViewport = (SDL_Rect) { offset, GAME_VIEW_HEIGHT + SKILL_BAR_HEIGHT,
		BOTTOM_GUI_WIDTH, BOTTOM_GUI_WIDTH };

	statsGuiViewport = (SDL_Rect) { offset + GAME_VIEW_WIDTH, 0,
		RIGHT_GUI_WIDTH, STATS_GUI_HEIGHT };

	minimapViewport = (SDL_Rect) { offset + GAME_VIEW_WIDTH, STATS_GUI_HEIGHT,
		RIGHT_GUI_WIDTH, MINIMAP_GUI_HEIGHT };

	menuViewport = (SDL_Rect) {
		offset + ((SCREEN_WIDTH - GAME_VIEW_WIDTH) >> 1),
		(SCREEN_HEIGHT - GAME_VIEW_HEIGHT) >> 1,
		GAME_VIEW_WIDTH,
		GAME_VIEW_HEIGHT
	};
}

static bool
initGame(void)
{
	initViewports(0);
	input_init(&input);
	texturecache_init(gRenderer);
	gCamera = camera_create(gRenderer);
	gRoomMatrix = roommatrix_create();
	gGui = gui_create(gCamera);
	skillbar_set_controller_mode(controllerMode);
	gSkillBar = skillbar_create(gCamera);
	item_builder_init(gRenderer);
#ifdef DEBUG
	gPointer = pointer_create(gRenderer);
#endif // DEBUG
	particle_engine_init();
	menuTimer = timer_create();
	actiontextbuilder_init(gRenderer);

#ifdef DEBUG
	fpsSprite = sprite_create();
	sprite_load_text_texture(fpsSprite, "GUI/SDS_8x8.ttf", 0, 14, 1);
	fpsSprite->pos = POS(16, 16);
	fpsSprite->fixed = true;
#endif // DEBUG

	return true;
}

static void
startGame(void)
{
	cLevel = 1;
	gGameState = PLAYING;
	if (gPlayer)
		player_destroy(gPlayer);
	gPlayer = player_create(playerClass, gCamera);
	mixer_play_music(GAME_MUSIC0 + get_random(2));
	resetGame();
	skillbar_reset(gSkillBar);
	gui_clear_message_log();
	gui_log("The Dungeon Crawl begins!");
	gui_event_message("Welcome to the dungeon!");

	Settings *settings = settings_get();
	if (!settings->howto_tooltip_shown)
		gGui->activeTooltip = howto_tooltip;
	settings->howto_tooltip_shown = true;
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
	if (gGameState == PLAYING ||
	    gGameState == GAME_OVER ||
	    gGameState == COMPLETED)
		gGameState = IN_GAME_MENU;
	else if (is_player_dead())
		gGameState = GAME_OVER;
	else if (cLevel >= 20)
		gGameState = COMPLETED;
	else
		gGameState = PLAYING;
}

static void
on_character_select(const char *str)
{
	if (strcmp(str, "warrior") == 0)
		playerClass = WARRIOR;
	else if (strcmp(str, "rogue") == 0)
		playerClass = ROGUE;

	startGame();
}

static void
goToCharacterMenu(void *unused)
{
	UNUSED(unused);
	charSelectMenu = menu_create_character_selector(on_character_select);
	gGameState = CHARACTER_MENU;
}

static void
goToMainMenu(void *unused)
{
	UNUSED(unused);
	gui_clear_message_log();
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
showHowToTooltip(void *unused)
{
	UNUSED(unused);
	toggleInGameMenu(NULL);
	gGui->activeTooltip = howto_tooltip;
}

static void
initInGameMenu(void)
{
	static TEXT_MENU_ITEM menu_items[] = {
		{ "RESUME", toggleInGameMenu },
		{ "HOW TO PLAY", showHowToTooltip },
		{ "MAIN MENU", goToMainMenu },
		{ "QUIT", exitGame },
	};

	menu_create_text_menu(&inGameMenu, &menu_items[0], 4, gRenderer);
}

static void
createInGameGameOverMenu(void)
{
	static TEXT_MENU_ITEM menu_items[] = {
		{ "NEW GAME", goToCharacterMenu },
		{ "MAIN MENU", goToMainMenu },
		{ "QUIT", exitGame },
	};

	if (inGameMenu) {
		menu_destroy(inGameMenu);
		inGameMenu = NULL;
	}
	menu_create_text_menu(&inGameMenu, &menu_items[0], 3, gRenderer);
}

static void
viewCredits(void *unused)
{
	UNUSED(unused);
	gGameState = CREDITS;
}

static void
viewScoreScreen(void *unused)
{
	UNUSED(unused);
	gGameState = SCORE_SCREEN;
}

static void
initMainMenu(void)
{
	static TEXT_MENU_ITEM menu_items[] = {
		{ "PLAY", goToCharacterMenu },
		{ "SCORES", viewScoreScreen },
		{ "CREDITS", viewCredits },
		{ "QUIT", exitGame },
	};

	if (gMap)
		map_destroy(gMap);

	gMap = map_lua_generator_single_room__run(cLevel, gRenderer);

	menu_create_text_menu(&mainMenu, &menu_items[0], 4, gRenderer);
	mixer_play_music(MENU_MUSIC);
	creditsScreen = screen_create_credits(gRenderer);
	scoreScreen = screen_create_hiscore(gRenderer);
}

static void
repopulate_roommatrix(void)
{
	roommatrix_populate_from_map(gRoomMatrix, gMap);
	roommatrix_add_lightsource(gRoomMatrix,
		&gPlayer->sprite->pos);
	roommatrix_build_lightmap(gRoomMatrix);
}

static void
resetGame(void)
{
	SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

	if (mainMenu)
		menu_destroy(mainMenu);
	mainMenu = NULL;
	if (charSelectMenu)
		menu_destroy(charSelectMenu);
	charSelectMenu = NULL;

	if (creditsScreen)
		screen_destroy(creditsScreen);
	creditsScreen = NULL;

	if (scoreScreen)
		screen_destroy(scoreScreen);
	scoreScreen = NULL;

	if (inGameMenu)
		menu_destroy(inGameMenu);
	inGameMenu = NULL;
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
	repopulate_roommatrix();
}

static bool
init(void)
{
#ifdef STEAM_BUILD
	if (!steam_restart_needed()) {
		steam_init();
	} else {
		error("%s needs to be started through Steam", GAME_TITLE);
		return false;
	}
#endif // STEAM_BUILD

	if (!initSDL()) {
		return false;
	} else if (!initGame()) {
		return false;
	}

	settings_init();
	hiscore_init();
	initMainMenu();

	tooltip_set_controller_mode(controllerMode);
	howto_tooltip = tooltip_create(how_to_play_tooltip, gCamera);
	new_skill_tooltip = tooltip_create(skills_tooltip, gCamera);
	new_artifact_tooltip = tooltip_create(artifacts_tooltip, gCamera);

	gCamera->pos = (Position) { 0, 0 };

	gGameState = MENU;

	return true;
}

static void
toggle_fullscreen(void)
{
	bool isFullscreen = SDL_GetWindowFlags(gWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP;
	Settings *settings = settings_get();
	if (isFullscreen) {
		initViewports(0);
		SDL_SetWindowFullscreen(gWindow, 0);
		settings->fullscreen_enabled = false;
	}
	else {
		int w, h, lw, lh;
		SDL_RenderGetLogicalSize(gRenderer, &lw, &lh);
		SDL_GetWindowSize(gWindow, &w, &h);

		double lratio = (double) w / (double) lw;

		SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);

		SDL_DisplayMode dMode;
		SDL_GetWindowDisplayMode(gWindow, &dMode);
		double ratio = (double) (dMode.w) / w;
		double offset = ((dMode.w - w) / 2);
		initViewports((Uint32)(offset/(ratio*lratio)));
		settings->fullscreen_enabled = true;
	}
}

static void
handle_settings_input(void)
{
	if (input_modkey_is_pressed(&input, KEY_CTRL_M)) {
		if (mixer_toggle_music(&gGameState))
			gui_log("Music enabled");
		else
			gui_log("Music disabled");
	}

	if (input_modkey_is_pressed(&input, KEY_CTRL_S)) {
		if (mixer_toggle_sound())
			gui_log("Sound enabled");
		else
			gui_log("Sound disabled");
	}

	if (input_modkey_is_pressed(&input, KEY_CTRL_D)) {
		Settings *s = settings_get();
		s->tooltips_enabled = !s->tooltips_enabled;
		if (s->tooltips_enabled)
			gui_log("Tooltips enabled");
		else
			gui_log("Tooltips disabled");
	}

	if (input_modkey_is_pressed(&input, KEY_CTRL_F)) {
		toggle_fullscreen();
	}
}

static void
handle_main_input(void)
{
	if (input_key_is_pressed(&input, KEY_ESC)) {
		switch (gGameState) {
			case CREDITS:
			case SCORE_SCREEN:
			case CHARACTER_MENU:
				gGameState = MENU;
				break;
			case MENU:
				gGameState = QUIT;
				break;
			case PLAYING:
			case IN_GAME_MENU:
			case GAME_OVER:
			case COMPLETED:
				if (gGui->activeTooltip)
					gGui->activeTooltip = NULL;
				else
					toggleInGameMenu(NULL);
			default:
				break;
		}

	}

	handle_settings_input();
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
end_game_details(void)
{
	gui_log("You earned %.2f gold", gPlayer->gold);
	gui_event_message("You earned %.2f gold", gPlayer->gold);
	
	if (hiscore_get_top_gold() < gPlayer->gold) {
		gui_event_message("NEW HIGHSCORE");
		gui_log("NEW HIGHSCORE");
	}
}

static void
check_next_level(void)
{
	if (cLevel >= 20) {
		return;
	}

	Room *room = gMap->rooms[gMap->currentRoom.x][gMap->currentRoom.y];
	Position pos = position_to_matrix_coords(&gPlayer->sprite->pos);

	MapTile *tile = room->tiles[pos.x][pos.y];
	if (!tile) {
		error("Looks like we are out of place");
		return;
	}
	if (tile->levelExit) {
		mixer_play_effect(NEXT_LEVEL);
		++cLevel;
		if (cLevel > 19) {
			mixer_play_music(BOSS_MUSIC0);
		} else if (cLevel % 5 == 0) {
			gui_log("You sense something powerful in the vicinity");
			mixer_play_music(BOSS_MUSIC0);
		} else {
			mixer_play_music(GAME_MUSIC0 + get_random(2));
		}

		if (cLevel < 20) {
			resetGame();
		}
	}
}

static void
populateUpdateData(UpdateData *data, float deltatime)
{
	data->player = gPlayer;
	data->map = gMap;
	data->matrix = gRoomMatrix;
	data->input = &input;
	data->gui = gGui;
	data->deltatime = deltatime;
	data->cam = gCamera;
}

static bool
check_skillbar_activation(void)
{
	static unsigned int playerLevel = 1;

	if (playerLevel != gPlayer->stats.lvl) {
		playerLevel = gPlayer->stats.lvl;
		return skillbar_check_skill_activation(gSkillBar, gPlayer);
	}
	return false;
}

static void
check_tooltip_activation(bool skillActivated)
{
	static bool artifactTooltipShown = false;

	Settings *settings = settings_get();
	if (skillActivated && settings->tooltips_enabled) {
		gGui->activeTooltip = new_skill_tooltip;
	}
	if (!artifactTooltipShown && gPlayer->equipment.hasArtifacts) {
		artifactTooltipShown = true;
		if (settings->tooltips_enabled)
			gGui->activeTooltip = new_artifact_tooltip;
	}
}

static void
run_game_update(void)
{
	static UpdateData updateData;

	if (gGameState == IN_GAME_MENU)
		menu_update(inGameMenu, &input);

	populateUpdateData(&updateData, deltaTime);

	bool skillActivated = check_skillbar_activation();
	check_tooltip_activation(skillActivated);

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
			map_on_new_turn(gMap);
		}
	} else if (currentTurn == MONSTER) {
		if (map_move_monsters(gMap, gRoomMatrix)) {
			currentTurn = PLAYER;
		}
	}

	map_clear_expired_entities(gMap, gPlayer);
	repopulate_roommatrix();
}

static void
render_gui(void)
{
	SDL_RenderSetViewport(gRenderer, &statsGuiViewport);
	gui_render_panel(gGui, gCamera);
	SDL_RenderSetViewport(gRenderer, &minimapViewport);
	gui_render_minimap(gGui, gMap, gCamera);
	SDL_RenderSetViewport(gRenderer, &skillBarViewport);
	skillbar_render(gSkillBar, gPlayer, gCamera);
	SDL_RenderSetViewport(gRenderer, &bottomGuiViewport);
	gui_render_log(gGui, gCamera);
	SDL_RenderSetViewport(gRenderer, &mainViewport);
}

static void
render_game_completed(void)
{
	SDL_RenderSetViewport(gRenderer, &gameViewport);
	if (!is_player_dead()) {
		player_render(gPlayer, gCamera);
		player_render_toplayer(gPlayer, gCamera);
	}
	actiontextbuilder_render(gCamera);
	gui_render_event_message(gGui, gCamera);

	if (gGameState == IN_GAME_MENU) {
		SDL_Rect dimmer = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 150);
		SDL_RenderFillRect(gRenderer, &dimmer);
		menu_render(inGameMenu, gCamera);
	}
#ifdef DEBUG
	sprite_render(fpsSprite, gCamera);
	pointer_render(gPointer, gCamera);
#endif // DEBUG
}

static void
render_game(void)
{
	SDL_RenderSetViewport(gRenderer, &gameViewport);
	map_render(gMap, gCamera);
	particle_engine_render_game(gCamera);

	map_render_mid_layer(gMap, gCamera);

	if (!is_player_dead()) {
		player_render(gPlayer, gCamera);
		player_render_toplayer(gPlayer, gCamera);
	}

	map_render_top_layer(gMap, gRoomMatrix, gCamera);

	if (gPlayer->class == MAGE || gPlayer->class == PALADIN)
		roommatrix_render_mouse_square(gRoomMatrix, gCamera);

	roommatrix_render_lightmap(gRoomMatrix, gCamera);
	actiontextbuilder_render(gCamera);
	gui_render_event_message(gGui, gCamera);
}

static void
run_game_render(void)
{
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	SDL_RenderClear(gRenderer);

	render_game();
	render_gui();

	SDL_RenderSetViewport(gRenderer, &mainViewport);
	particle_engine_render_global(gCamera);
	gui_render_tooltip(gGui, gCamera);

	if (gGameState == IN_GAME_MENU) {
		SDL_Rect dimmer = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 150);
		SDL_RenderFillRect(gRenderer, &dimmer);
		menu_render(inGameMenu, gCamera);
	}
#ifdef DEBUG
	sprite_render(fpsSprite, gCamera);
	pointer_render(gPointer, gCamera);
#endif // DEBUG

	SDL_RenderPresent(gRenderer);
}

static void
run_game(void)
{
	run_game_update();

	if (cLevel >= 20) {
		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderClear(gRenderer);
		render_game_completed();
		render_gui();
		SDL_RenderPresent(gRenderer);
	} else {
		run_game_render();
	}

	if (gGameState == PLAYING && is_player_dead()) {
		camera_shake(VECTOR2D_RIGHT, 800);
		gui_log("The dungeon consumed you");
		gui_event_message("You died!");
		end_game_details();
		mixer_play_effect(SPLAT);
		gGameState = GAME_OVER;
		createInGameGameOverMenu();
		hiscore_register(gPlayer, cLevel);
#ifdef STEAM_BUILD
		uint8_t details[4] = { (uint8_t) gPlayer->stats.lvl, (uint8_t) cLevel, 0, 0 };
		steam_register_score((int) gPlayer->gold, (int32_t*) &details, 1);
		steam_register_kills((int) gPlayer->stat_data.kills, (int32_t*) &details, 1);
#endif // STEAM_BUILD

	} else {
		check_next_level();
	}

	if (gGameState == PLAYING && cLevel >= 20) {
		gGameState = COMPLETED;
		createInGameGameOverMenu();
		gui_event_message("Your break is over!");
		gui_log("Your break is over!");
		gui_event_message("Well done!");
		end_game_details();
#ifdef STEAM_BUILD
		steam_set_achievement(BACK_TO_WORK);
#endif // STEAM_BUILD
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

	if (gGameState != MENU
	    && gGameState != CREDITS
	    && gGameState != SCORE_SCREEN
	    && gGameState != CHARACTER_MENU)
		return;

	if (gGameState == MENU)
		menu_update(mainMenu, &input);
	else if (gGameState == CHARACTER_MENU)
		menu_update(charSelectMenu, &input);

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	SDL_RenderClear(gRenderer);
	if (gGameState != CHARACTER_MENU)  {
		SDL_RenderSetViewport(gRenderer, &menuViewport);
		map_render(gMap, gCamera);
		map_render_mid_layer(gMap, gCamera);
		map_render_top_layer(gMap, gRoomMatrix, gCamera);
		roommatrix_render_lightmap(gRoomMatrix, gCamera);
	}

	SDL_RenderSetViewport(gRenderer, &mainViewport);

	if (gGameState == MENU)
		menu_render(mainMenu, gCamera);
	if (gGameState == CHARACTER_MENU)
		menu_render(charSelectMenu, gCamera);
	else if (gGameState == CREDITS)
		screen_render(creditsScreen, gCamera);
	else if (gGameState == SCORE_SCREEN)
		screen_render(scoreScreen, gCamera);

#ifdef DEBUG
	sprite_render(fpsSprite, gCamera);
	pointer_render(gPointer, gCamera);
#endif // DEBUG

	SDL_RenderPresent(gRenderer);
}

static void
run(void)
{
	static int oldTime = 0;
	static int currentTime = 0;

	bool quit = false;

#ifdef DEBUG
	Uint32 frame = 0;
	Timer *fpsTime = timer_create();
	Timer *updateTimer = timer_create();
	timer_start(fpsTime);
	timer_start(updateTimer);
#endif // DEBUG

	Timer *fpsTimer = timer_create();

	while (!quit)
	{
		timer_start(fpsTimer);

		quit = handle_events();
		handle_main_input();
#ifdef DEBUG
		pointer_handle_input(gPointer, &input);
#endif // DEBUG

#ifdef STEAM_BUILD
		steam_run_callbacks();
#endif // STEAM_BUILD

		switch (gGameState) {
			case PLAYING:
			case IN_GAME_MENU:
			case GAME_OVER:
			case COMPLETED:
				run_game();
				break;
			case MENU:
			case CREDITS:
			case SCORE_SCREEN:
			case CHARACTER_MENU:
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
#ifdef DEBUG
		frame++;
		if (timer_get_ticks(updateTimer) > 1000) {
			char buffer[20];
			m_sprintf(buffer, 20, "FPS: %u", frame / (timer_get_ticks(fpsTime) / 1000));
			texture_load_from_text(fpsSprite->textures[0], buffer, C_RED, C_WHITE, gRenderer);
			fpsSprite->dim = fpsSprite->textures[0]->dim;
			timer_start(updateTimer);
		}
#endif // DEBUG
	}

	timer_destroy(fpsTimer);
#ifdef DEBUG
	timer_destroy(fpsTime);
	timer_destroy(updateTimer);
#endif // DEBUG
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
	if (creditsScreen)
		screen_destroy(creditsScreen);
	if (scoreScreen)
		screen_destroy(scoreScreen);
	if (inGameMenu)
		menu_destroy(inGameMenu);

	sprite_destroy(howto_tooltip);
	sprite_destroy(new_skill_tooltip);
	camera_destroy(gCamera);
	roommatrix_destroy(gRoomMatrix);
	gui_destroy(gGui);
	skillbar_destroy(gSkillBar);
#ifdef DEBUG
	pointer_destroy(gPointer);
	sprite_destroy(fpsSprite);
#endif // DEBUG
	actiontextbuilder_close();
	item_builder_close();
	particle_engine_close();
	timer_destroy(menuTimer);
	mixer_close();
	texturecache_close();
	settings_close();
	hiscore_close();

#ifdef STEAM_BUILD
	steam_shutdown();
#endif // STEAM_BUILD

	if (gController)
		SDL_GameControllerClose(gController);
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

	if (settings_get()->fullscreen_enabled) {
		// Game starts in windowed mode so this will 
		// change to fullscreen
		toggle_fullscreen();
	}
	run();
	close();
	PHYSFS_deinit();

	return 0;
}
