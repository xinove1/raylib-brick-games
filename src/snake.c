#include "game.h"

# define WINDOW_WIDTH 800
# define WINDOW_HEIGHT 450
# define GAME_BOARD_SIZE (Vector2){15, 15}
# define TILE_SIZE 16
# define MAX_APPLES 10
# define MAX_SNAKE_SIZE ((GAME_BOARD_SIZE.x - 1) * (GAME_BOARD_SIZE.y - 1))

void	draw_game();
int		check_collision(Vector2	pos);
void	move_snake_body(Vector2 new_head_pos, int add_body);
void	spawn_apple();

Vector2	board_offset = {WINDOW_WIDTH/3.0, 10};
Vector2	*snake;
Vector2	dir = {0, 0};
Vector2	apples[MAX_APPLES] = {10,10};

int main(void)
{
	snake = malloc(sizeof(Vector2) * MAX_SNAKE_SIZE);
	snake[0] = (Vector2) {5,5};
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake_game");
	SetTargetFPS(60);

    while (!WindowShouldClose())
    {
		// TODO check if dir is not snake body

		if (IsKeyPressed(KEY_D))
		{
			dir.x = 1;
			dir.y = 0;
		}
		if (IsKeyPressed(KEY_A))
		{
			dir.x = -1;
			dir.y = 0;
		}
		if (IsKeyPressed(KEY_W))
		{
			dir.x = 0;
			dir.y = -1;
		}
		if (IsKeyPressed(KEY_S))
		{
			dir.x = 0;
			dir.y = 1;
		}

		if (dir.x != 0 || dir.y != 0)
		{
			int	collision = check_collision(Vector2Add(snake[0], dir));
			if (collision == 1)
			{
				printf("Game Over\n");
				break ;
			}
			if (collision == 2)
				spawn_apple();
			move_snake_body(Vector2Add(snake[0], dir), collision);
			dir.x = 0;
			dir.y = 0;
		}

        BeginDrawing();
            ClearBackground(RAYWHITE);
			draw_game();
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }
	printf("board_offset: %f,%f\n", board_offset.x, board_offset.y);

    CloseWindow();
	free(snake);
    return 0;
}

void	move_snake_body(Vector2 new_head_pos, int add_body)
{
	int i;
	Vector2	tmp = snake[0];
	for (i = 1; i < MAX_SNAKE_SIZE; i++)
	{
		if (snake[i].x == 0)
			break ;
		Vector2	tmp2 = snake[i];
		snake[i] = tmp;
		tmp = tmp2;
	}
	if (add_body)
	{
		Vector2	_dir = Vector2Subtract(snake[i - 1], snake[i - 2]);
		printf("dir: %f,%f\n", _dir.x, _dir.y);
		snake[i] = tmp;
	}
	snake[0] = new_head_pos;
}

void	spawn_apple()
{
	Vector2	pos = {GetRandomValue(1, GAME_BOARD_SIZE.x - 1), GetRandomValue(1, GAME_BOARD_SIZE.y - 1)};

	for (int i = 0; i < MAX_APPLES; i++)
	{
		if (apples[i].x == 0)
		{
			apples[i] = pos;
			break ;
		}
	}
}

int	check_collision(Vector2	pos)
{
	// Check collision with game board
	if (pos.x == 0 || pos.x == GAME_BOARD_SIZE.x || pos.y == 0 || pos.y == GAME_BOARD_SIZE.y)
		return (1);
	for (int i = 1; i < MAX_SNAKE_SIZE; i++)
	{
		if (snake[i].x == 0)
			break ;
		if (pos.x == snake[i].x && pos.y == snake[i].y)
			return (1); // Colliding
	}

	for (int i = 0; i < MAX_APPLES; i++)
	{
		if (apples[i].x == 0)
			continue; ;
		if (pos.x == apples[i].x && pos.y == apples[i].y)
		{
			apples[i] = (Vector2) {0,0};
			return (2); // Colliding with apple
		}
	}
	return (0);
}

void	draw_game()
{
	for (int i = 0; i < MAX_SNAKE_SIZE; i++)
	{
		if (snake[i].x == 0)
			break ;
		Vector2	pos = Vector2Scale(snake[i], TILE_SIZE);
		pos = Vector2Add(pos, board_offset);
		DrawRectangle(pos.x, pos.y, TILE_SIZE, TILE_SIZE, BLUE);
	}

	for (int i = 0; i < MAX_APPLES; i++)
	{
		if (apples[i].x == 0)
			continue; ;
		Vector2	pos = Vector2Scale(apples[i], TILE_SIZE);
		pos = Vector2Add(pos, board_offset);
		DrawRectangle(pos.x, pos.y, TILE_SIZE, TILE_SIZE, RED);
	}

	for (int y = 0; y < GAME_BOARD_SIZE.y; y++)
	{
		DrawRectangle(board_offset.x, (y * TILE_SIZE) + board_offset.y, TILE_SIZE, TILE_SIZE, BLACK);
		DrawRectangle(board_offset.x + (GAME_BOARD_SIZE.x * TILE_SIZE), (y * TILE_SIZE) + board_offset.y, TILE_SIZE, TILE_SIZE, BLACK);
	}
	for (int x = 0; x < GAME_BOARD_SIZE.x + 1; x++)
	{
		DrawRectangle(board_offset.x + (x * TILE_SIZE), board_offset.y, TILE_SIZE, TILE_SIZE, BLACK);
		DrawRectangle(board_offset.x + (x * TILE_SIZE), (GAME_BOARD_SIZE.y * TILE_SIZE) + board_offset.y, TILE_SIZE, TILE_SIZE, BLACK);
	}
}
