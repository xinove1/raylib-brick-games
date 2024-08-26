#include "game.h"

internal void move_paddle(V2 dir, V2 *paddle);
internal void de_init();
internal void start();
internal void update();
internal void draw();

struct PongData {
	b32 play_screen;
	b32 paused;
	b32 options;
	b32 game_over;
	b32 won;
	V2 right_paddle;
	V2 left_paddle;
	V2 paddle_size;
	V2 ball_pos;
	V2 ball_size;
	V2 ball_dir;
	f32 ball_speed;
	UiContainer container;
};

global GameData *Data = NULL;
global PongData *Pong = NULL;

GameFunctions pong_init(GameData *data)
{
	Data = data;
	data->pong_data = calloc(1, sizeof(PongData));
	Pong = data->pong_data;

	*Pong = (PongData) {
		.play_screen = true,
	};

	V2 center_screen = {data->window_size.x * 0.5f, data->window_size.y * 0.25f}; // Center offset to where to start drawing text
	Pong->container = UiCreateContainer(center_screen, 0, data->ui_config);

	return (GameFunctions) { 
		.name = "Pong",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

internal void de_init()
{
	free(Pong);
}

internal void start()
{
	f32 padding = 10;

	Pong->right_paddle = (V2) { padding, Data->window_size.y * 0.5f };
	Pong->left_paddle = (V2) { Data->window_size.x - padding, Data->window_size.y * 0.5f };
	Pong->paddle_size = (V2) {5, 25};

	Pong->ball_pos = (V2) {Data->window_size.x * 0.5f, Data->window_size.y * 0.5f};
	Pong->ball_size = (V2) {5, 5};
	Pong->ball_dir = (V2) {0, 0};
	Pong->ball_speed = 100;
}

internal void update()
{
	if (!ShouldGameRun(&Pong->play_screen, &Pong->paused, &Pong->game_over)) {
		return ;
	}

	if (IsActionDown(DOWN)) {
		move_paddle((V2) {0, 1}, &Pong->right_paddle);
		move_paddle((V2) {0, -1}, &Pong->left_paddle);
	}
	if (IsActionDown(UP)) {
		move_paddle((V2) {0, -1}, &Pong->right_paddle);
		move_paddle((V2) {0, 1}, &Pong->left_paddle);
	}

	if (IsActionPressed(ACTION_1)) {
		Pong->ball_dir.x = 1;
	}
	
	Pong->ball_pos = V2Add(Pong->ball_pos, V2Scale(Pong->ball_dir, Pong->ball_speed * GetFrameTime()));
	{
		Rect ball_rect = {Pong->ball_pos.x , Pong->ball_pos.y, Pong->ball_size.x, Pong->ball_size.y};
		Rect right_rect = {Pong->right_paddle.x, Pong->right_paddle.y, Pong->paddle_size.x, Pong->paddle_size.y};
		Rect left_rect = {Pong->left_paddle.x, Pong->left_paddle.y, Pong->paddle_size.x, Pong->paddle_size.y};

		if (CheckCollisionRecs(ball_rect, right_rect)) {
			Pong->ball_dir.x = 1;
			Pong->ball_dir.y = -Pong->ball_dir.y;
		} else if (CheckCollisionRecs(ball_rect, left_rect)) {
			Pong->ball_dir.x = -1;
			Pong->ball_dir.y = -Pong->ball_dir.y;
		}
	}
	
	if (Pong->ball_pos.x > Data->window_size.x || Pong->ball_pos.x < 0) {
		Pong->game_over = true;
	}
}

internal void draw()
{
	DrawRectangleV(Pong->right_paddle, Pong->paddle_size, RED);
	DrawRectangleV(Pong->left_paddle, Pong->paddle_size, RED);
	DrawRectangleV(Pong->ball_pos, Pong->ball_size, RED);

	// Ui Screens
	if (Pong->play_screen) {
		UiContainer *panel = &Pong->container;

		UiBegin(panel);
		{
			UiText(panel, "Pong", true);

			if (UiTextButton(panel, "Play")) { 
				start();
				Pong->play_screen = false;
			}

			if (UiTextButton(panel, "Back")) { 
				Data->current_game = MAIN_MENU;
			}
		}
		UiEnd(panel);

		if (IsActionPressed(ACTION_2)) {
			Data->current_game = MAIN_MENU;
		}
	}

	if (Pong->won) {
		UiContainer *panel = &Pong->container;
		UiBegin(panel);
		{
			UiText(panel, "You Won!!!", true);

			if (UiTextButton(panel, "Play Again")) { 
				Pong->play_screen = true;
				Pong->won = false;
			} 

			if (UiTextButton(panel, "Exit To Main Menu")) { 
				Data->current_game = MAIN_MENU;
				Pong->play_screen = true;
				Pong->won = false;
			}

			if (UiTextButton(panel, "Exit To Desktop")) { 
				Data->quit = true;
			}
		}
		UiEnd(panel);
	}

	if (Pong->paused && Pong->options == false) {
		UiContainer *panel = &Pong->container;
		UiBegin(panel);
		{
			UiText(panel, "Game Paused", true);

			if (UiTextButton(panel, "Back to Game")) { 
				Pong->paused = false;
			} 

			if (UiTextButton(panel, "Options")) { 
				Pong->options = true;
			}

			if (UiTextButton(panel, "Exit To Main Menu")) { 
				Data->current_game = MAIN_MENU;
				Pong->paused = false;
				Pong->play_screen = true;
			}

			if (UiTextButton(panel, "Exit To Desktop")) { 
				Data->quit = true;
			}
		}
		UiEnd(panel);
		if (IsActionPressed(ACTION_2)) {
			Pong->paused = false;
		}

	} else if (Pong->paused && Pong->options) {
		UiStates state = options_screen(Data);
		if (state == BACK) {
			Pong->options = false;
		}
	}

	if (Pong->game_over) {
		UiStates state = game_over_screen(Data);
		if (state == NONE) {
			Pong->game_over = false;
			Pong->play_screen = true;
		} else if (state == TITLE_SCREEN) {
			Data->current_game = MAIN_MENU;
			Pong->game_over = false;
		}
	}
}

internal void move_paddle(V2 dir, V2 *paddle) 
{
	if ( !(paddle->y + dir.y + Pong->paddle_size.y > Data->window_size.y || paddle->y + dir.y < 0)) {
		paddle->y += dir.y;
	}
}

internal void resolve_ball_collision(V2 *paddle)
{
	Pong->ball_dir.x = -Pong->ball_dir.x;
	Pong->ball_dir.y = -Pong->ball_dir.x;
}
