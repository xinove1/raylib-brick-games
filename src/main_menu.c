#include "game.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

typedef struct {
	Font	font;
	int	size;
	int	spacing;
	Color	tint;
	Color	tint_hover;
} FontConfig;

static GameData	*game_data;
RenderTexture2D	target_texture;

static void	main_menu(GameData *data);
static void	play_menu(GameData *data);
static void	options_menu(GameData *data);
static void	keybind_menu(GameData *data); // TODO
static void	paused_menu(GameData *data);
static void	colors_menu(GameData *data);
static void	game_over_menu(GameData *data);
static void	tutorial_menu(GameData *data);

static void	start() 
{
}

static void	de_init()
{
	UnloadRenderTexture(target_texture);
}

static void	update()
{
	// V2	target_pos = {0, 0};
	// {
	// 	V2	from = {0, game_data->window_size.y};
	// 	V2	to = {0, 0};
	// 	target_pos = Vector2Lerp(from, to, 5);
	// }
	
	BeginTextureMode(target_texture);
	ClearBackground(RAYWHITE);

	switch (game_data->current_ui) {
		case (TITLE_SCREEN):
			main_menu(game_data);
			break;
		case (PLAY_MENU):
			play_menu(game_data);
			break;
		case (OPTIONS_MENU):
			options_menu(game_data);
			break;
		case (GAME_OVER_MENU):
			game_over_menu(game_data);
			break;
		// case(PAUSED_MENU):
		// 	paused_menu(game_data);
		// 	break;
		default:
			TraceLog(LOG_INFO, "Main_menu.c: MenuScreen not implementd. state id: %d \n", game_data->current_ui);
			break;
	}
	if (IsActionPressed("action_1")) {
		printf("main menu state: %d \n", game_data->current_ui);
	}

	EndTextureMode();

	BeginDrawing();
	Rect	rect = {0, 0, game_data->window_size.x, -game_data->window_size.y};
	DrawTextureRec(target_texture.texture, rect, (V2){0, 0}, WHITE);
	EndDrawing();
}

GameFunctions	main_menu_init(GameData *data)
{
	game_data = data;
	target_texture = LoadRenderTexture(data->window_size.x, data->window_size.y);
	return (GameFunctions) { 
		.name = "Main menu",
		.update = &update,
		.start = &start,
		.de_init = &de_init,
	};
}

// Draw Text button centralized and add text height to pos
bool	text_button(char *text, V2 *pos, FontConfig config)
{
	bool	r = false;
	V2	text_size = MeasureTextEx(config.font, text, config.size, config.spacing);
	V2	offset = {pos->x -  0.5f * text_size.x, pos->y};
	Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};
	Color	color = config.tint;

	if (CheckCollisionPointRec(GetMousePosition(), rect)) {
		color = config.tint_hover;
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			r = true;
		}
	}

	DrawTextEx(config.font, text, offset, config.size, config.spacing, color);

	pos->y += text_size.y;

	return (r);
}

void	main_menu(GameData *data)
{
	V2	window = data->window_size;
	Font	font = GetFontDefault();
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	FontConfig	text_config = {
		.font = GetFontDefault(),
		.size = 35,
		.spacing = 3,
		.tint = data->palette.red,
		.tint_hover = data->palette.purple,
	};

	// Draw Title
	{
		char	*text = "Raylib Brick Games";
		V2	text_size = MeasureTextEx(font, text, 50, 5);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(text_config.font, text, offset, 50, 5, text_config.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}

	if (text_button("Play", &center, text_config)) {
		data->current_ui = PLAY_MENU;
	}
	center.y += 10; // padding

	if (text_button("Options", &center, text_config)) {
		data->current_ui = OPTIONS_MENU;
	}
	center.y += 10; // padding

	if (text_button("Quit", &center, text_config)) {
		data->quit = true;
	}
}

void	play_menu(GameData *data)
{
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
				data->current_game = SNAKE_GAME;
				data->current_ui = NONE;
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
				data->current_game = TETRIS;
				data->current_ui = NONE;
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
				data->current_ui = BACK;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);
		center.y += text_size.y;
		center.y += 5; // Add padding
	}
}

void	options_menu(GameData *data)
{
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	FontConfig	text_config = {
		.font = GetFontDefault(),
		.size = 35,
		.spacing = 3,
		.tint = data->palette.red,
		.tint_hover = data->palette.purple,
	};

	DrawRectangleV((V2){0,0}, window, (Color){ 100, 100, 100, 100}); 

	// Draw Title
	{
		char	*text = "Options";
		V2	text_size = MeasureTextEx(text_config.font, text, 50, 5);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(text_config.font, text, offset, 50, 5, text_config.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}


	// TODO add options

	if (text_button("Back", &center, text_config)){
		data->current_ui = BACK;
	}
}

void	game_over_menu(GameData *data)
{
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	FontConfig	text_config = {
		.font = GetFontDefault(),
		.size = 35,
		.spacing = 3,
		.tint = data->palette.red,
		.tint_hover = data->palette.purple,
	};
	// Draw Title
	{
		char	*text = "Game Over";
		V2	text_size = MeasureTextEx(text_config.font, text, 50, 5);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(text_config.font, text, offset, 50, 5, text_config.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}

	if (text_button("Play Again", &center, text_config)) {
		data->current_ui = NONE;
	}
	
	center.y += 30;

	if (text_button("Quit to main menu", &center, text_config)) {
		data->current_ui = TITLE_SCREEN;
	}
}
