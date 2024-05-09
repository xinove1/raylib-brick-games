#include "game.h"
#include <stdio.h>

typedef enum {TITLE_SCREEN, PLAY_MENU, OPTIONS_MENU} States;

static States	state;
static States	previous_state;
static GameData	*data;

static void	draw_main_menu();
static void	draw_play_menu();
static void	draw_ui();


static void	start() 
{
	state = TITLE_SCREEN; // State 
	previous_state = TITLE_SCREEN; // State 
}

static void	de_init() {}

static void	update()
{
	switch (state) {
		printf("main_menu state: %d\n", state);
		case(TITLE_SCREEN):
			draw_main_menu();
			break;
		case(PLAY_MENU):
			draw_play_menu();
			break;
		case(OPTIONS_MENU):
			draw_options_menu();
			break;
	}
}

GameFunctions	main_menu_init(GameData *game_data)
{
	data = game_data;
	return (GameFunctions) { 
		.name = "Main menu",
		.update = &update,
		.start = &start,
		.de_init = &de_init,
	};
}

static void	draw_main_menu()
{
	BeginDrawing();
	ClearBackground(RAYWHITE);
	V2	window = data->window_size;
	Font	font = GetFontDefault();
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text

	// Draw Title
	{
		char	*text = "Raylib Brick Games";
		V2	text_size = MeasureTextEx(font, text, 50, 5);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(GetFontDefault(), text, offset, 50, 5, RED);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}

	{
		char	*text = "Play";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = RED;
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = PURPLE;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				printf("mouse button released\n");
				state = PLAY_MENU;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);

		center.y += text_size.y;
		center.y += 10; // Add padding
	}
	{
		char	*text = "Options";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = RED;
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = PURPLE;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				printf("mouse button released\n");
				state = OPTIONS_MENU;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);

		center.y += text_size.y;
		center.y += 5; // Add padding
	}
	{
		char	*text = "Quit";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = RED;
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = PURPLE;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				data->quit = true;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);

		center.y += text_size.y;
		center.y += 5; // Add padding
	}

	EndDrawing();
}

static void	draw_play_menu() 
{

	BeginDrawing();
	ClearBackground(RAYWHITE);

	V2	window = data->window_size;
	ColorPalette	palette = data->palette;
	Font	font = GetFontDefault();

	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	Rect	target_rect = {window.x * 0.1f, window.y * 0.2f, window.x * 0.8f, window.y * 0.6f}; 
	DrawRectangleLinesEx(target_rect, 5, palette.blue);
	center.y += target_rect.height;
	center.y += 10;

	{
		char	*text = "Snake Game";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = palette.green;
		V2	offset = {target_rect.x + 30, target_rect.y + 30};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = palette.yellow;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				previous_state = PLAY_MENU;
				data->current_game = SNAKE_GAME;
			}
		}
		DrawRectangleRec(rect, BLUE);
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);

		target_rect.x += 30 + text_size.x;
		target_rect.y += 30;
	}

	{
		char	*text = "Tetris";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = palette.blue;
		V2	offset = {target_rect.x + text_size.x, target_rect.y};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = palette.purple;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				previous_state = PLAY_MENU;
				data->current_game = TETRIS;
			}
		}
		DrawRectangleRec(rect, palette.yellow);
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);
	}

	{
		char	*text = "Back";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = palette.red;
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = palette.purple;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				state = TITLE_SCREEN;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);
		center.y += text_size.y;
		center.y += 5; // Add padding
	}
	EndDrawing();
}

void	draw_options_menu()
{
	BeginDrawing();
	ClearBackground(RAYWHITE);
	Font	font = GetFontDefault();

	V2	window = data->window_size;
	ColorPalette	palette = data->palette;

	DrawRectangleV((V2){0,0}, window, (Color){ 100, 100, 100, 100}); 
	char	*str = "Options";
	V2	str_size = MeasureTextEx(font, str, 30, 4);
	V2	offset = {window.x / 2 - str_size.x / 2, window.y / 2 - str_size.y / 2};
	Rect	rect = {offset.x , offset.y + 50, str_size.x, str_size.y};

	DrawRectangleV(offset, str_size, (Color){ 100, 200, 100, 255}); 
	DrawTextEx(font, str, offset, 30, 4, RED);

	GuiSliderBar(rect, NULL, NULL, &data->music_vol, 0.0, 1.0);

	rect = (Rect){rect.x, rect.y + 25, rect.width, rect.height};
	GuiSliderBar(rect, NULL, NULL, &data->effects_vol, 0.0, 1.0);

	{
		char	*text = "Back";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = palette.red;
		V2	offset = {rect.x -  0.5f * text_size.x, rect.y + 30};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = palette.purple;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				state = previous_state;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);
	}
	EndDrawing();
}

int	game_over_screen(GameData *data) 
{
	V2	window = data->window_size;
	Font	font = GetFontDefault();
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text

	// Draw Title
	{
		char	*text = "Game Over";
		V2	text_size = MeasureTextEx(font, text, 50, 5);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(GetFontDefault(), text, offset, 50, 5, RED);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}
	{
		char	*text = "Play Again";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = RED;
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = PURPLE;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				return 1;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);

		center.y += text_size.y;
		center.y += 10; // Add padding
	}
	{
		char	*text = "Go back to menu";
		V2	text_size = MeasureTextEx(font, text, 35, 3);
		Color	text_color = RED;
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};

		if (CheckCollisionPointRec(GetMousePosition(), rect)) {
			text_color = PURPLE;
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				data->current_game = MAIN_MENU;
				//previous_state = TITLE_SCREEN;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);

		center.y += text_size.y;
		center.y += 5; // Add padding
	}
	return 0;
}
