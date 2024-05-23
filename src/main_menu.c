#include "game.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

static GameData		*Data;
static RenderTexture2D	TextTexture;
static RenderTexture2D	BackgroundTexture;
static V2		TextTexturePos;
static V2		BackgroundPos;
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

void	draw_blocks(GameData *data);

static void	start() 
{
	//target_pos = (V2) {game_data->window_size.x, 0}; // right
}

static void	de_init()
{
	UnloadRenderTexture(TextTexture);
	UnloadRenderTexture(BackgroundTexture);
}

static void	update()
{
	BeginTextureMode(TextTexture);
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
	EndTextureMode();

	static bool flag = false;

	if (flag == false || IsKeyPressed(KEY_R)) {
		printf("drawing blocks\n");
		BeginTextureMode(BackgroundTexture);
		ClearBackground(Data->palette.white);
		draw_blocks(Data);
		EndTextureMode();
		flag = true;
	}
	//printf("target_pos: %f,%f \n", target_pos.x, target_pos.y);
}

static void draw() 
{
	TextTexturePos = Vector2Lerp(TextTexturePos, (V2) {0, 0}, 0.08);
	BackgroundPos = Vector2Lerp(BackgroundPos, (V2) {0, 0}, 0.11);
	Rect	rect = {0, 0, Data->window_size.x, -Data->window_size.y};
	Rect	rect_back = {0, 0, Data->window_size.x, Data->window_size.y};
	DrawTextureRec(BackgroundTexture.texture, rect_back, BackgroundPos, WHITE);
	DrawTextureRec(TextTexture.texture, rect, TextTexturePos, WHITE);
}

GameFunctions	main_menu_init(GameData *data)
{
	Data = data;
	TextTexture = LoadRenderTexture(data->window_size.x, data->window_size.y);
	BackgroundTexture = LoadRenderTexture(data->window_size.x, data->window_size.y);
	TextConfig = data->assets.fonts[1];
	TextConfig.tint = data->palette.black;
	TextConfig.tint_hover = data->palette.red;

	TextConfigHeading = data->assets.fonts[2];
	TextConfigHeading.tint = data->palette.black;
	TextConfigHeading.tint_hover = data->palette.purple;

	return (GameFunctions) { 
		.name = "Main menu",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

// Draw Text button centralized and add text height to pos, return true if pressed (mouse or keyboard)
bool	text_button(char *text, V2 *pos, char **active_ui, FontConfig config)
{
	bool	r = false;
	bool	mouse_inside = false;
	V2	text_size = MeasureTextEx(config.font, text, config.size, config.spacing);
	V2	offset = {pos->x -  0.5f * text_size.x, pos->y};
	Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};
	Color	color = config.tint;

	if (CheckCollisionPointRec(GetMousePosition(), rect)) {
		*active_ui = text;
		mouse_inside = true;
	}
	if (text == *active_ui) {
		color = config.tint_hover;
		DrawRectangle(offset.x - 10, offset.y + (text_size.y * 0.5f) - 2.5f, 5, 5, RED); // Temp
		if ((mouse_inside && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsActionPressed("action_1")) {
			r = true;
		}
	}

	DrawTextEx(config.font, text, offset, config.size, config.spacing, color);

	pos->y += text_size.y;
	return (r);
}

void	ui_trasition_from(V2 dir)
{
	if (dir.x == 0 && dir.y == -1) { // Bottom
		TextTexturePos = (V2) {0, Data->window_size.y};
		BackgroundPos = (V2) {0, Data->window_size.y};
	} else if (dir.x == 1 && dir.y == 0) { // Right
		TextTexturePos = (V2) {Data->window_size.x, 0};
		BackgroundPos = (V2) {Data->window_size.x,0};
	} else {
		TraceLog(LOG_INFO, "ui_transition_from: dir (%f,%f) not supported \n", dir.x, dir.y);
	}
}

void	draw_blocks(GameData *data) {
	V2	block_size = {60, 30};
	V2	qty = {(data->window_size.x / block_size.x) + 2, (data->window_size.y / block_size.y) + 2};
	ColorPalette	palette = data->palette;
	Color	colors[9] = {
		palette.red,
		palette.blue,
		palette.yellow,
		palette.green,
		palette.orange,
		palette.purple,
		palette.background,
		palette.black,
		palette.white,
	};

	for (int y = 0; y < qty.y; y++) {
		for (int x = 0; x < qty.x; x++) {
			Rect	rect = {x * block_size.x, y * block_size.y, block_size.x, block_size.y};
			Color	color; // = colors[GetRandomValue(0, 7)];
			if (y % 2 == 0) {
				rect.x -= block_size.x * 0.5f;
				if (x % 2 == 0) {
					color = palette.green;
				} else {
					color = palette.background;
				}
			} else {
				if (x % 2 == 0) {
					color = palette.purple;
				} else {
					color = palette.pink;
				}
			}
			DrawRectangleRec(rect, color);
			DrawRectangleLinesEx(rect, 1, palette.black);
		}
	}
}

void	main_menu(GameData *data)
{
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	
	const int	max_ui = 3;
	static char *uis[3] = {
		"Play",
		"Options",
		"Quit",
	};
	static char	*active_ui = NULL;
	static int	ui_index;
	if (active_ui == NULL) {
		active_ui = uis[0];
		ui_index = 0;
	}

	if (IsActionPressed("up")) {
		ui_index -= 1;
		if (ui_index < 0) {
			ui_index = max_ui -1;
		}
		active_ui = uis[ui_index];
	}
	if (IsActionPressed("down")) {
		ui_index += 1;
		if (ui_index >= max_ui) {
			ui_index = 0;
		}
		active_ui = uis[ui_index];
	}
	if (IsActionPressed("action_2")) {
		data->current_ui = BACK;
	}

	// Draw Title
	{
		char	*text = "Raylib Brick Games";
		V2	text_size = MeasureTextEx(TextConfigHeading.font, text, TextConfigHeading.size, TextConfigHeading.spacing);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawRectangle(offset.x - 5, offset.y - 5, text_size.x + 5, text_size.y + 5, data->palette.white);
		DrawTextEx(TextConfigHeading.font, text, offset, TextConfigHeading.size, TextConfigHeading.spacing, TextConfigHeading.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}

	// Play
	if (text_button(uis[0], &center, &active_ui, TextConfig)) {
		data->current_ui = PLAY_MENU;
	}
	center.y += 10; // padding

	// Options
	if (text_button(uis[1], &center, &active_ui, TextConfig)) {
		data->current_ui = OPTIONS_MENU;
	}
	center.y += 10; // padding

	if (text_button(uis[2], &center,  &active_ui, TextConfig)) {
		data->quit = true;
	}
}

void	play_menu(GameData *data)
{
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	
	const int	max_ui = 4;
	static char *uis[4] = {
		"Tetris",
		"Snake",
		"Test",
		"Back",
	};
	static char	*active_ui = NULL;
	static int	ui_index;
	if (active_ui == NULL) {
		active_ui = uis[0];
		ui_index = 0;
	}

	if (IsActionPressed("up")) {
		ui_index -= 1;
		if (ui_index < 0) {
			ui_index = max_ui -1;
		}
		active_ui = uis[ui_index];
	}
	if (IsActionPressed("down")) {
		ui_index += 1;
		if (ui_index >= max_ui) {
			ui_index = 0;
		}
		active_ui = uis[ui_index];
	}
	if (IsActionPressed("action_2")) {
		data->current_ui = BACK;
	}

	// Draw Title
	{
		char	*text = "Games:";
		V2	text_size = MeasureTextEx(TextConfigHeading.font, text, TextConfigHeading.size, TextConfigHeading.spacing);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawRectangle(offset.x - 5, offset.y - 5, text_size.x + 5, text_size.y + 5, data->palette.white);
		DrawTextEx(TextConfigHeading.font, text, offset, TextConfigHeading.size, TextConfigHeading.spacing, TextConfigHeading.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}

	// Tetris
	if (text_button(uis[0], &center, &active_ui, TextConfig)) {
		data->current_game = TETRIS;
		data->current_ui = NONE;
	}
	center.y += 10; // padding

	// Snake
	if (text_button(uis[1], &center, &active_ui, TextConfig)) {
		data->current_game = SNAKE_GAME;
		data->current_ui = NONE;
	}
	center.y += 10; // padding

	// Test
	if (text_button(uis[2], &center,  &active_ui, TextConfig)) {
		data->current_game = TEST;
		data->current_ui = NONE;
	}
	center.y += 10; // padding
	
	// Back
	if (text_button(uis[3], &center,  &active_ui, TextConfig)) {
		data->current_ui = BACK;
	}
}

void	options_menu(GameData *data)
{
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text

	const int	max_ui = 2;
	static char *uis[2] = {
		"Nothing",
		"Back",
	};
	static char	*active_ui = NULL;
	static int	ui_index;
	if (active_ui == NULL) {
		active_ui = uis[0];
		ui_index = 0;
	}

	if (IsActionPressed("up")) {
		ui_index -= 1;
		if (ui_index < 0) {
			ui_index = max_ui -1;
		}
		active_ui = uis[ui_index];
	}
	if (IsActionPressed("down")) {
		ui_index += 1;
		if (ui_index >= max_ui) {
			ui_index = 0;
		}
		active_ui = uis[ui_index];
	}
	if (IsActionPressed("action_2")) {
		data->current_ui = BACK;
	}
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
	
	// Nothing
	if (text_button(uis[0], &center, &active_ui,  TextConfig)){
		printf("Nothing\n");
	}

	// BACK
	if (text_button(uis[1], &center, &active_ui,  TextConfig)){
		data->current_ui = BACK;
	}
}

void	game_over_menu(GameData *data)
{
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	
	const int	max_ui = 2;
	static char *uis[2] = {
		"Play Again",
		"Quit to main menu",
	};
	static char	*active_ui = NULL;
	static int	ui_index;
	if (active_ui == NULL) {
		active_ui = uis[0];
		ui_index = 0;
	}

	if (IsActionPressed("up")) {
		ui_index -= 1;
		if (ui_index < 0) {
			ui_index = max_ui -1;
		}
		active_ui = uis[ui_index];
	}
	if (IsActionPressed("down")) {
		ui_index += 1;
		if (ui_index >= max_ui) {
			ui_index = 0;
		}
		active_ui = uis[ui_index];
	}
	if (IsActionPressed("action_2")) {
		data->current_ui = BACK;
	}

	// Draw Title
	{
		char	*text = "Game Over";
		V2	text_size = MeasureTextEx(TextConfigHeading.font, text, TextConfigHeading.size, TextConfigHeading.spacing);
		V2	offset = {center.x -  0.5f * text_size.x, center.y};
		DrawTextEx(TextConfigHeading.font, text, offset, TextConfigHeading.size, TextConfigHeading.spacing, TextConfig.tint);
		center.y += text_size.y;
		center.y += 15; // Add padding
	}

	// Play again
	if (text_button(uis[0], &center, &active_ui, TextConfig)) {
		data->current_ui = NONE;
	}
	
	center.y += 30;

	// Quit to main menu
	if (text_button(uis[1], &center, &active_ui, TextConfig)) {
		data->current_ui = TITLE_SCREEN;
	}
}
