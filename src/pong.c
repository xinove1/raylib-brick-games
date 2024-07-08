#include "game.h"

static void move_paddle(V2 dir, V2 *paddle);

static GameData	*Data = NULL;
static V2	RightPaddle = {};
static V2	LeftPaddle = {};
static V2	PaddleSize = {};
static V2	BallPos = {};
static V2	BallSize = {};
static V2	BallDir = {};
static float	BallSpeed = {};
static bool	GameOver = false;


static void	start()
{
	float	padding = 10;

	RightPaddle = (V2) { padding, Data->window_size.y * 0.5f };
	LeftPaddle = (V2) { Data->window_size.x - padding, Data->window_size.y * 0.5f };
	PaddleSize = (V2) {5, 25};

	BallPos = (V2) {Data->window_size.x * 0.5f, Data->window_size.y * 0.5f};
	BallSize = (V2) {5, 5};
	BallDir = (V2) {0, 0};
	BallSpeed = 100;
}

static void	de_init()
{
}

static void	update()
{
	if (GameOver) {

		return ;
	}
	if (IsActionDown(DOWN)) {
		move_paddle((V2) {0, 1}, &RightPaddle);
		move_paddle((V2) {0, -1}, &LeftPaddle);
	}
	if (IsActionDown(UP)) {
		move_paddle((V2) {0, -1}, &RightPaddle);
		move_paddle((V2) {0, 1}, &LeftPaddle);
	}

	if (IsActionPressed(ACTION_1)) {
		BallDir.x = 1;
	}
	
	BallPos = Vector2Add(BallPos, Vector2Scale(BallDir, BallSpeed * GetFrameTime()));
	{
		Rect	ball_rect = {BallPos.x , BallPos.y, BallSize.x, BallSize.y};
		Rect	right_rect = {RightPaddle.x, RightPaddle.y, PaddleSize.x, PaddleSize.y};
		Rect	left_rect = {LeftPaddle.x, LeftPaddle.y, PaddleSize.x, PaddleSize.y};

		if (CheckCollisionRecs(ball_rect, right_rect)) {
			BallDir.x = 1;
			BallDir.y = -BallDir.y;
		} else if (CheckCollisionRecs(ball_rect, left_rect)) {
			BallDir.x = -1;
			BallDir.y = -BallDir.y;
		}
	}
	
	if (BallPos.x > Data->window_size.x || BallPos.x < 0) {
		GameOver = true;
	}
}

static void	draw()
{
	DrawRectangleV(RightPaddle, PaddleSize, RED);
	DrawRectangleV(LeftPaddle, PaddleSize, RED);
	DrawRectangleV(BallPos, BallSize, RED);

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
}

GameFunctions	pong_init(GameData *data)
{
	Data = data;

	return (GameFunctions) { 
		.name = "Pong",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

static void move_paddle(V2 dir, V2 *paddle) 
{
	if ( !(paddle->y + dir.y + PaddleSize.y > Data->window_size.y || paddle->y + dir.y < 0)) {
		paddle->y += dir.y;
	}
}

static void resolve_ball_collision(V2 *paddle)
{
	BallDir.x = -BallDir.x;
	BallDir.y = -BallDir.x;
}
