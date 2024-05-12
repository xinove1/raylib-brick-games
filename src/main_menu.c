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

static GameData		*Data;
static RenderTexture2D	TargetTexture;
static V2		TargetPos;
static FontConfig	TextConfig;
static FontConfig	TextConfigHeading;
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
	//target_pos = (V2) {game_data->window_size.x, 0}; // right
}

static void	de_init()
{
	UnloadRenderTexture(TargetTexture);
}

static void	update()
{
	// V2	target_pos = {0, 0};
	// {
	// 	V2	from = {0, game_data->window_size.y};
	// 	V2	to = {0, 0};
	// 	target_pos = Vector2Lerp(from, to, 5);
	// }
	
	BeginTextureMode(TargetTexture);
	ClearBackground((Color){0, 0, 0, 0});

	switch (Data->current_ui) {
		case (TITLE_SCREEN):
			main_menu(Data);
			break;
		case (PLAY_MENU):
			play_menu(Data);
			break;
		case (OPTIONS_MENU):
			options_menu(Data);
			break;
		case (GAME_OVER_MENU):
			game_over_menu(Data);
			break;
		// case(PAUSED_MENU):
		// 	paused_menu(game_data);
		// 	break;
		default:
			TraceLog(LOG_INFO, "Main_menu.c: MenuScreen not implementd. state id: %d \n", Data->current_ui);
			break;
	}
	if (IsActionPressed("action_1")) {
		printf("main menu state: %d \n", Data->current_ui);
	}

	EndTextureMode();

	BeginDrawing();
	ClearBackground(RAYWHITE);
	//printf("target_pos: %f,%f \n", target_pos.x, target_pos.y);
	TargetPos = Vector2Lerp(TargetPos, (V2) {0, 0}, 0.08);
	Rect	rect = {0, 0, Data->window_size.x, -Data->window_size.y};
	DrawTextureRec(TargetTexture.texture, rect, TargetPos, WHITE);
	EndDrawing();
}

GameFunctions	main_menu_init(GameData *data)
{
	Data = data;
	TargetTexture = LoadRenderTexture(data->window_size.x, data->window_size.y);
	TextConfig = (FontConfig) {
		.font = data->assets.fonts[1],
		.size = 22,
		.spacing = 2,
		.tint = data->palette.red,
		.tint_hover = data->palette.purple,
	};

	TextConfigHeading = (FontConfig) {
		.font = data->assets.fonts[2],
		.size = 42,
		.spacing = 2,
		.tint = data->palette.blue,
		.tint_hover = data->palette.purple,
	};

	return (GameFunctions) { 
		.name = "Main menu",
		.update = &update,
		.start = &start,
		.de_init = &de_init,
	};
}

void	ui_trasition_from(V2 dir)
{
	if (dir.x == 0 && dir.y == -1) { // Bottom
		TargetPos = (V2) {0, Data->window_size.y};
	} else if (dir.x == 1 && dir.y == 0) { // Right
		TargetPos = (V2) {Data->window_size.x, 0};
	} else {
		TraceLog(LOG_INFO, "ui_transition_from: dir (%f,%f) not supported \n", dir.x, dir.y);
	}
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
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text

	// Draw Title
	{
		char	*text = "Raylib Brick Games";
		V2	text_size = MeasureTextEx(TextConfigHeading.font, text, TextConfigHeading.size, TextConfigHeading.spacing);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(TextConfigHeading.font, text, offset, TextConfigHeading.size, TextConfigHeading.spacing, TextConfigHeading.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}

	if (text_button("Play", &center, TextConfig)) {
		data->current_ui = PLAY_MENU;
	}
	center.y += 10; // padding

	if (text_button("Options", &center, TextConfig)) {
		data->current_ui = OPTIONS_MENU;
	}
	center.y += 10; // padding

	if (text_button("Quit", &center, TextConfig)) {
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

	//DrawRectangleV((V2){0,0}, window, (Color){ 100, 100, 100, 100}); 

	// Draw Title
	{
		char	*text = "Options";
		V2	text_size = MeasureTextEx(TextConfigHeading.font, text, TextConfigHeading.size, TextConfigHeading.spacing);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(TextConfigHeading.font, text, offset, TextConfigHeading.size, TextConfigHeading.spacing, TextConfig.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}


	// TODO add options

	if (text_button("Back", &center, TextConfig)){
		data->current_ui = BACK;
	}
}

void	game_over_menu(GameData *data)
{
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	// Draw Title
	{
		char	*text = "Game Over";
		V2	text_size = MeasureTextEx(TextConfigHeading.font, text, TextConfigHeading.size, TextConfigHeading.spacing);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(TextConfigHeading.font, text, offset, TextConfigHeading.size, TextConfigHeading.spacing, TextConfig.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}

	if (text_button("Play Again", &center, TextConfig)) {
		data->current_ui = NONE;
	}
	
	center.y += 30;

	if (text_button("Quit to main menu", &center, TextConfig)) {
		data->current_ui = TITLE_SCREEN;
	}
}
