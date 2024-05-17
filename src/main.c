#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#define XI_INPUT_ACTIONS_IMPLEMENTATION
#include "game.h"
#include "style_candy.h"

void	_Testfunc(char *str, GameData data) 
{
	printf("str: %s, data- quit: %d, music_vol: %f\n", str, data.quit, data.music_vol);
}
#define Testfunc(str, ...) _Testfunc((str), (GameData){.quit = false, __VA_ARGS__})

static void	load_assets(GameData *data);
static void	unload_assets(GameData *data);

static GameFunctions	games[GAME_COUNT] = {0};
static Games_e	game;
static UiState	ui;
static UiState	prev_ui;

int	main()
{
	GameData	data = {0};

	data.window_size = (V2) {800, 600};
	data.music_vol = 1.0f;
	data.effects_vol = 1.0f;
	data.assets = (Assets) {0};
	data.palette = (ColorPalette) {
		.black = {0, 0, 0, 255},
		.white = {233, 228, 205, 255},
		.red   = {249, 39, 0, 255},
		.pink  = {211, 0, 92, 255},
		.blue  = {49, 21, 172, 255},
		.yellow= {224, 245, 0, 255},
		.green = {249, 111, 0, 255},
		.orange= {244, 93, 1, 255},
		.purple= {130, 5, 165, 255},
		.background = {237, 191, 198, 255},
	};
	data.current_game = MAIN_MENU;
	game = -1;
	data.current_ui = TITLE_SCREEN;
	ui = NONE;
	prev_ui = NONE;

	// Enable config flags for resizable window and vertical synchro
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
	InitWindow(data.window_size.x,data.window_size.y, "Raylib Bricks games");
	InitAudioDevice();
	SetTargetFPS(60);
	SetExitKey(0);

	register_input_action("right", KEY_D);
	register_input_action("right", KEY_RIGHT);
	register_input_action("left", KEY_A);
	register_input_action("left", KEY_LEFT);
	register_input_action("up", KEY_W);
	register_input_action("up", KEY_UP);
	register_input_action("down", KEY_S);
	register_input_action("down", KEY_DOWN);

	register_input_action("action_1", KEY_J);
	register_input_action("action_1", KEY_X);
	register_input_action("action_2", KEY_K);
	register_input_action("action_2", KEY_Z);
	register_input_action("space_bar", KEY_SPACE);
	register_input_action("open_menu", KEY_ESCAPE);
	register_input_action("open_menu", KEY_E);

	load_assets(&data);

	games[SNAKE_GAME] = snake_game_init(&data);
	games[TETRIS] = tetris_init(&data);
	games[MAIN_MENU] = main_menu_init(&data);

	while (!WindowShouldClose() && !data.quit) {
		if (data.current_game < 0 || data.current_game >= GAME_COUNT) {
			TraceLog(LOG_INFO, "main.c: Invalid option for current_game, won't change.\n");
		}
		else if (data.current_game != game) {
			printf("changed game\n");
			game = data.current_game;
			games[game].start();
		}

		if (data.current_ui != ui && data.current_ui != BACK) {
			prev_ui = ui;
			ui = data.current_ui;
			if (ui == TITLE_SCREEN) {
				ui_trasition_from((V2) {0, -1});
			}
		} else if (data.current_ui == BACK) {
			ui = prev_ui;
			data.current_ui = prev_ui;
			prev_ui = NONE;
		}

		if (ui == NONE) { // Only call update if none of the ui is active
			games[game].update();
		} else {
			games[MAIN_MENU].update();
		}

		// TODO Separeta games update from draw and call here
	}

	games[SNAKE_GAME].de_init();
	games[TETRIS].de_init();
	games[MAIN_MENU].de_init();

	unload_assets(&data);

	CloseWindow();
	CloseAudioDevice();
	return (0);
}

static void	load_assets(GameData *data) {
	data->assets.music[0] = LoadMusicStream("./assets/retro_comedy.ogg");
	data->assets.sounds[0] = LoadSound("./assets/upgrade4.ogg");
	data->assets.sounds[1] = LoadSound("./assets/gameover3.ogg");
	data->assets.fonts[0] = LoadFont("./assets/kenney_blocks.ttf");
	// data->assets.fonts[1] = LoadFontEx("./assets/PixeloidSans-Bold.ttf", 22, NULL, 0);
	// data->assets.fonts[2] = LoadFontEx("./assets/PixeloidSans-Bold.ttf", 42, NULL, 0);
	data->assets.fonts[1] = LoadFontEx("./assets/Kaph-Regular.ttf", 22, NULL, 0);
	data->assets.fonts[2] = LoadFontEx("./assets/Kaph-Regular.ttf", 42, NULL, 0);
}

static void	unload_assets(GameData *data) {
	for (int i = 0; i < MAX_ASSET; i++) {
		UnloadSound(data->assets.sounds[i]);
		UnloadMusicStream(data->assets.music[i]);
		UnloadFont(data->assets.fonts[i]);
	}
}

// TODO change raylib window flags to window unfocused
void	pause_game() 
{
	printf("Pause game called\n");
}
