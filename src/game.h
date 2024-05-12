#ifndef GAME_H_
# define GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "raylib.h"
//#include "stb_truetype.h"
#include "raymath.h"
#include "raygui.h"
#include "input.h"


typedef Vector2 V2 ;
typedef Rectangle Rect;

typedef enum {MAIN_MENU, TETRIS, SNAKE_GAME, GAME_COUNT} Games_e;
typedef enum {
	TITLE_SCREEN, 
	PLAY_MENU, // Game Selection
	OPTIONS_MENU,
	KEYBIND_MENU,
	PAUSED_MENU,
	COLORS_MENU,
	GAME_OVER_MENU,
	TUTORIAL_MENU,
	// Option state returned by menus funcs
	NONE,
	BACK,
} UiState;

typedef struct ColorPalette {
	Color	black;
	Color	white;
	Color	red;
	Color	blue;
	Color	yellow;
	Color	green;
	Color	orange;
	Color	purple;
	Color	background;
} ColorPalette;

#define MAX_ASSET 5

typedef struct Assets {
	Font	fonts[MAX_ASSET];
	Sound	sounds[MAX_ASSET];
	Music	music[MAX_ASSET];
} Assets;

typedef struct {
	Vector2	window_size;
	float	effects_vol;
	float	music_vol;
	bool	quit;
	ColorPalette	palette;
	Games_e	current_game;
	UiState	current_ui;
	Assets	assets;
} GameData;

typedef struct GameFunctions GameFunctions ;
struct GameFunctions
{
	char	*name;
	void	(*start)(void);
	void	(*update)(void);
	void	(*de_init)(void);
};

GameFunctions	snake_game_init(GameData *data);
GameFunctions	tetris_init(GameData *data);
GameFunctions	main_menu_init(GameData *data);

void	ui_trasition_from(V2 dir);
void	pause_game();
#endif // GAME_H_
