#include "game.h"

static GameData	*Data;

static void	start()
{

}

static void	update()
{

}

static void	draw()
{
	V2	window = Data->window_size;
	V2	center = {window.x / 2, window.y / 2};

	draw_grid(center, (V2){1,1} , TILE_SIZE);
	DrawPixel(center.x, center.y, RED);
	DrawPixel(center.x + TILE_SIZE, center.y, RED);
	DrawPixel(center.x, center.y + TILE_SIZE, RED);
	DrawPixel(center.x + TILE_SIZE, center.y + TILE_SIZE, RED);
}

static void	de_init()
{
}

GameFunctions	test_game_init(GameData *data)
{
	Data = data;

	return (GameFunctions) {
		.name = "Test game",
		.start = &start,
		.update = &update,
		.draw = &draw,
		.de_init = &de_init,
	};
}
