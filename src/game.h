#ifndef GAME_H_
# define GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include "input.h"


typedef Vector2 V2 ;
typedef Rectangle Rect;

typedef enum {MAIN_MENU, PLAY_MENU, OPTIONS_MENU, GAME} States;

typedef struct ColorPalette {
	Color	black;
	Color	white;
	Color	red;
	Color	blue;
	Color	yellow;
	Color	green;
	Color	orange;
	Color	purple;
} ColorPalette;

typedef struct {
	Vector2	window_size;
	float	effects_vol;
	float	music_vol;
	bool	quit;
	States	state; // State 
	States	previous_state; // State 
	ColorPalette	palette;
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

#endif // GAME_H_
