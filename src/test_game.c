#include "game.h"
#include "raylib.h"

static GameData	*Data;
static bool Flag = false;

static void	start()
{

}

static void	update()
{

	if (IsActionPressed(ACTION_2)) {
		Data->current_ui = TITLE_SCREEN;
	}
	if (IsActionPressed(ACTION_1)) {
		if (Flag) {
			Flag =  false;
		} else {
			Flag =  true;
		}

	}

}

static void	draw()
{
	V2	window = Data->window_size;
	V2	center = {window.x / 2, window.y / 2};

	V2	mouse = GetMousePosition();

	if (Flag) {
		printf("center: %f, %f \n", center.x, center.y);
		DrawPixel(center.x, center.y, RED);
		DrawPixel(center.x + TILE_SIZE -1, center.y, RED);
		DrawPixel(center.x, center.y + TILE_SIZE, RED);
		DrawPixel(center.x + TILE_SIZE -1, center.y + TILE_SIZE, RED);
	}
	static V2	dir = {0, 0};
	if (IsActionPressed(RIGHT)) {
		dir.x += 1;
	}
	if (IsActionPressed(LEFT)) {
		dir.x += -1;
	}
	if (IsActionPressed(DOWN)) {
		dir.y += 1;
	}
	if (IsActionPressed(UP)) {
		dir.y += -1;
	}
	//draw_grid_ex(center, (V2){1,1} , TILE_SIZE, 2, ColorAlpha(GREEN, 0.2));
	draw_grid_ex(center, (V2){4,4} , TILE_SIZE, 1, ColorAlpha(BLACK, 0.2));
	// DrawRectangle(center.x, center.y, TILE_SIZE, TILE_SIZE, ColorAlpha(RED, 0.2));
	
	DrawRectangle(center.x + (dir.x * TILE_SIZE), center.y + (dir.y * TILE_SIZE), TILE_SIZE, TILE_SIZE, ColorAlpha(BLUE, 0.2));

	DrawText(TextFormat("%d, %d", (int)mouse.x, (int)mouse.y), mouse.x, mouse.y - 5, 1, BLACK);
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
