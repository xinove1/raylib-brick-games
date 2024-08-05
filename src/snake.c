#include "game.h"
#include "raylib.h"

# define MAX_APPLES 10
# define MAX_SNAKE_SIZE ((BoardSize.x - 1) * (BoardSize.y - 1))

static void draw();
static void draw_game();
static void move_snake_body(Vector2 new_head_pos, int add_body);
static void spawn_apple();
static i32  check_collision(Vector2	pos);

static i32 TileSize = TILE_SIZE;
static V2 BoardSize = {15, 15};
static V2 BoardOffset;
static V2 *Snake;
static V2 Dir = {1, 0};
static V2 NewDir = {1, 0};
static V2 Apples[MAX_APPLES] = {0};
static f32 TickTimeCount = 0;
static f32 TickRate = 0.150; // How much time until a game tick in seconds
static f32 AppleSpawnRate = 5; // in seconds
static bool PlayScreen = true;
static bool Paused = false;
static bool GameOver = false;
static bool EasyMode = true;
static UiContainer Container;

GameData *data = 0;

static void start()
{
	memset(Snake, 0, sizeof(V2) * MAX_SNAKE_SIZE);
	memset(Apples, 0, sizeof(V2) * MAX_APPLES);
	Snake[0] = (V2) {(int) (BoardSize.x * 0.5f), (int) (BoardSize.y * 0.5f)}; // Set snake head to middle of the board

	PlayScreen = true; 
	GameOver = false;
	Paused = false;

	TickTimeCount = 0;
	Dir = (V2) {1,0};
	NewDir = (V2) {1,0};
	spawn_apple();
}

static void update()
{
	if (!ShouldGameRun(&PlayScreen, &Paused, &GameOver)) {
		return ;
	}

	if (IsActionPressed(RIGHT)) {
		NewDir.x = 1;
		NewDir.y = 0;
	}
	if (IsActionPressed(LEFT)) {
		NewDir.x = -1;
		NewDir.y = 0;
	}
	if (IsActionPressed(UP)) {
		NewDir.x = 0;
		NewDir.y = -1;
	}
	if (IsActionPressed(DOWN)) {
		NewDir.x = 0;
		NewDir.y = 1;
	}

	// TODO  Implement easy mode (not collide with walls)
	
	
	TickTimeCount += GetFrameTime();
	if (TickTimeCount >= TickRate) {
		TickTimeCount = 0;
		// Check to see if player is not tryng to 
		if (Snake[1].x == 0 || -NewDir.x != Dir.x || -NewDir.y != Dir.y ) {
			Dir = NewDir;
		}

		V2	new_pos = Vector2Add(Snake[0], Dir);

		if (EasyMode) {
			if (new_pos.x == 0) {
				new_pos.x = BoardSize.x - 1;
			} else if (new_pos.x == BoardSize.x) {
				new_pos.x = 1;
			}
			if (new_pos.y == 0) {
				new_pos.y = BoardSize.y - 1;
			} else if (new_pos.y == BoardSize.y) {
				new_pos.y = 1;
			}
		}

		i32 collision = check_collision(new_pos);
		move_snake_body(new_pos, collision);
		if (collision == 1) {
			GameOver = true;
			return ;
		}
		if (collision == 2 || (int)TickTimeCount % (int)AppleSpawnRate == 0) {
			spawn_apple();
		}
		if (Snake[(int)MAX_SNAKE_SIZE - 1].x != 0) {
			printf("YOU WIN!!/n");
			GameOver = true;
			// TODO draw win screen
		}
	}
}

static void	draw() 
{
	ClearBackground(RAYWHITE);
	draw_game();

	if (PlayScreen) {
		UiContainer *panel = &Container;
		UiBegin(panel);
		{
			// Workaround for now
			V2 max_size = MeasureTextEx(panel->config.font.font, " Mode: Normal ", panel->config.font.size, panel->config.font.spacing);
			panel->width = max_size.x + 15;

			UiText(panel, "Snake", true);

			if (UiTextButton(panel, "Play")) { 
				PlayScreen = false;
			}

			char *mode = EasyMode ? "Mode: Easy" : "Mode: Normal";
			if (UiTextButton(panel, mode)) { 
				EasyMode = EasyMode ? false : true;
			}

			if (UiTextButton(panel, "Back")) { 
				data->current_game = MAIN_MENU;
			}
		}
		UiEnd(panel);

		if (IsActionPressed(ACTION_2)) {
			data->current_game = MAIN_MENU;
		}
	}

	if (GameOver) {
		UiStates state = game_over_screen(data);
		if (state == NONE) {
			GameOver = false;
			start();
		} else if (state == TITLE_SCREEN) {
			// SAVE?
			data->current_game = MAIN_MENU;
			GameOver = false;
		}
	}

	static bool options = false;
	if (Paused && options == false) {

		UiContainer *panel = &Container;
		UiBegin(panel);
		{
			UiText(panel, "Game Paused", true);

			if (UiTextButton(panel, "Back to Game")) { 
				Paused = false;
			} 

			if (UiTextButton(panel, "Options")) { 
				options = true;
			}

			if (UiTextButton(panel, "Exit To Main Menu")) { 
				data->current_game = MAIN_MENU;
			}

			if (UiTextButton(panel, "Exit To Desktop")) { 
				data->quit = true;
			}
		}
		UiEnd(panel);
		if (IsActionPressed(ACTION_2)) {
			Paused = false;
		}

	} else if (Paused && options) {
		UiStates state = options_screen(data);
		if (state == BACK) {
			options = false;
		}
	}
}

void	de_init() {
	free(Snake);
}

GameFunctions	snake_game_init(GameData *game_data)
{
	data = game_data;

	Snake = calloc(MAX_SNAKE_SIZE, sizeof(Vector2));
	BoardOffset =  (V2) {
		.x = data->window_size.x * 0.5f - (BoardSize.x * TileSize) * 0.5f,
		.y = data->window_size.y * 0.5f - (BoardSize.y * TileSize) * 0.5f
	};

	V2	center_screen = {data->window_size.x * 0.5f, data->window_size.y * 0.25f}; // Center offset to where to start drawing text
	Container = CreateContainer(center_screen, 0, data->ui_config);

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
	Vector2	tmp = Snake[0];
	for (i = 1; i < MAX_SNAKE_SIZE; i++) {
		if (Snake[i].x == 0) // Assuming snake is initialised with 0's and that pos 0,0 is inside board and you can't reach this part of the code being there
			break ;
		Vector2	tmp2 = Snake[i];
		Snake[i] = tmp;
		tmp = tmp2;
	}
	assert(i != MAX_SNAKE_SIZE);
	if (add_body) {
		//Vector2	_dir = Vector2Subtract(snake[i - 1], snake[i - 2]);
		//printf("dir: %f,%f\n", _dir.x, _dir.y);
		Snake[i] = tmp;
	}
	Snake[0] = new_head_pos;
}

static void	spawn_apple()
{
	Vector2	pos;
	do {
		pos = (V2) {GetRandomValue(1, BoardSize.x - 1), GetRandomValue(1, BoardSize.y - 1)};
	} while (check_collision(pos));

	for (int i = 0; i < MAX_APPLES; i++) {
		if (Apples[i].x == 0) {
			Apples[i] = pos;
			break ;
		}
	}
}

static int	check_collision(Vector2	pos)
{
	// Check collision with game board
	if (!EasyMode && (pos.x == 0 || pos.x == BoardSize.x || pos.y == 0 || pos.y == BoardSize.y)) {
		return (1);
	}

	for (int i = 1; i < MAX_SNAKE_SIZE; i++) {
		if (Snake[i].x == 0)
			break ;
		if (pos.x == Snake[i].x && pos.y == Snake[i].y) {
			return (1); // Colliding
		}
	}

	for (int i = 0; i < MAX_APPLES; i++) {
		if (Apples[i].x == 0)
			continue;
		if (pos.x == Apples[i].x && pos.y == Apples[i].y) {
			Apples[i] = (Vector2) {0,0};
			return (2); // Colliding with apple
		}
	}
	return (0);
}

static void	draw_game()
{
	ColorPalette	palette = data->palette;

	// Grid
	draw_grid((V2){BoardOffset.x + TileSize, BoardOffset.y + TileSize}, (V2){BoardSize.x -1, BoardSize.y -1}, TileSize);

	// Snake
	for (int i = 0; i < MAX_SNAKE_SIZE; i++) {
		if (Snake[i].x == 0)
			break ;
		Vector2	pos = Vector2Scale(Snake[i], TileSize);
		pos = Vector2Add(pos, BoardOffset);
		DrawRectangle(pos.x, pos.y, TileSize, TileSize, palette.green);
		// Head Eyes
		if (i == 0) {
			float	offset = TileSize * 0.35f;
			float	size = 2.5;
			V2	v1 = {pos.x + offset - size, pos.y + offset - size}; // upper left
			V2	v2 = {(pos.x + TileSize) - offset, pos.y + offset - size}; // upper right
			V2	v3 = {pos.x + offset - size, (pos.y + TileSize) - offset}; // down left
			V2	v4 = {(pos.x + TileSize) - offset, (pos.y + TileSize) - offset}; // down right
			V2	eye1;
			V2	eye2;
			if (Dir.x == 1) {
				eye1 = v2;
				eye2 = v4;
			}
			if (Dir.x == -1) {
				eye1 = v1;
				eye2 = v3;
			}
			if (Dir.y == 1) {
				eye1 = v3;
				eye2 = v4;
			}
			if (Dir.y == -1) {
				eye1 = v1;
				eye2 = v2;
			}
			DrawRectangle(eye1.x, eye1.y, size, size, palette.black);
			DrawRectangle(eye2.x, eye2.y, size, size, palette.black);
		}
	}

	// Apples
	for (int i = 0; i < MAX_APPLES; i++) {
		if (Apples[i].x == 0)
			continue;
		Vector2	pos = Vector2Scale(Apples[i], TileSize);
		pos = Vector2Add(pos, BoardOffset);
		DrawRectangle(pos.x, pos.y, TileSize, TileSize, palette.red);
	}

	// Board border
	float	line_thickness = 5;
	Rect	board = {
		.x = BoardOffset.x + TileSize - line_thickness,
		.y = BoardOffset.y + TileSize - line_thickness,
		.width = BoardSize.x * (TileSize -1) + TileSize * 0.5f,
		.height= BoardSize.y * (TileSize -1) + TileSize * 0.5f
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
