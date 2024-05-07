#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#define XI_INPUT_ACTIONS_IMPLEMENTATION

#include "game.h"
#include "style_candy.h"

void	draw_main_menu(GameData *data);
void	draw_play_menu(GameData *data);
void	draw_options_menu(GameData *data);

typedef enum {TETRIS, SNAKE_GAME, GAME_COUNT} Games_e;

static GameFunctions	games_lst[GAME_COUNT] = {0};
static GameFunctions	game;

int	main()
{
	GameData	data = {0};

	data.window_size = (V2) {800, 600};
	data.music_vol = 1.0f;
	data.effects_vol = 1.0f;
	data.state = MAIN_MENU;
	data.previous_state = MAIN_MENU;
	data.palette = (ColorPalette) {
		.black = BLACK,
		.white = RAYWHITE,
		.red = RED,
		.blue = BLUE,
		.yellow = YELLOW,
		.green = GREEN,
		.orange = ORANGE,
		.purple = PURPLE,
	};

	games_lst[SNAKE_GAME] = snake_game_init(&data);

	InitWindow(data.window_size.x,data.window_size.y, "Raylib Bricks games");
	InitAudioDevice();
	SetTargetFPS(60);
	SetExitKey(0);

	register_input_action("right", KEY_D);
	register_input_action("right", KEY_RIGHT);
	register_input_action("left", KEY_A);
	register_input_action("left", KEY_LEFT);
	register_input_action("up", KEY_W);
	register_input_action("up", KEY_UP);
	register_input_action("down", KEY_S);
	register_input_action("down", KEY_DOWN);

	register_input_action("action_1", KEY_J);
	register_input_action("action_1", KEY_X);
	register_input_action("action_2", KEY_K);
	register_input_action("action_2", KEY_Z);
	register_input_action("open_menu", KEY_ESCAPE);
	register_input_action("open_menu", KEY_E);

	game = games_lst[SNAKE_GAME];
	game.start();

	while (!WindowShouldClose() && !data.quit) {
		switch (data.state) {
			case MAIN_MENU:
				draw_main_menu(&data); break;
			case PLAY_MENU:
				draw_play_menu(&data); break;
			case OPTIONS_MENU:
				draw_options_menu(&data); break;
			case GAME:
				game.update(); break;
			default:
				TraceLog(LOG_INFO, "main.c: Invalid option for state\n");
		}
	}

	CloseWindow();
	CloseAudioDevice();
	return (0);
}

void	draw_main_menu(GameData *data)
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
				data->state = PLAY_MENU;
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
				data->state = OPTIONS_MENU;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);

		center.y += text_size.y;
		center.y += 5; // Add padding
	}

	EndDrawing();
}

void	draw_play_menu(GameData *data) 
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
				data->state = GAME;
				data->previous_state = PLAY_MENU;
				game = games_lst[SNAKE_GAME];
			}
		}
		DrawRectangleRec(rect, BLUE);
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
				data->state = MAIN_MENU;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);
		center.y += text_size.y;
		center.y += 5; // Add padding
	}
	EndDrawing();
}

void	draw_options_menu(GameData *data)
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
				data->state = data->previous_state;
			}
		}
		DrawTextEx(GetFontDefault(), text, offset, 35, 3, text_color);
	}
	EndDrawing();
}
