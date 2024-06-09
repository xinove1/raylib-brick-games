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
}

GameFunctions	main_menu_init(GameData *data)
{
	Data = data;
	TextTexture = LoadRenderTexture(data->window_size.x, data->window_size.y);
	BackgroundTexture = LoadRenderTexture(data->window_size.x * 1.2f, data->window_size.y * 1.2f);
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

void	main_menu(GameData *data)
{
	static UiPanel	panel = {.id_current = 0, .centralized = true};
	static bool	flag = false;
	if (flag == false) {
		panel.id_current = 0;
		flag = true;
	}
	
	DrawRectangle(panel.pos.x, panel.pos.y, panel.width, panel.at_y - panel.pos.y, RED);
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	panel.pos = center;

	panel_begin(&panel);
	panel_title(&panel, "Raylib Brick Games", TextConfigHeading);

	if (panel_text_button(&panel, "Play", TextConfig)) {
		data->current_ui = PLAY_MENU;
	}

	if (panel_text_button(&panel, "Options", TextConfig)) {
		data->current_ui = OPTIONS_MENU;
	}
	if (panel_text_button(&panel, "Quit", TextConfig) || IsActionPressed(ACTION_2)) {
		data->quit = true;
	}
	panel_end(&panel);
}

void	play_menu(GameData *data)
{
	static UiPanel	panel = {.id_current = 0, .centralized = true};
	static bool	flag = false;
	if (flag == false) {
		panel.id_current = 0;
		flag = true;
	}
	
	DrawRectangle(panel.pos.x, panel.pos.y, panel.width, panel.at_y - panel.pos.y, RED);
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	panel.pos = center;

	panel_begin(&panel);
	panel_title(&panel, "Games", TextConfigHeading);

	if (panel_text_button(&panel, "Tetris", TextConfig)) {
		data->current_game = TETRIS;
		data->current_ui = NONE;
	}

	if (panel_text_button(&panel, "Snake", TextConfig)) {
		data->current_game = SNAKE_GAME;
		data->current_ui = NONE;
	}

	if (panel_text_button(&panel, "Test", TextConfig)) {
		data->current_game = TEST;
		data->current_ui = NONE;
	}
	
	if (panel_text_button(&panel, "Back", TextConfig) || IsActionPressed(ACTION_2)) {
		data->current_ui = BACK;
	}

	panel_end(&panel);
}

void	options_menu(GameData *data)
{
	static UiPanel	panel = {.id_current = 0, .centralized = true};
	static bool	flag = false;
	if (flag == false) {
		panel.id_current = 0;
		flag = true;
	}
	
	DrawRectangle(panel.pos.x, panel.pos.y, panel.width, panel.at_y - panel.pos.y, RED);
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	panel.pos = center;

	panel_begin(&panel);
	panel_title(&panel, "Options", TextConfigHeading);

	if (panel_text_button(&panel, "NOthing", TextConfig)) {
	}

	if (panel_text_button(&panel, "NOthing agian!", TextConfig)) {
	}
	
	if (panel_text_button(&panel, "Back", TextConfig) || IsActionPressed(ACTION_2)) {
		data->current_ui = BACK;
	}

	panel_end(&panel);
}

void	game_over_menu(GameData *data)
{
	static UiPanel	panel = {.id_current = 0, .centralized = true};
	static bool	flag = false;
	if (flag == false) {
		panel.id_current = 0;
		flag = true;
	}
	
	DrawRectangle(panel.pos.x, panel.pos.y, panel.width, panel.at_y - panel.pos.y, RED);
	V2	window = data->window_size;
	V2	center = {window.x * 0.5f, window.y * 0.25f}; // Center offset to where to start drawing text
	panel.pos = center;

	panel_begin(&panel);
	panel_title(&panel, "Game Over", TextConfigHeading);

	if (panel_text_button(&panel, "Play Again", TextConfig)) {
		data->current_ui = NONE;
	}
	
	// Quit to main menu
	if (panel_text_button(&panel, "Quit to Main Menu", TextConfig)) {
		data->current_ui = TITLE_SCREEN;
	}
	if (panel_text_button(&panel, "Quit to Desktop", TextConfig)) {
		data->quit = true;
	}

	panel_end(&panel);
}
