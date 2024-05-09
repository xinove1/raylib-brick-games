#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#define RAYGUI_IMPLEMENTATION
#define XI_INPUT_ACTIONS_IMPLEMENTATION

#include "game.h"
#include "style_candy.h"

void	_Testfunc(char *str, GameData data) 
{
	printf("str: %s, data- quit: %d, music_vol: %f\n", str, data.quit, data.music_vol);
}
#define Testfunc(str, ...) _Testfunc((str), (GameData){.quit = false, __VA_ARGS__})

static GameFunctions	games[GAME_COUNT] = {0};
static Games_e	game;

int	main()
{
	GameData	data = {0};

	data.window_size = (V2) {800, 600};
	data.music_vol = 1.0f;
	data.effects_vol = 1.0f;
	data.palette = (ColorPalette) {
		.black = BLACK,
		.white = RAYWHITE,
		.red = RED,
		.blue = BLUE,
		.yellow = YELLOW,
		.green = GREEN,
		.orange = ORANGE,
		.purple = PURPLE,
		.background = BEIGE,
	};
	data.current_game = MAIN_MENU;
	game = -1;

	// Enable config flags for resizable window and vertical synchro
	//SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
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
	register_input_action("open_menu", KEY_ESCAPE);
	register_input_action("open_menu", KEY_E);

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
		games[game].update();
	}

	games[SNAKE_GAME].de_init();
	games[TETRIS].de_init();
	games[MAIN_MENU].de_init();

	CloseWindow();
	CloseAudioDevice();
	return (0);
}

// TODO change raylib window flags to window unfocused
void	pause_game() 
{
	printf("Pause game called\n");
}
