#ifndef GAME_H_
# define GAME_H_

# include <stdio.h>
# include <stdlib.h>
# include <assert.h>
# include <string.h>
# include "raylib.h"
# include "raymath.h"
# include "input.h"
# include "ui.h"
# include "types.h"

# define TILE_SIZE 16

typedef enum {MAIN_MENU, TETRIS, SNAKE_GAME, PONG, BREAKOUT, TEST, GAME_COUNT} Games_e;
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
	NOTHING, // When nothing was done
} UiStates;

typedef struct ColorPalette {
	Color	black;
	Color	white;
	Color	red;
	Color	pink;
	Color	blue;
	Color	yellow;
	Color	green;
	Color	orange;
	Color	purple;
	Color	background;
} ColorPalette;

#define MAX_ASSET 5

typedef struct Assets {
	FontConfig	fonts[MAX_ASSET];
	Sound		sounds[MAX_ASSET];
	Music		music[MAX_ASSET];
	Texture2D	textures[MAX_ASSET];
} Assets;

typedef struct {
	Vector2		window_size; // TODO  rename, canvas?
	float		effects_vol;
	float		music_vol;
	bool		quit;
	ColorPalette	palette;
	Games_e		current_game;
	Assets		assets;
	UiConfig	ui_config;
} GameData;

typedef struct GameFunctions
{
	char	*name;
	void	(*start)(void);
	void	(*update)(void);
	void	(*draw)(void);
	void	(*de_init)(void);
} GameFunctions;

GameFunctions	snake_game_init(GameData *data);
GameFunctions	tetris_init(GameData *data);
GameFunctions	main_menu_init(GameData *data);
GameFunctions	test_game_init(GameData *data);
GameFunctions	pong_init(GameData *data);
GameFunctions	breakout_init(GameData *data);

void	update_volume(GameData *data);
void	ui_trasition_from(V2 dir);
UiStates	game_over_screen(GameData *data);
UiStates	options_screen(GameData *data);
void	pause_game(); // Only called from js on the web version

void	draw_grid_ex(V2 position, V2 grid_size, int tile_size, float line_thickness, Color color);
void	draw_grid(V2 position, V2 grid_size, int tile_size);
V2	ExpDecayV2(V2 a, V2 b, float decay);
Rect	RectV2(V2 pos, V2 size);
V2	V2RectPos(Rect rect);
V2	V2RectSize(Rect rect);
bool	ShouldGameRun(bool *play_screen, bool *paused, bool *game_over);
#endif // GAME_H_
