#include "game.h"
#include <raylib.h>

# define GAME_BOARD_SIZE (Vector2){15, 15}
# define TILE_SIZE 16
# define MAX_APPLES 10
# define MAX_SNAKE_SIZE ((GAME_BOARD_SIZE.x - 1) * (GAME_BOARD_SIZE.y - 1))

static void	draw_game();
static int	check_collision(Vector2	pos);
static void	move_snake_body(Vector2 new_head_pos, int add_body);
static void	spawn_apple();

static Vector2	board_offset;
static Vector2	*snake;
static Vector2	dir = {0, 0};
static Vector2	apples[MAX_APPLES] = {10,10};

GameData *data = 0;

static void	start()
{
	// TODO  Reset board etc
	printf("hello start from snake\n");
}

static void update()
{
	if (IsActionPressed("right")) {
		dir.x = 1;
		dir.y = 0;
	}
	if (IsActionPressed("left")) {
		dir.x = -1;
		dir.y = 0;
	}
	if (IsActionPressed("up")) {
		dir.x = 0;
		dir.y = -1;
	}
	if (IsActionPressed("down")) {
		dir.x = 0;
		dir.y = 1;
	}

	if (dir.x != 0 || dir.y != 0) {
		int	collision = check_collision(Vector2Add(snake[0], dir));
		if (collision == 1) {
			printf("Game Over\n");
			data->quit = true;
			return ;
		}
		if (collision == 2) {
			spawn_apple();
		}
		move_snake_body(Vector2Add(snake[0], dir), collision);
		dir.x = 0;
		dir.y = 0;
	}

	BeginDrawing();
	ClearBackground(RAYWHITE);
	draw_game();
	EndDrawing();
//	printf("board_offset: %f,%f\n", board_offset.x, board_offset.y);
}

void	de_init() {
	free(snake);
}

GameFunctions	snake_game_init(GameData *game_data)
{
	data = game_data;

	snake = calloc(MAX_SNAKE_SIZE, sizeof(Vector2));
	snake[0] = (Vector2) {5,5};
	board_offset =  (V2) {data->window_size.x / 3.0, 10};;

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
	Vector2	pos = {GetRandomValue(1, GAME_BOARD_SIZE.x - 1), GetRandomValue(1, GAME_BOARD_SIZE.y - 1)};

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
	if (pos.x == 0 || pos.x == GAME_BOARD_SIZE.x || pos.y == 0 || pos.y == GAME_BOARD_SIZE.y) {
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
	for (int i = 0; i < MAX_SNAKE_SIZE; i++) {
		if (snake[i].x == 0)
			break ;
		Vector2	pos = Vector2Scale(snake[i], TILE_SIZE);
		pos = Vector2Add(pos, board_offset);
		DrawRectangle(pos.x, pos.y, TILE_SIZE, TILE_SIZE, BLUE);
	}

	for (int i = 0; i < MAX_APPLES; i++) {
		if (apples[i].x == 0)
			continue;
		Vector2	pos = Vector2Scale(apples[i], TILE_SIZE);
		pos = Vector2Add(pos, board_offset);
		DrawRectangle(pos.x, pos.y, TILE_SIZE, TILE_SIZE, RED);
	}

	for (int y = 0; y < GAME_BOARD_SIZE.y; y++) {
		DrawRectangle(board_offset.x, (y * TILE_SIZE) + board_offset.y, TILE_SIZE, TILE_SIZE, BLACK);
		DrawRectangle(board_offset.x + (GAME_BOARD_SIZE.x * TILE_SIZE), (y * TILE_SIZE) + board_offset.y, TILE_SIZE, TILE_SIZE, BLACK);
	}
	for (int x = 0; x < GAME_BOARD_SIZE.x + 1; x++) {
		DrawRectangle(board_offset.x + (x * TILE_SIZE), board_offset.y, TILE_SIZE, TILE_SIZE, BLACK);
		DrawRectangle(board_offset.x + (x * TILE_SIZE), (GAME_BOARD_SIZE.y * TILE_SIZE) + board_offset.y, TILE_SIZE, TILE_SIZE, BLACK);
	}
}
