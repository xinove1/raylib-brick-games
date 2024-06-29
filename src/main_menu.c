#include "game.h"
#include "ui.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

static GameData		*Data;
static RenderTexture2D	TextTexture;
static RenderTexture2D	BackgroundTexture;
static V2		TextTexturePos;
static V2		BackgroundPos;
static V2		BrickSize = {60, 30};
static float	tick_time = 0.050f; // In seconds
static FontConfig	TextConfig;
static FontConfig	TextConfigHeading;
static Color		UiBackgroundColor;
static UiState	current_screen;
static void	title_screen(GameData *data);
static void	play_screen(GameData *data);
static void	paused_screen(GameData *data);

// TODO : 
static void	tutorial_screen(GameData *data);
static void	colors_screen(GameData *data);
static void	keybind_screen(GameData *data);

void	draw_blocks(GameData *data);

static void	start() 
{
	//target_pos = (V2) {game_data->window_size.x, 0}; // right
	ui_trasition_from((V2){0, -1});
	current_screen = TITLE_SCREEN;
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

	switch (current_screen) {
		case (TITLE_SCREEN):
			title_screen(Data);
			break;
		case (PLAY_MENU):
			play_screen(Data);
			break;
		case (OPTIONS_MENU):
			if (options_screen(Data) == BACK) {
				current_screen = TITLE_SCREEN;
			}
			break;
		case (GAME_OVER_MENU):
			game_over_screen(Data);
			break;
		// case(PAUSED_MENU):
		// 	paused_menu(game_data);
		// 	break;
		default:
			TraceLog(LOG_INFO, "Main_menu.c: MenuScreen not implementd. state id: %d \n", current_screen);
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

	// TODO  Change to snap to 0, 0?
	TextTexturePos = ExpDecayV2(TextTexturePos, (V2) {0, 0}, 2.5f);
	//BackgroundPos = ExpDecayV2(BackgroundPos, (V2) {0, -(BrickSize.y * 0.2f)}, 3.5);
	{
		static V2	speed = {0,0};
		V2	target_pos = {0, -(BrickSize.y * 0.2f)};
		speed = ExpDecayV2(speed, Vector2Scale(Vector2Subtract(target_pos, BackgroundPos), 0.05f), 4.0f);

		BackgroundPos = Vector2Add(BackgroundPos, speed);
	}
}

static void draw() 
{
	Rect	rect = {0, 0, Data->window_size.x, -Data->window_size.y};
	Rect	rect_back = {0, 0, Data->window_size.x * 1.2f, -Data->window_size.y * 1.2f};
	DrawTextureRec(BackgroundTexture.texture, rect_back, BackgroundPos, WHITE);
	DrawTextureRec(TextTexture.texture, rect, TextTexturePos, WHITE);

	{
		char	*text  = "By: @thomi_dx"; // FIX  When created twitter account
		V2	text_size = MeasureTextEx(TextConfig.font, text, TextConfig.size, TextConfig.spacing); 
		V2	pos = {Data->window_size.x - text_size.x - 5, Data->window_size.y - text_size.y - 5};
		DrawTextEx(TextConfig.font, text, pos, TextConfig.size, TextConfig.spacing, TextConfig.tint);
	}
}

GameFunctions	main_menu_init(GameData *data)
{
	Data = data;
	TextTexture = LoadRenderTexture(data->window_size.x, data->window_size.y);
	BackgroundTexture = LoadRenderTexture(data->window_size.x * 1.2f, data->window_size.y * 1.2f);

	UiBackgroundColor = data->palette.red;
	TextConfig = data->assets.fonts[1];
	TextConfig.tint = data->palette.black;
	TextConfig.tint_hover = data->palette.red;
	TextConfigHeading = data->assets.fonts[2];
	TextConfigHeading.tint = data->palette.black;
	TextConfigHeading.tint_hover = data->palette.blue;

	current_screen = TITLE_SCREEN;

	return (GameFunctions) { 
		.name = "Main menu",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

void	ui_trasition_from(V2 dir)
{
	if (dir.x == 0 && dir.y == -1) { // Bottom
		TextTexturePos = (V2) {0, Data->window_size.y};
		BackgroundPos = (V2) {0, Data->window_size.y};
	} else if (dir.x == 1 && dir.y == 0) { // Right
		TextTexturePos = (V2) {Data->window_size.x, 0};
		BackgroundPos = (V2) {Data->window_size.x, 0};
	} else {
		TraceLog(LOG_INFO, "ui_transition_from: dir (%f,%f) not supported \n", dir.x, dir.y);
	}
}

void	draw_blocks(GameData *data) {
	V2	qty = {(data->window_size.x / BrickSize.x) + 4, (data->window_size.y / BrickSize.y) + 2};
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
		float	x_offset = 0;
		for (int x = 0; x < qty.x; x++) {
			Rect	rect = {x_offset, y * BrickSize.y, BrickSize.x, BrickSize.y};
			Color	color; // = colors[GetRandomValue(0, 7)];
			if (y % 2 == 0) {
				rect.x -= BrickSize.x * 0.5f;
				if (x % 2 == 0) {
					color = palette.green;
					rect.width = rect.width * 0.80f;
				} else {
					color = palette.background;
				}
			} else {
				if (x % 2 == 0) {
					color = palette.purple;
				} else {
					rect.width = rect.width * 0.60f;
					color = palette.pink;
				}
			}
			DrawRectangleRec(rect, color);
			DrawRectangleLinesEx(rect, 1, palette.black);
			x_offset += rect.width;
		}
	}
}

void	title_screen(GameData *data)
{
	static UiPanel	panel = {.id_current = 0, .centralized = true};
	
	DrawRectangle(panel.pos.x - 2, panel.pos.y, panel.width, panel.at_y - panel.pos.y, UiBackgroundColor);
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	panel.pos = center;

	panel_begin(&panel);
	{
		panel_text(&panel, "Raylib Brick Games", TextConfigHeading);

		if (panel_text_button(&panel, "Play", TextConfig)) {
			current_screen = PLAY_MENU;
		}

		if (panel_text_button(&panel, "Options", TextConfig)) {
			current_screen = OPTIONS_MENU;
		}
		if (panel_text_button(&panel, "Quit", TextConfig) || IsActionPressed(ACTION_2)) {
			data->quit = true;
		}
	}
	panel_end(&panel);
}

void	play_screen(GameData *data)
{
	static UiPanel	panel = {.id_current = 0, .centralized = true};
	
	DrawRectangle(panel.pos.x - 2, panel.pos.y, panel.width, panel.at_y - panel.pos.y, UiBackgroundColor);
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	panel.pos = center;

	panel_begin(&panel);
	panel_text(&panel, "Games", TextConfigHeading);

	if (panel_text_button(&panel, "Tetris", TextConfig)) {
		data->current_game = TETRIS;
		current_screen = NONE;
	}

	if (panel_text_button(&panel, "Snake", TextConfig)) {
		data->current_game = SNAKE_GAME;
		current_screen = NONE;
	}

	if (panel_text_button(&panel, "Test", TextConfig)) {
		data->current_game = TEST;
		current_screen = NONE;
	}
	
	if (panel_text_button(&panel, "Back", TextConfig) || IsActionPressed(ACTION_2)) {
		current_screen = TITLE_SCREEN;
	}

	panel_end(&panel);
}

UiState	options_screen(GameData *data)
{
	static UiPanel	panel = {.id_current = 0, .centralized = true};
	static bool	flag = false;
	if (flag == false) {
		V2	txt_size = MeasureTextEx(TextConfig.font, "test", TextConfig.size, TextConfig.spacing);
		panel.height = txt_size.y;
		flag = true;
	}
	
	DrawRectangle(panel.pos.x - 2, panel.pos.y, panel.width, panel.at_y - panel.pos.y, UiBackgroundColor);
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	panel.pos = center;

	panel_begin(&panel);
	panel_text(&panel, "Options", TextConfigHeading);


	panel_text(&panel, "Music Volume", TextConfig);
	if (panel_slider(&panel, &data->music_vol, 0, 1)) {
		update_volume(Data);
	}

	
	// if (panel_text_button(&panel, "Effects Volume:", TextConfig)) {
	// }
	panel_text(&panel, "Effects Volume", TextConfig);
	if (panel_slider(&panel, &data->effects_vol, 0, 1)) {
		update_volume(Data);
	}
	
	if (panel_text_button(&panel, "Back", TextConfig) || IsActionPressed(ACTION_2)) {
		return (BACK);
	}

	panel_end(&panel);
	return (NOTHING);
}

UiState	game_over_screen(GameData *data)
{
	static UiPanel	panel = {.id_current = 0, .centralized = true};
	
	DrawRectangle(panel.pos.x - 2, panel.pos.y, panel.width, panel.at_y - panel.pos.y, UiBackgroundColor);
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	panel.pos = center;

	panel_begin(&panel);
	panel_text(&panel, "Game Over", TextConfigHeading);

	if (panel_text_button(&panel, "Play Again", TextConfig)) {
		return (NONE);
	}
	
	// Quit to main menu
	if (panel_text_button(&panel, "Quit to Main Menu", TextConfig)) {
		return (TITLE_SCREEN);
	}
	if (panel_text_button(&panel, "Quit to Desktop", TextConfig)) {
		data->quit = true;
	}
	panel_end(&panel);
	return (NOTHING);
}
