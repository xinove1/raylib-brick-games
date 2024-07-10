#include "game.h"
#include "raylib.h"
#include "collision.h"

static GameData	*Data = NULL;
static bool	GameOver = false;
static bool	GamePaused = false;
static bool	PlayScreen = true;

static V2	Dir = { 0 };

static void	start()
{
	GameOver = false;
	GamePaused = false;
	PlayScreen = true;
}

static void	update()
{
	if (!ShouldGameRun(&PlayScreen , &GamePaused, &PlayScreen)) {
		return ;
	}

	if (IsActionPressed(RIGHT)) {
		Dir.x = 1;
	}
	if (IsActionPressed(LEFT)) {
		Dir.x = -1;
	}
	if (IsActionPressed(UP)) {
		Dir.y = -1;
	}
	if (IsActionPressed(DOWN)) {
		Dir.y = 1;
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
	float	time;
	float	speed = 50;
	V2	point, normal  = { 0 };
	V2	origin = {0, 0};
	static Rect	p = {50, 50, 10, 15};
	static Rect	e = {50, 100, 50, 15};
	static V2	vel = {0, 0};
	vel = (V2) { Dir.x * speed, Dir.y * speed};

	DrawRectangleRec(p, RED);
	DrawRectangleRec(e, GREEN);

	if (CheckCollisionDynamicRectRect(p, vel, e, &point, &normal, &time, GetFrameTime()) && time < 1) {
		DrawCircleV(point, 1, YELLOW);
		DrawLineV(point, Vector2Add(point, Vector2Scale(normal, 5)), BLUE);
		Vector2	vel_abs = {fabsf(vel.x), fabsf(vel.y)};
		vel = Vector2Add(vel, Vector2Multiply(normal, Vector2Scale(vel_abs, 1 - time)));
	}

	V2	mouse_pos = GetMousePosition();
	V2	mouse_dir = Vector2Normalize(Vector2Subtract(mouse_pos, V2RectPos(p)));
	if (CheckCollisionRayRec(V2RectPos(p), mouse_dir, e, &point, &normal, &time)) {
		DrawCircleV(point, 2, YELLOW);
		DrawLineV(point, Vector2Add(point, Vector2Scale(normal, 5)), BLUE);
	}
	DrawLineV(V2RectPos(p), mouse_pos, GREEN);

	p.x += vel.x * GetFrameTime();
	p.y += vel.y * GetFrameTime();

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
