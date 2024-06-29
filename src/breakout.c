#include "game.h"

typedef struct {
	V2	pos;
	V2	size;
	V2	dir;
	float	speed;
} Obj;

static GameData	*Data = NULL;
static bool	GameOver = false;
static bool	GamePaused = false;
static bool	PlayScreen = true;

static Obj	Paddle = {};
static Obj	Ball = {};

static void	start()
{
	GameOver = false;
	GamePaused = false;
	PlayScreen = true;
	
	Paddle = (Obj) {
		.pos = (V2) { Data->window_size.x * 0.5f, Data->window_size.y - 10 },
		.size = (V2) { 25, 5 },
		.dir = (V2) { 0 },
		.speed = 10,
	};

	Ball = (Obj) {
		.pos = (V2) { Paddle.pos.x, Paddle.pos.y - 10},
		.size = (V2) { 10, 10 },
		.dir = (V2) { 0 },
		.speed = 10,
	};
}

static void	de_init()
{
}

static void	update()
{
	if (!PlayScreen && !GameOver && IsActionPressed(OPEN_MENU)) { 
		GamePaused = GamePaused  ? false : true;
	}

	if (GameOver || GamePaused || PlayScreen) {
		return ;
	}
}

static void	draw()
{

	// Ui Screens
	if (PlayScreen) {
		static UiPanel	panel = {.id_current = 0, .centralized = true};

		DrawRectangle(panel.pos.x, panel.pos.y, panel.width, panel.at_y - panel.pos.y, RED);
		V2	window = Data->window_size;
		V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
		panel.pos = center;

		panel_begin(&panel);
		{
			FontConfig	big = Data->assets.fonts[2];
			big.tint = YELLOW;
			FontConfig	small = Data->assets.fonts[1];

			panel_text(&panel, "BreakOut", big);

			if (panel_text_button(&panel, "Play", small)) { 
				PlayScreen = false;
			}

			// char	*mode = easy_mode ? "Mode: Easy" : "Mode: Normal";
			// if (panel_text_button(&panel, mode, small)) { 
			// 	easy_mode = easy_mode ? false : true;
			// }

			if (panel_text_button(&panel, "Back", small)) { 
				Data->current_game = MAIN_MENU;
			}
		}
		panel_end(&panel);

		if (IsActionPressed(ACTION_2)) {
			Data->current_game = MAIN_MENU;
		}
	}
	if (GameOver) {
		UiState	state = game_over_screen(Data);
		if (state == NONE) {
			GameOver = false;
			start();
		} else if (state == TITLE_SCREEN) {
			Data->current_game = MAIN_MENU;
			GameOver = false;
		}
	}
}

GameFunctions	breakout_init(GameData *data)
{
	Data = data;

	return (GameFunctions) { 
		.name = "BreakOut",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}
