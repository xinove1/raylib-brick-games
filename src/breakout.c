#include "game.h"
#include "collision.h"

#define MAX_BRICKS 100

typedef enum {
	SPEED_SELF,
	SPEED_OTHER,
	DESTROY_SELF,
	DESTROY_OTHER,
} CollResolution_e ;

typedef enum {
	ObjectEmpty,
	ObjectBall,
	ObjectPaddle,
	ObjectBrick,
	ObjectCount,
} ObjectType_e;

typedef struct {
	ObjectType_e type;
	V2 pos;
	V2 size;
	V2 dir;
	f32 speed;
	Color color;
	CollResolution_e collision;
} Object;


struct BreakoutData {
	b32 game_over;
	b32 game_paused;
	b32 play_screen;
	b32 options_screen;
	b32 game_won;
	V2 board_size;
	V2 board_offset;
	Object bricks[MAX_BRICKS];
	i32    bricks_count;
	Object paddle;
	Object ball;
	UiContainer container;
};

internal b32 CollideBallWithRect(Rect rect);
internal void draw_game();
internal void de_init();
internal void start();
internal void update();
internal void draw();

global GameData *Data = NULL;
global BreakoutData *Break = NULL;

GameFunctions breakout_init(GameData *data)
{
	Data = data;

	data->breakout_data = calloc(1, sizeof(BreakoutData));
	Break = data->breakout_data;
	
	*Break = (BreakoutData) {
		.game_over = false,
		.game_paused = false,
		.play_screen = true,
		.board_size = { 100, 200 },
	};

	V2 center_screen = {data->window_size.x * 0.5f, data->window_size.y * 0.25f}; // Center offset to where to start drawing text
	Break->container = UiCreateContainer(center_screen, 0, data->ui_config);

	return (GameFunctions) { 
		.name = "BreakOut",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

internal void de_init()
{
}

internal void start()
{
	Break->game_over = false;
	Break->game_paused = false;
	Break->play_screen = true;

	Break->board_offset = (V2){ Data->window_size.x * 0.5f - Break->board_size.x * 0.5f,  Data->window_size.y * 0.5f - Break->board_size.y * 0.5f };
	
	Break->paddle = (Object) {
		.type = ObjectPaddle,
		.pos  = (V2) { Break->board_size.x * 0.5f, Break->board_size.y - 10 },
		.size = (V2) { 25, 5 },
		.dir  = (V2) { 0, 0 },
		.speed = 80,
		.color = Data->palette.red,
	};

	Break->ball = (Object) {
		.type = ObjectBall,
		.pos  = (V2) { Break->paddle.pos.x, Break->paddle.pos.y - 10},
		.size = (V2) { 5, 5 },
		.dir  = (V2) { 1, 1 },
		.speed = 100,
		.color = Data->palette.blue,
	};

	memset(Break->bricks, 0, sizeof(Break->bricks));

	{
		f32 padding = 2; // Padding between bricks
		f32 padding_sides = padding * 2; // Padding between bricks an corner of the board
		i32 collumns = 5; // How Many bricks per line do we want
		
		V2 brick_size = {((Break->board_size.x - (padding_sides * 2)) / collumns) - padding, 5};
		i32 lines = (Break->board_size.y * 0.3f) / (brick_size.y + padding * 2);

		printf("qty_line * lines: %d\n", collumns * lines);
		printf("brick_size: %f, %f\n", brick_size.x, brick_size.y);
		Assert(collumns * lines < MAX_BRICKS);
		Break->bricks_count = collumns * lines;

		float at_y = padding_sides;
		for (i32 row = 0; row < lines; row++) {
			float at_x = padding_sides;
			for (i32 col = 0; col < collumns; col++) {
				Object *brick = &Break->bricks[(row * collumns) + col];
				*brick = (Object) {
					.type = ObjectBrick,
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

internal void update()
{
	if (!ShouldGameRun(&Break->play_screen , &Break->game_paused, &Break->game_over)) {
		return ;
	}
	if (Break->game_won) {
		printf("You Won! \n");
		return ;
	}

	Object *paddle = &Break->paddle;
	Object *ball = &Break->ball;

	if (IsActionDown(RIGHT)) {
		paddle->dir = (V2) {1, 0};
	} else if (IsActionDown(LEFT)) {
		paddle->dir = (V2) {-1, 0};
	} else {
		paddle->dir = (V2) {0, 0};
	}

	#ifdef BUILD_DEBUG
		if (IsActionPressed(ACTION_1)) {
			ball->dir = (V2) {0, -1};
		}
	#endif

	{
		paddle->pos = V2Add(paddle->pos, V2Scale(paddle->dir, paddle->speed * GetFrameTime()));

		if (paddle->pos.x <= 0) {
			paddle->pos.x = 0;
		} else if ((paddle->pos.x + paddle->size.x) >= Break->board_size.x) {
			paddle->pos.x = Break->board_size.x - paddle->size.x;
		}
	}
	
	{
		ball->pos = V2Add(ball->pos, V2Scale(ball->dir, ball->speed * GetFrameTime()));

		Rect ball_rec = RectV2(ball->pos, ball->size);
		Rect paddle_rec = RectV2(paddle->pos, paddle->size);

		b32 at_least_one_brick = false;
		// Check ball collision with bricks
		for (i32 i = 0; i < Break->bricks_count; i++) {
			Object *brick = &Break->bricks[i];
			if (brick->type == ObjectEmpty) continue ;

			at_least_one_brick = true;
			if (CollideBallWithRect(RectV2(brick->pos, brick->size))) {
				brick->type = ObjectEmpty;
				PlaySound(Data->assets.sounds[SoundIceBreak]);
			}
		}

		if (!at_least_one_brick) { Break->game_won = true; };

		// Check ball Collision with paddle
		if (CollideBallWithRect(paddle_rec)) {
			ball->dir.y = -ball->dir.y;
			if (paddle->dir.x != 0) {
				ball->dir.x = paddle->dir.x;
			}
			PlaySound(Data->assets.sounds[SoundImpactMetal]);
		} 

		// Check ball Collision with board border
		if (ball_rec.x <= 0 || (ball_rec.x + ball_rec.width) >= Break->board_size.x) {
			ball->dir.x = -ball->dir.x;
			PlaySound(Data->assets.sounds[SoundImpactGlass]);
		} else if (ball_rec.y <= 0) {
			ball->dir.x = -ball->dir.x;
			ball->dir.y = -ball->dir.y;
			PlaySound(Data->assets.sounds[SoundImpactGlass]);
		} else if ((ball_rec.y + ball_rec.height) >= Break->board_size.y){
			Break->game_over = true;
		}
	}
}

internal void draw()
{
	draw_game();
	// Ui Screens
	if (Break->play_screen) {
		UiContainer *panel = &Break->container;

		UiBegin(panel);
		{
			UiText(panel, "BreakOut", true);

			if (UiTextButton(panel, "Play")) { 
				start();
				Break->play_screen = false;
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

	if (Break->game_won) {
		UiContainer *panel = &Break->container;
		UiBegin(panel);
		{
			UiText(panel, "You Won!!!", true);

			if (UiTextButton(panel, "Play Again")) { 
				Break->play_screen = true;
				Break->game_won = false;
			} 

			if (UiTextButton(panel, "Exit To Main Menu")) { 
				Data->current_game = MAIN_MENU;
				Break->play_screen = true;
				Break->game_won = false;
			}

			if (UiTextButton(panel, "Exit To Desktop")) { 
				Data->quit = true;
			}
		}
		UiEnd(panel);
	}

	if (Break->game_paused && Break->options_screen == false) {
		UiContainer *panel = &Break->container;
		UiBegin(panel);
		{
			UiText(panel, "Game Paused", true);

			if (UiTextButton(panel, "Back to Game")) { 
				Break->game_paused = false;
			} 

			if (UiTextButton(panel, "Options")) { 
				Break->options_screen = true;
			}

			if (UiTextButton(panel, "Exit To Main Menu")) { 
				Data->current_game = MAIN_MENU;
				Break->game_paused = false;
				Break->play_screen = true;
			}

			if (UiTextButton(panel, "Exit To Desktop")) { 
				Data->quit = true;
			}
		}
		UiEnd(panel);
		if (IsActionPressed(ACTION_2)) {
			Break->game_paused = false;
		}

	} else if (Break->game_paused && Break->options_screen) {
		UiStates state = options_screen(Data);
		if (state == BACK) {
			Break->options_screen = false;
		}
	}

	if (Break->game_over) {
		UiStates state = game_over_screen(Data);
		if (state == NONE) {
			Break->game_over = false;
			Break->play_screen = true;
		} else if (state == TITLE_SCREEN) {
			Data->current_game = MAIN_MENU;
			Break->game_over = false;
		}
	}
}

internal void draw_game()
{
	DrawRectangleV(Break->board_offset, Break->board_size, Data->palette.black);
	DrawRectangleV(V2Add(Break->ball.pos, Break->board_offset), Break->ball.size, Break->ball.color);
	DrawRectangleV(V2Add(Break->paddle.pos, Break->board_offset), Break->paddle.size, Break->paddle.color);
	//DrawRectangleLinesEx(RectV2(board_offset, board_size), 1, Data->palette.green);

	for (i32 i = 0; i < Break->bricks_count; i++) {
		Object brick = Break->bricks[i];
		if (brick.type == ObjectEmpty) continue ;
		DrawRectangleV(V2Add(brick.pos, Break->board_offset), brick.size, brick.color);
	}
}

// TODO  More generic CollideObject with rect or other Object and resolve based on collision resolution type
internal b32 CollideBallWithRect(Rect rec) 
{
	b32 collided = false;

	if (CheckCollisionRecs(RectV2(Break->ball.pos, Break->ball.size), rec)) {
		collided = true;

		V2 rect_center = {rec.x * 0.5f, rec.y * 0.5f};
		V2 collision_normal = V2Normalize(V2Subtract(Break->ball.pos, rect_center));
		Break->ball.dir = (V2) {collision_normal.x, collision_normal.y};
	}

	return (collided);
}

internal b32 CollideObjectObject(Object a, Object b) 
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
