/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2025  Linus Probert <linus.probert@gmail.com>
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
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_main.h>
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
#include "particle_engine.h"
#include "menu.h"
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
#include "tooltip_manager.h"
#include "gamecontroller.h"
#include "time.h"
#include "sprite_util.h"
#include "event.h"
#include "config.h"
#include "save.h"

#ifdef STEAM_BUILD
#include "checksum.h"
#include "steam/steamworks_api_wrapper.h"
#endif // STEAM_BUILD

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
static Menu		*gameSelectMenu		= NULL;
static Menu		*inGameMenu		= NULL;
static Menu		*charSelectMenu		= NULL;
static Timer		*menuTimer		= NULL;
static Camera		*gCamera		= NULL;
static Screen		*creditsScreen		= NULL;
static Screen		*scoreScreen		= NULL;
static Screen		*characterSelectScreen	= NULL;
static unsigned int	cLevel			= 1;
static float		deltaTime		= 1.0;
static Turn		currentTurn		= PLAYER;
static class_t		playerClass		= WARRIOR;
static bool		quickGame		= false;
static bool		arcadeGame		= false;
static bool		weeklyGame		= false;
static GameState	gGameState;
static SDL_Rect		mainViewport;
static SDL_Rect		gameViewport;
static SDL_Rect		skillBarViewport;
static SDL_Rect		bottomGuiViewport;
static SDL_Rect		statsGuiViewport;
static SDL_Rect		minimapViewport;
static SDL_Rect		menuViewport;
static Input		input;

#ifdef DEBUG
static Sprite	*fpsSprite	= NULL;
static Pointer	*gPointer	= NULL;
#endif // DEBUG

static void resetGame(void);
static void initMainMenu(void);
static bool is_player_dead(void);
static void initGamepads(void);

static SDL_Surface *window_icon = NULL;

static
bool initSDL(void)
{
	debug("Initializing SDL");
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC))
	{
		error("Could not initiate SDL3: %s", SDL_GetError());
		return false;
	}

	Dimension dim = getScreenDimensions();

	debug("Initializing SDL_ttf");
	if (!TTF_Init()) {
		error("Unable to initiate ttf library: %s",
		       SDL_GetError());
		return false;
	}

	debug("Initializing gamepads");
	initGamepads();

	debug("Initializing SDL_mixer");
	mixer_init();

	char buffer[100] = { '\0' };
	m_sprintf(buffer, 100, "%s %d.%d.%d %s", GAME_TITLE, MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, RELEASE_TYPE);
	gWindow = SDL_CreateWindow(buffer,
				   SCREEN_WIDTH,
				   SCREEN_HEIGHT, 0);
	if (gWindow == NULL)
	{
		error("Unable to create window: %s", SDL_GetError());
		return false;
	}

	// Set the window icon
	window_icon = IMG_Load_IO(io_load_rwops("Extras/icon.png"), true);
	SDL_SetWindowIcon(gWindow, window_icon);

	gRenderer = SDL_CreateRenderer(gWindow, NULL);
	if (gRenderer == NULL)
	{
		error("Unable to create renderer: %s", SDL_GetError());
		return false;
	}
	if (!SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND)) {
		error("Unable to set blend mode: %s", SDL_GetError());
		return false;
	}
	if (!SDL_SetRenderLogicalPresentation(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX))
	{
		error("Unable to initiate scaling: %s",
		       SDL_GetError());
		return false;
	}

	if (SDL_TextInputActive(gWindow)) {
		debug("Disabling text input");
		SDL_StopTextInput(gWindow);
	}

	memset(buffer, '\0', 100);
	sprintf(buffer, "%d.%d.%d", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
	SDL_SetAppMetadata(GAME_TITLE, buffer, "com.oliveshark.breakhack");

	return true;
}

static void
initViewports(Uint32 offset)
{
	/* FIXME: This does not work with
	 * SDL_SetRenderLogicalPresentation any longer. Needs looking at. */
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
	sprite_util_init(gRenderer);
	gCamera = camera_create(gRenderer);
	gRoomMatrix = roommatrix_create(gRenderer);
	gGui = gui_create(gCamera);
	skillbar_set_controller_mode(gamecontroller_mode());
	gSkillBar = skillbar_create(gCamera);
	item_builder_init(gRenderer);
#ifdef DEBUG
	gPointer = pointer_create(gRenderer);
#endif // DEBUG
	particle_engine_init();
	menuTimer = _timer_create();
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
	if (arcadeGame)
		cLevel = 19;
	else
		cLevel = 1;

	if (weeklyGame)
		set_random_seed((unsigned int) time_get_weekly_seed());
	else
		set_random_seed(0);

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
	if (!settings->howto_tooltip_shown) {
		gGui->activeTooltip = tooltip_manager_get_tooltip(TOOLTIP_TYPE_HOWTO);
	}
	settings->howto_tooltip_shown = true;

	if (arcadeGame)
		player_set_level(gPlayer, 22);
}

static void
exitGame(void *unused)
{
	UNUSED(unused);
	gGameState = QUIT;
}

static bool
gameCompleted(void)
{
	return cLevel >= 20 || (quickGame && cLevel >= 12);
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
	else if (gameCompleted())
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
	else if (strcmp(str, "mage") == 0)
		playerClass = MAGE;

	startGame();
}

static void
goToCharacterMenu(void *unused)
{
	UNUSED(unused);
	charSelectMenu = menu_create_character_selector(on_character_select, gCamera);
	characterSelectScreen = screen_create_characterselect(gRenderer);
	gGameState = CHARACTER_MENU;
}

static void
choose_music(void)
{
	if (cLevel > (unsigned int) (quickGame ? 11 : 19)) {
		mixer_play_music(BOSS_MUSIC0);
	} else if (cLevel % (quickGame ? 3 : 5) == 0) {
		gui_log("You sense something powerful in the vicinity");
		mixer_play_music(BOSS_MUSIC0);
	} else {
		mixer_play_music(GAME_MUSIC0 + get_random(2));
	}
}

static void
continueGame(void *unused)
{
	(void) unused;
	const Save *save = save_get();
	quickGame = save->quickGame;
	arcadeGame = save->arcadeGame;

	playerClass = save->player_class;
	cLevel = save->map_level;
	set_random_seed(save->seed);
	debug("Loading seed: %d", save->seed);
	debug("Loading map level: %d", save->map_level);

	gGameState = PLAYING;
	if (gPlayer)
		player_destroy(gPlayer);
	gPlayer = player_create(playerClass, gCamera);

	// Load player from save
	gPlayer->daggers = save->player_daggers;
	gPlayer->xp = save->player_xp;
	gPlayer->stateData = save->player_state;
	gPlayer->stats = save->player_stats;
	gPlayer->stat_data = save->player_player_stats;
	gPlayer->potion_sips = save->player_potion_sips;
	gPlayer->equipment = save->player_equipment;
	gPlayer->gold = save->player_gold;

	choose_music();
	resetGame();
	skillbar_reset(gSkillBar);
	gui_clear_message_log();
	gui_log("The Dungeon Crawl continues!");
	gui_event_message("Welcome back!");
}

static void
startRegularGame(void *unused)
{
	quickGame = false;
	weeklyGame = false;
	goToCharacterMenu(unused);
}

static void
startQuickGame(void *unused)
{
	quickGame = true;
	weeklyGame = false;
	goToCharacterMenu(unused);
}

#ifdef STEAM_BUILD
static void
startWeeklyGame(void *unused)
{
	quickGame = true;
	weeklyGame = true;
	goToCharacterMenu(unused);
}
#endif

static void
startArcadeGame(void *unused)
{
	arcadeGame = true;
	quickGame = false;
	weeklyGame = false;
	goToCharacterMenu(unused);
}

static void
goToMainMenu(void *unused)
{
	UNUSED(unused);
	save_load();
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
goToGameSelectMenu(void *unused)
{
	UNUSED(unused);
	int item_count = 3;
#ifdef STEAM_BUILD
	item_count += 1;
#endif
	if (save_exists()) {
		item_count += 1;
	}
	TEXT_MENU_ITEM *menuItems = ec_malloc(item_count * sizeof(TEXT_MENU_ITEM));
	int i = 0;
	if (save_exists()) {
		menuItems[i++] = (TEXT_MENU_ITEM) {
			"CONTINUE",
			"Continue your last session",
			continueGame,
		};
	}
	menuItems[i++] = (TEXT_MENU_ITEM) {
		"STANDARD GAME",
		"Standard 20 level game, recommended for new players",
		startRegularGame
	};
#ifdef STEAM_BUILD
	menuItems[i++] = (TEXT_MENU_ITEM) {
		"WEEKLY CHALLENGE",
		"Quick game with weekly leaderboards at breakhack.net",
		startWeeklyGame
	};
#endif
	menuItems[i++] = (TEXT_MENU_ITEM) {
		"QUICK GAME",
		"Shorter 12 level game, with more action earlier in the game",
		startQuickGame
	};
	menuItems[i++] = (TEXT_MENU_ITEM) {
		"ARCADE GAME",
		"One big level with lots of action",
		startArcadeGame
	};

	menu_create_text_menu(&gameSelectMenu, menuItems, item_count, gRenderer);
	free(menuItems);
	gGameState = GAME_SELECT;
}

static void
showHowToTooltip(void *unused)
{
	UNUSED(unused);
	toggleInGameMenu(NULL);
	gGui->activeTooltip = tooltip_manager_get_tooltip(TOOLTIP_TYPE_HOWTO);
}

static void
initInGameMenu(void)
{
	static TEXT_MENU_ITEM menu_items[] = {
		{ "RESUME", "", toggleInGameMenu },
		{ "HOW TO PLAY", "", showHowToTooltip },
		{ "MAIN MENU", "", goToMainMenu },
		{ "QUIT", "Exit game", exitGame },
	};

	menu_create_text_menu(&inGameMenu, &menu_items[0], 4, gRenderer);
}

static void
createInGameGameOverMenu(void)
{
	static TEXT_MENU_ITEM menu_items[] = {
		{ "NEW GAME", "Start a new game with the same settings",
			goToCharacterMenu },
		{ "MAIN MENU", "", goToMainMenu },
		{ "QUIT", "Exit game", exitGame },
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
initGamepads(void)
{
	int num_joysticks;
	SDL_JoystickID *sticks = SDL_GetJoysticks(&num_joysticks);
	debug("Found %d joysticks", num_joysticks);
	for (int i = 0; i < num_joysticks; i++) {
		int stick = sticks[i];
		if (!SDL_IsGamepad(stick)) {
			continue;
		}

		SDL_Gamepad *ctrler = SDL_OpenGamepad(stick);
		if (ctrler) {
			gamecontroller_set(ctrler);
		}
	}
	SDL_free(sticks);
}

static void
initMainMenu(void)
{
	static TEXT_MENU_ITEM menu_items[] = {
		{ "PLAY", "Start game", goToGameSelectMenu },
		{ "SCORES", "View your top 10 scores", viewScoreScreen },
		{ "CREDITS", "View game credits", viewCredits },
		{ "QUIT", "Exit game", exitGame },
	};

	if (gMap)
		map_destroy(gMap);

	gMap = map_lua_generator_single_room__run(cLevel, gRenderer);

	menu_create_text_menu(&mainMenu, &menu_items[0], 4, gRenderer);
	mixer_play_music(MENU_MUSIC);
	creditsScreen = screen_create_credits(gRenderer);
	scoreScreen = screen_create_hiscore(gRenderer);
	quickGame = false;
	weeklyGame = false;
	arcadeGame = false;
}

static void
repopulate_roommatrix(void)
{
	roommatrix_populate_from_map(gRoomMatrix, gMap);
	roommatrix_add_lightsource(gRoomMatrix,
		&gPlayer->sprite->pos);
	roommatrix_build_lightmap(gRoomMatrix, gCamera);
}

static void
resetGame(void)
{
	SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);

	if (mainMenu) {
		menu_destroy(mainMenu);
		mainMenu = NULL;
	}
	if (charSelectMenu) {
		menu_destroy(charSelectMenu);
		charSelectMenu = NULL;
	}
	if (gameSelectMenu) {
		menu_destroy(gameSelectMenu);
		gameSelectMenu = NULL;
	}
	if (characterSelectScreen) {
		screen_destroy(characterSelectScreen);
		characterSelectScreen = NULL;
	}
	if (creditsScreen) {
		screen_destroy(creditsScreen);
		creditsScreen = NULL;
	}
	if (scoreScreen) {
		screen_destroy(scoreScreen);
		scoreScreen = NULL;
	}
	if (inGameMenu) {
		menu_destroy(inGameMenu);
		inGameMenu = NULL;
	}

	initInGameMenu();

	if (gMap)
		map_destroy(gMap);

	particle_engine_clear();

	GameMode mode = REGULAR;
	if (quickGame)
		mode = QUICK;
	else if (arcadeGame)
		mode = ARCADE;

	info("Building new map");
	gMap = map_lua_generator_run(cLevel, mode, gPlayer, gRenderer);

	player_reset_on_levelchange(gPlayer);

	map_set_current_room(gMap, &gPlayer->sprite->pos);
	camera_follow_position(gCamera, &gPlayer->sprite->pos);
	repopulate_roommatrix();
}

static void
on_event_callback(Event *event)
{
	if (event->type == MONSTER_KILLED_EVENT) {
		if (strcmp(event->monsterKilled.monster->label, "A Fairy") == 0) {
			LinkedList *monsters = gMap->monsters;
			while (monsters) {
				Monster *monster = monsters->data;
				monsters = monsters->next;
				if (position_in_room(&monster->sprite->pos, &gMap->currentRoom))
					monster_set_bloodlust(monster, true);
			}
		}
	}
}

static bool
init(void)
{
#ifdef STEAM_BUILD
#ifndef DEBUG
	if (!steam_restart_needed()) {
		steam_init();
	} else {
		error("%s needs to be started through Steam", GAME_TITLE);
		return false;
	}
#else
	steam_init();
#endif
#endif // STEAM_BUILD

	if (!initSDL()) {
		return false;
	} else if (!initGame()) {
		return false;
	}

	event_register_listener(on_event_callback);

	settings_init();
	save_init();
	hiscore_init();
	initMainMenu();
	tooltip_manager_init(gCamera);

	gCamera->pos = (Position) { 0, 0 };

	gGameState = MENU;

	return true;
}

static void
toggle_fullscreen(void)
{
	SDL_WindowFlags flags = SDL_GetWindowFlags(gWindow) & SDL_WINDOW_FULLSCREEN;
	Settings *settings = settings_get();
	bool fullscreen = flags == SDL_WINDOW_FULLSCREEN;
	SDL_SetWindowFullscreen(gWindow, !fullscreen);
	settings->fullscreen_enabled = !fullscreen;
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
				gGameState = MENU;
				break;
			case GAME_SELECT:
				menu_destroy(gameSelectMenu);
				gameSelectMenu = NULL;
				gGameState = MENU;
				break;
			case CHARACTER_MENU:
				if (mainMenu == NULL)
					break;

				screen_destroy(characterSelectScreen);
				characterSelectScreen = NULL;
				menu_destroy(charSelectMenu);
				charSelectMenu = NULL;
				gGameState = GAME_SELECT;
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
	static InputDeviceType last_device_type = DeviceType_Unknown;
	static SDL_Event event;

	InputDeviceType device_type = DeviceType_Unknown;
	bool quit = false;


	input_reset(&input);
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_EVENT_QUIT) {
			quit = true;
			continue;
		}

		if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
			initGamepads();
		} else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
			debug("Gamepad removed");
			gamecontroller_set(NULL);
			skillbar_set_controller_mode(GAMEPAD_TYPE_NONE);
			tooltip_manager_set_controller_mode(GAMEPAD_TYPE_NONE);
		}

		input_handle_event(&input, &event, &device_type);

		if (device_type != DeviceType_Unknown && device_type != last_device_type) {
			debug("Device type changed: %d", last_device_type);
			last_device_type = device_type;
			if (device_type == DeviceType_Gamepad) {
				skillbar_set_controller_mode(gamecontroller_mode());
				tooltip_manager_set_controller_mode(gamecontroller_mode());
			} else {
				skillbar_set_controller_mode(GAMEPAD_TYPE_NONE);
				tooltip_manager_set_controller_mode(GAMEPAD_TYPE_NONE);
			}
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
	if (gameCompleted()) {
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
		++cLevel;
		if (!weeklyGame) {
			save_save(get_random_seed(),
				  cLevel,
				  quickGame,
				  arcadeGame,
				  gPlayer);
		}

		mixer_play_effect(NEXT_LEVEL);
		choose_music();
		if (!gameCompleted()) {
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
	if (settings->tooltips_enabled) {
		if (skillActivated) {
			gGui->activeTooltip = tooltip_manager_get_tooltip(TOOLTIP_TYPE_SKILL);
		}
		if (!artifactTooltipShown && gPlayer->equipment.hasArtifacts) {
			artifactTooltipShown = true;
			gGui->activeTooltip = tooltip_manager_get_tooltip(TOOLTIP_TYPE_ARTIFACT);
		}
	}
}

static void
run_game_update(void)
{
	static UpdateData updateData;

	if (gGameState == IN_GAME_MENU)
		menu_update(inGameMenu, &input, gCamera);

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
	SDL_SetRenderViewport(gRenderer, &statsGuiViewport);
	gui_render_panel(gGui, gCamera);
	SDL_SetRenderViewport(gRenderer, &minimapViewport);
	gui_render_minimap(gGui, gMap, gCamera);
	SDL_SetRenderViewport(gRenderer, &skillBarViewport);
	skillbar_render(gSkillBar, gPlayer, gCamera);
	SDL_SetRenderViewport(gRenderer, &bottomGuiViewport);
	gui_render_log(gGui, gCamera);
	SDL_SetRenderViewport(gRenderer, &mainViewport);
}

static void
render_game_completed(void)
{
	SDL_SetRenderViewport(gRenderer, &gameViewport);
	if (!is_player_dead()) {
		player_render(gPlayer, gCamera);
		player_render_toplayer(gPlayer, gCamera);
	}
	actiontextbuilder_render(gCamera);
	gui_render_event_message(gGui, gCamera);

	if (gGameState == IN_GAME_MENU) {
		SDL_FRect dimmer = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
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
	SDL_SetRenderViewport(gRenderer, &gameViewport);
	map_render(gMap, gCamera);
	particle_engine_render_game(gCamera);

	map_render_mid_layer(gMap, gCamera);

	if (!is_player_dead()) {
		player_render(gPlayer, gCamera);
		player_render_toplayer(gPlayer, gCamera);
	}

	map_render_top_layer(gMap, gRoomMatrix, gCamera);

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

	SDL_SetRenderViewport(gRenderer, &mainViewport);
	particle_engine_render_global(gCamera);
	gui_render_tooltip(gGui, gCamera);

	if (gGameState == IN_GAME_MENU) {
		SDL_FRect dimmer = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
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

#ifdef STEAM_BUILD
static inline void
register_scores(void)
{
		uint8_t details[4] = {
			(uint8_t) gPlayer->stats.lvl,
			(uint8_t) cLevel, (uint8_t) (gPlayer->class + 1), 0
		};
		steam_register_score((int) gPlayer->gold, (int32_t*)
				     &details, 1);
		steam_register_kills((int) gPlayer->stat_data.kills,
				     (int32_t*) &details, 1);
		if (quickGame) {
			steam_register_qp_score((int) gPlayer->gold,
						(int32_t*) &details, 1);
		}
		if (weeklyGame) {
			steam_register_weekly_score((int) gPlayer->gold,
						    (int32_t*) &details, 1);
		}
		if (arcadeGame) {
			steam_register_arcade_score((int)gPlayer->gold,
						    (int32_t*) &details, 1);
		}
		if (gPlayer->class == ROGUE) {
			steam_set_achievement(ROGUE_LIKE);
			steam_register_rogue_score((int) gPlayer->gold,
						   (int32_t*) &details, 1);
		} else if (gPlayer->class == WARRIOR) {
			steam_register_warrior_score((int) gPlayer->gold,
						     (int32_t*) &details, 1);
		} else if (gPlayer->class == MAGE) {
			steam_set_achievement(MAGICAL);
			steam_register_mage_score((int) gPlayer->gold,
						  (int32_t*) &details, 1);
		}
}
#endif

static void
run_game(void)
{
	run_game_update();

	if (gameCompleted()) {
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
		save_clear();
		end_game_details();
		mixer_play_effect(SPLAT);
		gGameState = GAME_OVER;
		createInGameGameOverMenu();
		hiscore_register(gPlayer, cLevel);
#ifdef STEAM_BUILD
		register_scores();
#endif // STEAM_BUILD

	} else {
		check_next_level();
	}

	if (gGameState == PLAYING && (gameCompleted())) {
		gGameState = COMPLETED;
		createInGameGameOverMenu();
		gui_event_message("Your break is over!");
		gui_log("Your break is over!");
		gui_event_message("Well done!");
		end_game_details();
#ifdef STEAM_BUILD
		if (cLevel >= 20 && !arcadeGame)
			steam_set_achievement(BACK_TO_WORK);
		register_scores();
#endif // STEAM_BUILD
	}
}

static Menu *
get_active_menu(void)
{
	switch (gGameState) {
		case MENU:
			return mainMenu;
		case CHARACTER_MENU:
			return charSelectMenu;
		case GAME_SELECT:
			return gameSelectMenu;
		default:
			return NULL;
	}
}

static inline void
render_current_screen(void)
{
	switch (gGameState) {
		case CHARACTER_MENU:
			screen_render(characterSelectScreen, gCamera);
			break;
		case CREDITS:
			screen_render(creditsScreen, gCamera);
			break;
		case SCORE_SCREEN:
			screen_render(scoreScreen, gCamera);
			break;
		default:
			break;
	}
}

static void
run_menu(void)
{
	if (!timer_started(menuTimer))
		timer_start(menuTimer);

	roommatrix_populate_from_map(gRoomMatrix, gMap);
	roommatrix_build_lightmap(gRoomMatrix, gCamera);
	if (timer_get_ticks(menuTimer) > 1000) {
		timer_stop(menuTimer);
		timer_start(menuTimer);
		map_move_monsters(gMap, gRoomMatrix);
	}

	if (gGameState != MENU
	    && gGameState != CREDITS
	    && gGameState != SCORE_SCREEN
	    && gGameState != GAME_SELECT
	    && gGameState != CHARACTER_MENU)
		return;


	menu_update(get_active_menu(), &input, gCamera);

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
	SDL_RenderClear(gRenderer);
	if (gGameState != CHARACTER_MENU)  {
		SDL_SetRenderViewport(gRenderer, &menuViewport);
		map_render(gMap, gCamera);
		map_render_mid_layer(gMap, gCamera);
		map_render_top_layer(gMap, gRoomMatrix, gCamera);
		roommatrix_render_lightmap(gRoomMatrix, gCamera);
	}

	SDL_SetRenderViewport(gRenderer, &mainViewport);

	render_current_screen();
	menu_render(get_active_menu(), gCamera);

#ifdef DEBUG
	sprite_render(fpsSprite, gCamera);
	pointer_render(gPointer, gCamera);
#endif // DEBUG

	SDL_RenderPresent(gRenderer);
}

static void
run(void)
{
	static Uint64 oldTime = 0;
	static Uint64 currentTime = 0;

	bool quit = false;

#ifdef DEBUG
	Uint32 frame = 0;
	Timer *fpsTime = _timer_create();
	Timer *updateTimer = _timer_create();
	timer_start(fpsTime);
	timer_start(updateTimer);
#endif // DEBUG

	Timer *fpsTimer = _timer_create();

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
			case GAME_SELECT:
			case CHARACTER_MENU:
				run_menu();
				break;
			case QUIT:
				quit = true;
				break;
			default:
				break;
		}

		Uint32 ticks = (Uint32) timer_get_ticks(fpsTimer);
		if (ticks < 1000/60)
			SDL_Delay((1000/60) - ticks);
		timer_stop(fpsTimer);

		if (currentTime == 0)
			currentTime = SDL_GetTicks();
		else {
			oldTime = currentTime;
			currentTime = SDL_GetTicks();
			deltaTime = ((float)(currentTime - oldTime) / 1000.0f);
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
	if (charSelectMenu)
		menu_destroy(charSelectMenu);
	if (gameSelectMenu)
		menu_destroy(gameSelectMenu);
	if (inGameMenu)
		menu_destroy(inGameMenu);
	if (creditsScreen)
		screen_destroy(creditsScreen);
	if (scoreScreen)
		screen_destroy(scoreScreen);
	if (characterSelectScreen)
		screen_destroy(characterSelectScreen);
	if (window_icon)
		SDL_DestroySurface(window_icon);

	tooltip_manager_close();
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
	save_close();

#ifdef STEAM_BUILD
	steam_shutdown();
#endif // STEAM_BUILD

	gamecontroller_close();
	event_clear_listeners();
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	TTF_Quit();
	SDL_Quit();
}

#ifdef STEAM_BUILD
static void
validate_lib_checksum(void)
{
	FILE *fp;
#ifdef WIN32
	const char *file = "./steam_api.dll";
	unsigned int expected = DLL_LIBSTEAM_CHECKSUM;
	fopen_s(&fp, file, "rb");
#else // WIN32
	const char *file = "./libsteam_api.so";
	unsigned int expected = SO_LIBSTEAM_CHECKSUM;
	fp = fopen(file, "rb");
#endif // WIN32

	if (!fp) {
		fatal("Unable to open %s for reading\n", file);
	}

	unsigned calculated = checksum_fp(fp);	
	fclose(fp);

	if (calculated != expected) {
		fatal("Checksum validation failiure: %#x != %#x", calculated, expected);
	} else {
		info("Checksum validated: %#x", calculated);
	}
}
#endif // STEAM_BUILD

int main(int argc, char *argv[])
{
	UNUSED(argc);

#ifdef STEAM_BUILD
	validate_lib_checksum();
#endif // STEAM_BUILD

	PHYSFS_init(argv[0]);
#ifndef DEBUG
	PHYSFS_mount("assets.pack", NULL, 0);
	PHYSFS_mount("data.pack", NULL, 0);
#else
	PHYSFS_mount("assets", NULL, 0);
	PHYSFS_mount("data", NULL, 0);
#endif // DEBUG

	if (argc > 1) {
		set_random_seed(atoi(argv[1]));
	}

	if (!init())
		return -1;

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
