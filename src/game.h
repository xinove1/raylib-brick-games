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

typedef struct {
	Vector2	window_size;
	float	effects_vol;
	float	music_vol;
	bool	quit;
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
