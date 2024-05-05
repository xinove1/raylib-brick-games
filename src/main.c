#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#define XI_INPUT_ACTIONS_IMPLEMENTATION

#include "game.h"
#include "style_candy.h"


typedef enum {TETRIS, SNAKE_GAME, GAME_COUNT} GAMES;

int	main()
{
	GameData	data = {0};
	GameFunctions	games_lst[GAME_COUNT] = {0};
	GameFunctions	game;

	data.window_size = (V2) {800, 600};
	data.music_vol = 1.0f;
	data.effects_vol = 1.0f;

	games_lst[SNAKE_GAME] = snake_game_init(&data);

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

	game = games_lst[SNAKE_GAME];
	game.start();
	while (!WindowShouldClose() && !data.quit) {
		game.update();
	}

	CloseWindow();
	CloseAudioDevice();
	return (0);
}


