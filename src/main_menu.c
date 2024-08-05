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
static FontConfig	TextConfig;
static FontConfig	TextConfigHeading;
static UiContainer	Container;
static Color		UiBackgroundColor;
static UiStates	current_screen;
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

	V2	center_screen = {data->window_size.x * 0.5f, data->window_size.y * 0.25f}; // Center offset to where to start drawing text
	Container = CreateContainer(center_screen, 0, data->ui_config);

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
	UiContainer *panel = &Container;

	UiBegin(panel);
	{
		UiText(panel, "Raylib Brick Games", true);

		if (UiTextButton(panel, "Play")) {
			current_screen = PLAY_MENU;
		}

		if (UiTextButton(panel, "Options")) {
			current_screen = OPTIONS_MENU;
		}
		if (UiTextButton(panel, "Quit") || IsActionPressed(ACTION_2)) {
			data->quit = true;
		}
	}
	UiEnd(panel);
}

void	play_screen(GameData *data)
{
	UiContainer *panel = &Container;
	
	UiBegin(panel);
	{
		UiText(panel, "Games", true);

		if (UiTextButton(panel, "Tetris")) {
			data->current_game = TETRIS;
			current_screen = NONE;
		}

		if (UiTextButton(panel, "Snake")) {
			data->current_game = SNAKE_GAME;
			current_screen = NONE;
		}

		if (UiTextButton(panel, "Pong")) {
			data->current_game = PONG;
			current_screen = NONE;
		}

		if (UiTextButton(panel, "BreakOut")) {
			data->current_game = BREAKOUT;
			current_screen = NONE;
		}

		if (UiTextButton(panel, "Test")) {
			data->current_game = TEST;
			current_screen = NONE;
		}

		if (UiTextButton(panel, "Back") || IsActionPressed(ACTION_2)) {
			current_screen = TITLE_SCREEN;
		}
	}
	UiEnd(panel);
}

UiStates	options_screen(GameData *data)
{
	UiContainer *panel = &Container;

	UiBegin(panel);
	{
		UiText(panel, "Options", true);

		UiText(panel, "Music Volume", true);
		if (UiSlider(panel, &data->music_vol, 0, 1)) {
			update_volume(Data);
		}

		UiText(panel, "Effects Volume", true);
		if (UiSlider(panel, &data->effects_vol, 0, 1)) {
			update_volume(Data);
		}

		if (UiTextButton(panel, "Back") || IsActionPressed(ACTION_2)) {
			return (BACK);
		}
	}
	UiEnd(panel);
	return (NOTHING);
}

UiStates	game_over_screen(GameData *data)
{
	UiContainer *panel = &Container;

	UiBegin(panel);
	{
		UiText(panel, "Game Over", true);

		if (UiTextButton(panel, "Play Again")) {
			return (NONE);
		}

		// Quit to main menu
		if (UiTextButton(panel, "Quit to Main Menu")) {
			return (TITLE_SCREEN);
		}
		if (UiTextButton(panel, "Quit to Desktop")) {
			data->quit = true;
		}
	}
	UiEnd(panel);

	return (NOTHING);
}
