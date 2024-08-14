#include "game.h"
#include "collision.h"

#define MAX_BRICKS 100

typedef enum {
	SPEED_SELF,
	SPEED_OTHER,
	DESTROY_SELF,
	DESTROY_OTHER,
} CollResolution_e ;

typedef struct {
	V2 pos;
	V2 size;
	V2 dir;
	f32 speed;
	Color color;
	CollResolution_e collision;
} Object;

static GameData *Data = NULL;
static b32 GameOver = false;
static b32 GamePaused = false;
static b32 PlayScreen = true;

static V2 BoardSize = { 100, 200 };
static V2 BoardOffset = { 0, 0};
static Object Bricks[MAX_BRICKS] = {};
static Object Paddle = {};
static Object Ball = {};
static UiContainer Container;

static void draw_game();
static b32 CollideBallWithRect(Rect rect);

static void start()
{
	GameOver = false;
	GamePaused = false;
	PlayScreen = true;

	BoardOffset = (V2){ Data->window_size.x * 0.5f - BoardSize.x * 0.5f,  Data->window_size.y * 0.5f - BoardSize.y * 0.5f };
	
	Paddle = (Object) {
		.pos  = (V2) { BoardSize.x * 0.5f, BoardSize.y - 10 },
		.size = (V2) { 25, 5 },
		.dir  = (V2) { 0, 0 },
		.speed = 80,
		.color = Data->palette.red,
	};

	Ball = (Object) {
		.pos  = (V2) { Paddle.pos.x, Paddle.pos.y - 10},
		.size = (V2) { 5, 5 },
		.dir  = (V2) { 1, 1 },
		.speed = 100,
		.color = Data->palette.blue,
	};

	memset(Bricks, -1, sizeof(Bricks));
	{
		f32 padding = 2; // Padding between bricks
		f32 padding_sides = padding * 2; // Padding between bricks an corner of the board
		i32 collumns = 5; // How Many bricks per line do we want
		
		V2 brick_size = {((BoardSize.x - (padding_sides * 2)) / collumns) - padding, 5};
		i32 lines = (BoardSize.y * 0.3f) / (brick_size.y + padding * 2);

		printf("qty_line * lines: %d\n", collumns * lines);
		printf("brick_size: %f, %f\n", brick_size.x, brick_size.y);
		assert(collumns * lines < MAX_BRICKS);

		float at_y = padding_sides;
		for (i32 row = 0; row < lines; row++) {
			float at_x = padding_sides;
			for (i32 col = 0; col < collumns; col++) {
				Object *brick = &Bricks[(row * collumns) + col];
				*brick = (Object) {
					.pos = (V2) {at_x, at_y},
					.size = brick_size,
					.dir = V2Zero(),
					.speed = 0,
					.collision = DESTROY_SELF,
					.color = Data->palette.pink,
				};
				at_x += padding + brick_size.x;
			}
			at_y += padding + brick_size.y;
		}
	}
}

static void de_init()
{
}

static void update()
{
	if (!ShouldGameRun(&PlayScreen , &GamePaused, &PlayScreen)) {
		return ;
	}

	if (IsActionDown(RIGHT)) {
		Paddle.dir = (V2) {1, 0};
	} else if (IsActionDown(LEFT)) {
		Paddle.dir = (V2) {-1, 0};
	} else {
		Paddle.dir = (V2) {0, 0};
	}

	{
		Paddle.pos = V2Add(Paddle.pos, V2Scale(Paddle.dir, Paddle.speed * GetFrameTime()));

		if (Paddle.pos.x <= 0) {
			Paddle.pos.x = 0;
		} else if ((Paddle.pos.x + Paddle.size.x) >= BoardSize.x) {
			Paddle.pos.x = BoardSize.x - Paddle.size.x;
		}
	}
	
	{
		Ball.pos = V2Add(Ball.pos, V2Scale(Ball.dir, Ball.speed * GetFrameTime()));

		Rect ball = RectV2(Ball.pos, Ball.size);
		Rect paddle = RectV2(Paddle.pos, Paddle.size);

		// Check ball collision with bricks
		for (i32 i = 0; i < MAX_BRICKS; i++) {
			Object *brick = &Bricks[i];
			if (brick->size.x == -1 && brick->size.y == -1) break ;
			if (brick->size.x == 0 && brick->size.y == 0) continue ;
			if (CollideBallWithRect(RectV2(brick->pos, brick->size))) {
				brick->size.x = 0; brick->size.y = 0;
			}
		}

		// Check ball Collision with paddle
		if (CollideBallWithRect(paddle)) {
			Ball.dir.y = -Ball.dir.y;
			if (Paddle.dir.x != 0) {
				Ball.dir.x = Paddle.dir.x;
			}
		} // Check ball Collision with board border
		else if (ball.x <= 0 || (ball.x + ball.width) >= BoardSize.x) {
			Ball.dir.x = -Ball.dir.x;
		} else if (ball.y <= 0) {
			Ball.dir.x = -Ball.dir.x;
			Ball.dir.y = -Ball.dir.y;
		} else if ((ball.y + ball.height) >= BoardSize.y){
			printf("death \n");
			start();
		}
	}
}

static void draw()
{
	draw_game();
	// Ui Screens
	if (PlayScreen) {
		UiContainer *panel = &Container;

		UiBegin(panel);
		{
			UiText(panel, "BreakOut", true);

			if (UiTextButton(panel, "Play")) { 
				PlayScreen = false;
			}

			// char	*mode = easy_mode ? "Mode: Easy" : "Mode: Normal";
			// if (panel_text_button(&panel, mode, small)) { 
			// 	easy_mode = easy_mode ? false : true;
			// }

			if (UiTextButton(panel, "Back")) { 
				Data->current_game = MAIN_MENU;
			}
		}
		UiEnd(panel);

		if (IsActionPressed(ACTION_2)) {
			Data->current_game = MAIN_MENU;
		}
	}
	if (GameOver) {
		UiStates state = game_over_screen(Data);
		if (state == NONE) {
			GameOver = false;
			start();
		} else if (state == TITLE_SCREEN) {
			Data->current_game = MAIN_MENU;
			GameOver = false;
		}
	}
}

GameFunctions breakout_init(GameData *data)
{
	Data = data;

	V2 center_screen = {data->window_size.x * 0.5f, data->window_size.y * 0.25f}; // Center offset to where to start drawing text
	Container = UiCreateContainer(center_screen, 0, data->ui_config);

	return (GameFunctions) { 
		.name = "BreakOut",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

static void draw_game()
{
	DrawRectangleV(BoardOffset, BoardSize, Data->palette.black);
	DrawRectangleV(V2Add(Ball.pos, BoardOffset), Ball.size, Ball.color);
	DrawRectangleV(V2Add(Paddle.pos, BoardOffset), Paddle.size, Paddle.color);
	//DrawRectangleLinesEx(RectV2(BoardOffset, BoardSize), 1, Data->palette.green);

	for (i32 i = 0; i < MAX_BRICKS; i++) {
		Object brick = Bricks[i];
		if (brick.size.x == -1 && brick.size.y == -1) break ;
		if (brick.size.x == 0 && brick.size.y == 0) continue ;
		DrawRectangleV(V2Add(brick.pos, BoardOffset), brick.size, brick.color);
	}
}

static b32 CollideBallWithRect(Rect rec) 
{
	b32 collided = false;

	if (CheckCollisionRecs(RectV2(Ball.pos, Ball.size), rec)) {
		collided = true;

		V2 rect_center = {rec.x * 0.5f, rec.y * 0.5f};
		V2 collision_normal = V2Normalize(V2Subtract(Ball.pos, rect_center));
		Ball.dir = (V2) {collision_normal.x, collision_normal.y};
	}

	return (collided);
}

static b32 CollideObjectObject(Object a, Object b) 
{
	b32 collided = false;

	// V2	contact_point, contact_normal;
	// float	time;
	// float	dt = GetFrameTime();
	// if (CheckCollisionDynamicRectRect(RectV2(a.pos, a.size), a.vel, RectV2(b.pos, b.size), &contact_point, &contact_normal, &time, dt))
	// {
	// 	Vector2	vel_abs = {fabsf(a.vel.x), fabsf(a.vel.y)};
	// 	a.vel = Vector2Add(a.vel, Vector2Multiply(contact_normal, Vector2Scale(vel_abs, 1 - time)));
	// 	DrawCircleV(contact_point, 4, RED);
	// }
	return (collided);
}
