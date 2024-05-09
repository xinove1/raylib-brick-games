#ifndef GAME_H_
# define GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include "input.h"


typedef Vector2 V2 ;
typedef Rectangle Rect;

typedef enum {MAIN_MENU, TETRIS, SNAKE_GAME, GAME_COUNT} Games_e;

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

typedef struct {
	Vector2	window_size;
	float	effects_vol;
	float	music_vol;
	bool	quit;
	ColorPalette	palette;
	Games_e	current_game;
} GameData;

typedef struct GameFunctions GameFunctions ;
struct GameFunctions
{
	char	*name;
	//GameInterface	(*init)(GameData *data);
	void	(*start)(void);
	void	(*update)(void);
	void	(*de_init)(void);
};

GameFunctions	snake_game_init(GameData *data);
GameFunctions	tetris_init(GameData *data);
GameFunctions	main_menu_init(GameData *data);
int	game_over_screen(GameData *data); // Return 1 if play clicks to play again
void	draw_options_menu();
void	ui_init(GameData *data);
void	ui_deinit();

void	pause_game();
#endif // GAME_H_
