#ifndef GAME_H_
# define GAME_H_

# include <stdio.h>
# include <stdlib.h>
# include "raylib.h"
# include "raymath_short.h"
# include "ui.h"
# include "input.h"
# include "types.h"

# define TILE_SIZE 16
# define SCORES_SAVE_LOCATION "./data"

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
	Color black;
	Color white;
	Color red;
	Color pink;
	Color blue;
	Color yellow;
	Color green;
	Color orange;
	Color purple;
	Color background;
} ColorPalette;

#define MAX_ASSET 5

typedef struct Assets {
	FontConfig fonts[MAX_ASSET];
	Sound      sounds[MAX_ASSET];
	Music      music[MAX_ASSET];
	Texture2D  textures[MAX_ASSET];
} Assets;

typedef struct SnakeData SnakeData;
typedef struct TetrisData TetrisData;

typedef struct 
{
	f32 snake[8];
} HighScores;

typedef struct {
	V2           window_size;
	f32          effects_vol;
	f32          music_vol;
	b32          quit;
	ColorPalette palette;
	Games_e      current_game;
	Assets       assets;
	HighScores   scores;
	UiConfig     ui_config;
	SnakeData    *snake_data;
	TetrisData   *tetris_data;
} GameData;

typedef struct GameFunctions
{
	byte *name;
	void (*start)(void);
	void (*update)(void);
	void (*draw)(void);
	void (*de_init)(void);
} GameFunctions;

GameFunctions snake_game_init(GameData *data);
GameFunctions tetris_init(GameData *data);
GameFunctions main_menu_init(GameData *data);
GameFunctions test_game_init(GameData *data);
GameFunctions pong_init(GameData *data);
GameFunctions breakout_init(GameData *data);

UiStates game_over_screen(GameData *data);
UiStates options_screen(GameData *data);
void update_volume(GameData *data);
void ui_trasition_from(V2 dir);
void pause_game(); // Only called from js on the web version
b32  IsPauseRequested();

void draw_grid_ex(V2 position, V2 grid_size, i32 tile_size, f32 line_thickness, Color color);
void draw_grid(V2 position, V2 grid_size, i32 tile_size);
V2   ExpDecayV2(V2 a, V2 b, f32 decay);
b32  ShouldGameRun(b32 *play_screen, b32 *paused, b32 *game_over);
b32 SaveScores(cstr *where, HighScores scores);
HighScores LoadScores(cstr *where) ;
#endif // GAME_H_
