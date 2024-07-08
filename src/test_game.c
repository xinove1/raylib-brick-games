#include "game.h"
#include "raylib.h"

static GameData	*Data = NULL;
static bool	GameOver = false;
static bool	GamePaused = false;
static bool	PlayScreen = true;

static void	start()
{
	GameOver = false;
	GamePaused = false;
	PlayScreen = true;
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

			panel_text(&panel, "Template", big);

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
		UiStates	state = game_over_screen(Data);
		if (state == NONE) {
			GameOver = false;
			start();
		} else if (state == TITLE_SCREEN) {
			Data->current_game = MAIN_MENU;
			GameOver = false;
		}
	}
}

static void	de_init()
{
}

GameFunctions	test_game_init(GameData *data)
{
	Data = data;

	return (GameFunctions) {
		.name = "Template",
		.start = &start,
		.update = &update,
		.draw = &draw,
		.de_init = &de_init,
	};
}
