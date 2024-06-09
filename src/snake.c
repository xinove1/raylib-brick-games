#include "game.h"
#include "raylib.h"

# define MAX_APPLES 10
# define MAX_SNAKE_SIZE ((board_size.x - 1) * (board_size.y - 1))

static V2	board_size = {15, 15};
static int	tile_size = TILE_SIZE;
static void	draw();
static void	draw_game();
static int	check_collision(Vector2	pos);
static void	move_snake_body(Vector2 new_head_pos, int add_body);
static void	spawn_apple();

static V2	board_offset;
static V2	*snake;
static V2	dir = {1, 0};
static V2	new_dir = {1, 0};
static V2	apples[MAX_APPLES] = {0};
static float	tick_time_count = 0;
static const float	tick_rate = 0.150; // How much time until a game tick in seconds
static float	apple_spawn_rate = 5; // in seconds
static bool	game_over = false;
static bool	easy_mode = true;

GameData *data = 0;

static void	start()
{
	memset(snake, 0, MAX_SNAKE_SIZE);
	memset(apples, 0, MAX_APPLES);
	snake[0] = (V2) {(int) (board_size.x * 0.5f), (int) (board_size.y * 0.5f)}; // Set snake head to middle of the board
	game_over = false;
	tick_time_count = 0;
	dir = (V2) {1,0};
	new_dir = (V2) {1,0};
	spawn_apple();
}

static void update()
{
	if (game_over) {
		data->current_ui = GAME_OVER_MENU;
		game_over = false;
		start(); // Reset game state
		return ;
	}

	if (IsActionPressed(RIGHT)) {
		new_dir.x = 1;
		new_dir.y = 0;
	}
	if (IsActionPressed(LEFT)) {
		new_dir.x = -1;
		new_dir.y = 0;
	}
	if (IsActionPressed(UP)) {
		new_dir.x = 0;
		new_dir.y = -1;
	}
	if (IsActionPressed(DOWN)) {
		new_dir.x = 0;
		new_dir.y = 1;
	}

	// TODO  Implement easy mode (not collide with walls)
	
	
	tick_time_count += GetFrameTime();
	if (tick_time_count >= tick_rate) {
		tick_time_count = 0;
		// Check to see if player is not tryng to 
		if (snake[1].x == 0 || -new_dir.x != dir.x || -new_dir.y != dir.y ) {
			dir = new_dir;
		}

		V2	new_pos = Vector2Add(snake[0], dir);

		if (easy_mode) {
			if (new_pos.x == 0) {
				new_pos.x = board_size.x - 1;
			} else if (new_pos.x == board_size.x) {
				new_pos.x = 1;
			}
			if (new_pos.y == 0) {
				new_pos.y = board_size.y - 1;
			} else if (new_pos.y == board_size.y) {
				new_pos.y = 1;
			}
		}

		int	collision = check_collision(new_pos);
		if (collision == 1) {
			game_over = true;
			return ;
		}
		if (collision == 2 || (int)tick_time_count % (int)apple_spawn_rate == 0) {
			spawn_apple();
		}
		move_snake_body(new_pos, collision);
		if (snake[(int)MAX_SNAKE_SIZE - 1].x != 0) {
			printf("YOU WIN!!/n");
			game_over = true;
			// TODO draw win screen
		}
	}
}

static void	draw() 
{
	ClearBackground(RAYWHITE);
	draw_game();
}

void	de_init() {
	free(snake);
}

GameFunctions	snake_game_init(GameData *game_data)
{
	data = game_data;

	snake = calloc(MAX_SNAKE_SIZE, sizeof(Vector2));
	board_offset =  (V2) {
		.x = data->window_size.x * 0.5f - (board_size.x * tile_size) * 0.5f,
		.y = data->window_size.y * 0.5f - (board_size.y * tile_size) * 0.5f
	};

	return (GameFunctions) { 
		.name = "Snake Game",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

static void	move_snake_body(Vector2 new_head_pos, int add_body)
{
	int i;
	Vector2	tmp = snake[0];
	for (i = 1; i < MAX_SNAKE_SIZE; i++) {
		if (snake[i].x == 0) // Assuming snake is initialised with 0's and that pos 0,0 is inside board and you can't reach this part of the code being there
			break ;
		Vector2	tmp2 = snake[i];
		snake[i] = tmp;
		tmp = tmp2;
	}
	assert(i != MAX_SNAKE_SIZE);
	if (add_body) {
		//Vector2	_dir = Vector2Subtract(snake[i - 1], snake[i - 2]);
		//printf("dir: %f,%f\n", _dir.x, _dir.y);
		snake[i] = tmp;
	}
	snake[0] = new_head_pos;
}

static void	spawn_apple()
{
	Vector2	pos;
	do {
		pos = (V2) {GetRandomValue(1, board_size.x - 1), GetRandomValue(1, board_size.y - 1)};
	} while (check_collision(pos));

	for (int i = 0; i < MAX_APPLES; i++) {
		if (apples[i].x == 0) {
			apples[i] = pos;
			break ;
		}
	}
}

static int	check_collision(Vector2	pos)
{
	// Check collision with game board
	if (!easy_mode && (pos.x == 0 || pos.x == board_size.x || pos.y == 0 || pos.y == board_size.y)) {
		return (1);
	}

	for (int i = 1; i < MAX_SNAKE_SIZE; i++) {
		if (snake[i].x == 0)
			break ;
		if (pos.x == snake[i].x && pos.y == snake[i].y) {
			return (1); // Colliding
		}
	}

	for (int i = 0; i < MAX_APPLES; i++) {
		if (apples[i].x == 0)
			continue;
		if (pos.x == apples[i].x && pos.y == apples[i].y) {
			apples[i] = (Vector2) {0,0};
			return (2); // Colliding with apple
		}
	}
	return (0);
}

static void	draw_game()
{
	ColorPalette	palette = data->palette;

	// Grid
	draw_grid((V2){board_offset.x + tile_size, board_offset.y + tile_size}, (V2){board_size.x -1, board_size.y -1}, tile_size);

	// Snake
	for (int i = 0; i < MAX_SNAKE_SIZE; i++) {
		if (snake[i].x == 0)
			break ;
		Vector2	pos = Vector2Scale(snake[i], tile_size);
		pos = Vector2Add(pos, board_offset);
		DrawRectangle(pos.x, pos.y, tile_size, tile_size, palette.green);
		// Head Eyes
		if (i == 0) {
			float	offset = tile_size * 0.35f;
			float	size = 2.5;
			V2	v1 = {pos.x + offset - size, pos.y + offset - size}; // upper left
			V2	v2 = {(pos.x + tile_size) - offset, pos.y + offset - size}; // upper right
			V2	v3 = {pos.x + offset - size, (pos.y + tile_size) - offset}; // down left
			V2	v4 = {(pos.x + tile_size) - offset, (pos.y + tile_size) - offset}; // down right
			V2	eye1;
			V2	eye2;
			if (dir.x == 1) {
				eye1 = v2;
				eye2 = v4;
			}
			if (dir.x == -1) {
				eye1 = v1;
				eye2 = v3;
			}
			if (dir.y == 1) {
				eye1 = v3;
				eye2 = v4;
			}
			if (dir.y == -1) {
				eye1 = v1;
				eye2 = v2;
			}
			DrawRectangle(eye1.x, eye1.y, size, size, palette.black);
			DrawRectangle(eye2.x, eye2.y, size, size, palette.black);
		}
	}

	// Apples
	for (int i = 0; i < MAX_APPLES; i++) {
		if (apples[i].x == 0)
			continue;
		Vector2	pos = Vector2Scale(apples[i], tile_size);
		pos = Vector2Add(pos, board_offset);
		DrawRectangle(pos.x, pos.y, tile_size, tile_size, palette.red);
	}

	// Board border
	float	line_thickness = 5;
	Rect	board = {
		.x = board_offset.x + tile_size - line_thickness,
		.y = board_offset.y + tile_size - line_thickness,
		.width = board_size.x * (tile_size -1) + tile_size * 0.5f,
		.height= board_size.y * (tile_size -1) + tile_size * 0.5f
	};
	DrawRectangleLinesEx(board, line_thickness, palette.red);

	// for (int y = 0; y < board_size.y; y++) {
	// 	DrawRectangle(
	// 		board_offset.x,
	// 		(y * tile_size) + board_offset.y,
	// 		tile_size,
	// 		tile_size,
	// 		light_grey);
	// 	DrawRectangle(
	// 		(board_size.x * tile_size) + board_offset.x,
	// 		(y * tile_size) + board_offset.y,
	// 		tile_size, 
	// 		tile_size,
	// 		light_grey);
	// }
	// for (int x = 0; x < board_size.x + 1; x++) {
	// 	DrawRectangle(
	// 		board_offset.x + (x * tile_size),
	// 		board_offset.y,
	// 		tile_size,
	// 		tile_size,
	// 		light_grey);
	// 	DrawRectangle(
	// 		(x * tile_size) + board_offset.x,
	// 		(board_size.y * tile_size) + board_offset.y,
	// 		tile_size,
	// 		tile_size,
	// 		light_grey);
	// }
}
