#include "game.h"

# define TILE_SIZE 16
# define MAX_APPLES 10
# define MAX_SNAKE_SIZE ((board_size.x - 1) * (board_size.y - 1))

static V2	board_size = {15, 15};
static void	draw_game();
static int	check_collision(Vector2	pos);
static void	move_snake_body(Vector2 new_head_pos, int add_body);
static void	spawn_apple();

static V2	board_offset;
static V2	*snake;
static V2	dir = {1, 0};
static V2	new_dir = {0, 0};
static V2	apples[MAX_APPLES] = {0};
static float	frame_count = 0;
static float	tick_rate = 15; // How many frames until a game tick
static bool	game_over = false;
static bool	easy_mode = false;

GameData *data = 0;
void	draw();

static void	start()
{
	memset(snake, 0, MAX_SNAKE_SIZE);
	memset(apples, 0, MAX_APPLES);
	snake[0] = (V2) {(int) (board_size.x * 0.5f), (int) (board_size.y * 0.5f)}; // Set snake head to middle of the board
	game_over = false;
	frame_count = 0;
	dir = (V2) {1,0};
	spawn_apple();
}

static void update()
{
	frame_count++;

	if (game_over) {
		data->current_ui = GAME_OVER_MENU;
		game_over = false;
		return ;
	}

	if (IsActionPressed("right")) {
		new_dir.x = 1;
		new_dir.y = 0;
	}
	if (IsActionPressed("left")) {
		new_dir.x = -1;
		new_dir.y = 0;
	}
	if (IsActionPressed("up")) {
		new_dir.x = 0;
		new_dir.y = -1;
	}
	if (IsActionPressed("down")) {
		new_dir.x = 0;
		new_dir.y = 1;
	}
	
	// Check to see if player is not tryng to go 
	if (snake[1].x == 0 || !(-new_dir.x == dir.x || -new_dir.y == dir.y)) {
		dir = new_dir;
	}
	
	if ((int) frame_count % (int) tick_rate == 0) {
		int	collision = check_collision(Vector2Add(snake[0], dir));
		if (collision == 1) {
			game_over = true;
			return ;
		}
		if (collision == 2) {
			spawn_apple();
		}
		move_snake_body(Vector2Add(snake[0], dir), collision);
		if (snake[(int)MAX_SNAKE_SIZE - 1].x != 0) {
			printf("YOU WIN!!/n");
			game_over = true;
			// TODO draw win screen
		}
		new_dir = (V2){0, 0};
	}

	draw();
}

void	draw() 
{
	BeginDrawing();
	ClearBackground(RAYWHITE);
	draw_game();
	if (game_over) {
		if (IsActionPressed("action_1")) {
			game_over = false;
			start();
		}
		// if (game_over_screen(data)) {
		// 	game_over = false;
		// 	start();
		// }
	}
	EndDrawing();
}

void	de_init() {
	free(snake);
}

GameFunctions	snake_game_init(GameData *game_data)
{
	data = game_data;

	snake = calloc(MAX_SNAKE_SIZE, sizeof(Vector2));
	board_offset =  (V2) {
		.x = data->window_size.x * 0.5f - (board_size.x * TILE_SIZE) * 0.5f,
		.y = data->window_size.y * 0.5f - (board_size.y * TILE_SIZE) * 0.5f
	};

	return (GameFunctions) { 
		.name = "Snake Game",
		.update = &update,
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
	if (!easy_mode && pos.x == 0 || pos.x == board_size.x || pos.y == 0 || pos.y == board_size.y) {
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
	for (int i = 0; i < MAX_SNAKE_SIZE; i++) {
		if (snake[i].x == 0)
			break ;
		Vector2	pos = Vector2Scale(snake[i], TILE_SIZE);
		pos = Vector2Add(pos, board_offset);
		DrawRectangle(pos.x, pos.y, TILE_SIZE - 1, TILE_SIZE - 1, palette.green);
	}

	for (int i = 0; i < MAX_APPLES; i++) {
		if (apples[i].x == 0)
			continue;
		Vector2	pos = Vector2Scale(apples[i], TILE_SIZE);
		pos = Vector2Add(pos, board_offset);
		DrawRectangle(pos.x, pos.y, TILE_SIZE - 0.5, TILE_SIZE - 0.5, palette.red);
	}

	for (int y = 0; y < board_size.y; y++) {
		DrawRectangle(
			board_offset.x,
			(y * TILE_SIZE) + board_offset.y,
			TILE_SIZE / 2,
			TILE_SIZE / 2,
			palette.black);
		DrawRectangle(
			(board_size.x * TILE_SIZE) + board_offset.x,
			(y * TILE_SIZE) + board_offset.y,
			TILE_SIZE * 0.5f, 
			TILE_SIZE * 0.5f,
			palette.black);
	}
	for (int x = 0; x < board_size.x + 1; x++) {
		DrawRectangle(
			board_offset.x + (x * TILE_SIZE),
			board_offset.y,
			TILE_SIZE / 2,
			TILE_SIZE / 2,
			palette.black);
		DrawRectangle(
			(x * TILE_SIZE) + board_offset.x,
			(board_size.y * TILE_SIZE) + board_offset.y,
			TILE_SIZE / 2,
			TILE_SIZE / 2,
			palette.black);
	}
}
