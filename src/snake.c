#include "game.h"
#include "raylib.h"

// TODO  Change max apples to be board_size dependent
# define MAX_APPLES 10
# define MAX_SNAKE_SIZE 20*20 // = Max board size
# define BOARD_SIZE_COUNT 4
static i32 BoardSizes[BOARD_SIZE_COUNT] = {8, 10, 15, 20};
static byte *BoardSizesText[BOARD_SIZE_COUNT] = {"8x8", "10x10", "15x15", "20x20"};

struct SnakeData {
	V2 board_size;
	V2 board_offset;
	V2 snake[MAX_SNAKE_SIZE];
	V2 dir;
	V2 dir_new;
	V2 apples[MAX_APPLES];
	i32 tile_size;
	i32 selected_board_size;
	i32 snake_size;
	f32 tick_count;
	f32 tick_rate; // How much time until a game tick in seconds
	i32 apples_max;
	f32 apple_spawn_rate; // in seconds
	f32 apple_spawn_count;
	f32 scores[8];
	b32 play_screen;
	b32 paused;
	b32 game_over;
	b32 easy_mode;
	b32 won;
	UiContainer Container;
};


static void start();
static void update();
static void draw();
static void draw_game();
static void de_init();
static void move_snake_body(V2 new_head_pos, i32 add_body);
static void spawn_apple();
static i32  check_collision(V2 pos);

GameData *Data = 0;
SnakeData *Snake = 0;

GameFunctions snake_game_init(GameData *data)
{
	Data = data;
	
	data->snake_data = calloc(1, sizeof(SnakeData));
	Snake = data->snake_data;

	*Snake = (SnakeData) {
		.board_size = (V2) {BoardSizes[0], BoardSizes[0]},
		.board_offset =  (V2) {.x = Data->window_size.x * 0.5f, .y = Data->window_size.y * 0.5f },
		.snake = 0,
		.dir = (V2) {1, 0},
		.dir_new = (V2){1, 0},
		.apples = {{0}},
		.tile_size = TILE_SIZE,
		.selected_board_size = 0,
		.snake_size = 0,
		.tick_count = 0,
		.tick_rate = 0.150, // How much time until a game tick in seconds
		.apples_max = 5,
		.apple_spawn_rate = 5, // in seconds
		.apple_spawn_count = 0,
		.scores = 0,
		.play_screen = true,
		.paused = false,
		.game_over = false,
		.won = false,
		.easy_mode = true,
	};

	V2 center_screen = {Data->window_size.x * 0.5f, Data->window_size.y * 0.25f};
	Snake->Container = UiCreateContainer(center_screen, 0, Data->ui_config);
	//Snake->scores = data->scores.snake;
	memcpy(&Snake->scores, &data->scores.snake, sizeof(Snake->scores));

	return (GameFunctions) { 
		.name = "snake Game",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

void de_init() {
	memcpy(&Data->scores.snake, &Snake->scores, sizeof(Snake->scores));
}

static void start()
{
	assert(Snake);
	// TODO  Remove start call from main.c, each game already has a main menu and can manully call it's own prep func

	Snake->board_size = (V2) {BoardSizes[Snake->selected_board_size] + 1, BoardSizes[Snake->selected_board_size] + 1};
	Snake->board_offset =  (V2) {
			.x = Data->window_size.x * 0.5f - (Snake->board_size.x * Snake->tile_size) * 0.5f,
			.y = Data->window_size.y * 0.5f - (Snake->board_size.y * Snake->tile_size) * 0.5f };
	Snake->snake_size = (BoardSizes[Snake->selected_board_size] * BoardSizes[Snake->selected_board_size]);
	memset(Snake->snake, 0, sizeof(V2) * MAX_SNAKE_SIZE);
	memset(Snake->apples, 0, sizeof(V2) * MAX_APPLES);
	Snake->snake[0] = (V2) {(i32) (Snake->board_size.x * 0.5f), (i32) (Snake->board_size.y * 0.5f)}; // Set snake head to middle of the board
	spawn_apple();

}

static void update()
{
	assert(Snake);
	if (!ShouldGameRun(&Snake->play_screen, &Snake->paused, &Snake->game_over) || Snake->won) {
		return ;
	}

	if (IsActionDown(RIGHT)) {
		Snake->dir_new.x = 1;
		Snake->dir_new.y = 0;
	}
	if (IsActionDown(LEFT)) {
		Snake->dir_new.x = -1;
		Snake->dir_new.y = 0;
	}
	if (IsActionDown(UP)) {
		Snake->dir_new.x = 0;
		Snake->dir_new.y = -1;
	}
	if (IsActionDown(DOWN)) {
		Snake->dir_new.x = 0;
		Snake->dir_new.y = 1;
	}

	static b32 debug_pressed = false;
	if (IsKeyDown(KEY_U)) debug_pressed = true;
	else debug_pressed = false;

	Snake->tick_count += GetFrameTime();
	Snake->apple_spawn_count += GetFrameTime();
	if (Snake->tick_count >= Snake->tick_rate) {
		Snake->tick_count = 0;
		// Check to see if player is not tryng to 
		if (Snake->snake[1].x == 0 || -Snake->dir_new.x != Snake->dir.x || -Snake->dir_new.y != Snake->dir.y ) {
			Snake->dir = Snake->dir_new;
		}

		V2 new_pos = V2Add(Snake->snake[0], Snake->dir);

		if (Snake->easy_mode) {
			if (new_pos.x == 0) {
				new_pos.x = Snake->board_size.x - 1;
			} else if (new_pos.x == Snake->board_size.x) {
				new_pos.x = 1;
			}
			if (new_pos.y == 0) {
				new_pos.y = Snake->board_size.y - 1;
			} else if (new_pos.y == Snake->board_size.y) {
				new_pos.y = 1;
			}
		}

		i32 collision = check_collision(new_pos);
		if (collision == 1) {
			printf("Game Over \n");
			Snake->game_over = true;
			return ;
		}
		if (debug_pressed) move_snake_body(new_pos, true);
		else move_snake_body(new_pos, collision);

		if (!V2Compare(Snake->snake[Snake->snake_size - 1], V2Zero())) { // This check needs to happen before spawn_apple() is maybe invocked 
			Snake->won = true;
			return ;
		}

		if (collision == 2) {
			i32 score_index = (Snake->selected_board_size * 2) + Snake->easy_mode;
			assert(score_index <= sizeof(Snake->scores));
			Snake->scores[score_index] += 50;
			spawn_apple();
		} else if (Snake->apple_spawn_count >= Snake->apple_spawn_rate) {
			Snake->apple_spawn_count = 0;
			spawn_apple();
		}
	}
}

static void draw() 
{
	ClearBackground(RAYWHITE);
	draw_game();

	static b32 scores_screen = false;
	if (Snake->play_screen && !scores_screen) {
		UiContainer *panel = &Snake->Container;
		UiBegin(panel);
		{
			// Workaround for now
			V2 max_size = MeasureTextEx(panel->config.font.font, " Board size: 20x20 ", panel->config.font.size, panel->config.font.spacing);
			panel->width = max_size.x + 15;

			UiText(panel, "snake", true);

			if (UiTextButton(panel, "Play")) { 
				Snake->play_screen = false;
			}

			if (UiTextOptionsEx(panel, panel->config, true, "Board size: ", BoardSizesText, BOARD_SIZE_COUNT, &Snake->selected_board_size)) {
				start();
			}

			byte *mode = Snake->easy_mode ? "Mode: Easy" : "Mode: Normal";
			if (UiTextButton(panel, mode)) { 
				Snake->easy_mode = Snake->easy_mode ? false : true;
			}

			if (UiTextButton(panel, "Scores")) { 
				scores_screen = true;
			}

			if (UiTextButton(panel, "Back")) { 
				Data->current_game = MAIN_MENU;
			}
		}
		UiEnd(panel);

		if (IsActionPressed(ACTION_2)) {
			Data->current_game = MAIN_MENU;
		}
	} else if (Snake->play_screen && scores_screen) {
		UiContainer *panel = &Snake->Container;

		if (!IsKeyDown(KEY_U)) panel->config.alignment = UiAlignLeft; // NOTE  debug stuff
		
		V2 panel_pos = panel->pos;
		panel->pos.x -=  panel->width * 0.5f;
		UiBegin(panel);
		{
			// Workaround for now
			// V2 max_size = MeasureTextEx(panel->config.font.font, " Board size: 20x20 ", panel->config.font.size, panel->config.font.spacing);
			// panel->width = max_size.x + 15;

			UiText(panel, "Scores :", true);

			UiStartColumn(panel, 3);
			UiText(panel, "     ", false);
			UiText(panel, " Normal", false);
			UiText(panel, " Easy  ", false);

			UiStartColumn(panel, 3);
			UiText(panel, "\t8x8\t", false);
			UiText(panel, (byte *) TextFormat("%6.f", Snake->scores[0]), false);
			UiText(panel, (byte *) TextFormat("%6.f", Snake->scores[1]), false);

			UiStartColumn(panel, 3);
			UiText(panel, (byte *) TextFormat("%s", BoardSizesText[1]), false);
			UiText(panel, (byte *) TextFormat("%6.f", Snake->scores[2]), false);
			UiText(panel, (byte *) TextFormat("%6.f", Snake->scores[3]), false);

			UiStartColumn(panel, 3);
			UiText(panel, (byte *) TextFormat("%s", BoardSizesText[2]), false);
			UiText(panel, (byte *) TextFormat("%6.f", Snake->scores[4]), false);
			UiText(panel, (byte *) TextFormat("%6.f", Snake->scores[5]), false);

			UiStartColumn(panel, 3);
			UiText(panel, (byte *) TextFormat("%s", BoardSizesText[3]), false);
			UiText(panel, (byte *) TextFormat("%6.f", Snake->scores[6]), false);
			UiText(panel, (byte *) TextFormat("%6.f", Snake->scores[7]), false);

			if (UiTextButton(panel, "Back")) { 
				scores_screen = false;
			}
		}
		UiEnd(panel);
		panel->config.alignment = UiAlignCentralized;
		panel->pos = panel_pos;

		if (IsActionPressed(ACTION_2)) {
			scores_screen = false;
		}
	}

	if (Snake->won) {
		UiContainer *panel = &Snake->Container;
		UiBegin(panel);
		{
			UiText(panel, "You Won!!!", true);

			if (UiTextButton(panel, "Play Again")) { 
				Snake->play_screen = true;
				Snake->won = false;
			} 

			if (UiTextButton(panel, "Exit To Main Menu")) { 
				Data->current_game = MAIN_MENU;
				Snake->play_screen = true;
				Snake->won = false;
			}

			if (UiTextButton(panel, "Exit To Desktop")) { 
				Data->quit = true;
			}
		}
		UiEnd(panel);
	}

	if (Snake->game_over) {
		UiStates state = game_over_screen(Data);
		if (state == NONE) {
			Snake->game_over = false;
			Snake->play_screen = true;
			start();
		} else if (state == TITLE_SCREEN) {
			// SAVE?
			Data->current_game = MAIN_MENU;
			Snake->game_over = false;
			Snake->play_screen = true;
		}
	}

	static bool options = false; // TODO  move to snake
	if (Snake->paused && options == false) {
		UiContainer *panel = &Snake->Container;
		UiBegin(panel);
		{
			UiText(panel, "Game Paused", true);

			if (UiTextButton(panel, "Back to Game")) { 
				Snake->paused = false;
			} 

			if (UiTextButton(panel, "Options")) { 
				options = true;
			}

			if (UiTextButton(panel, "Exit To Main Menu")) { 
				Data->current_game = MAIN_MENU;
				Snake->paused = false;
				Snake->play_screen = true;
			}

			if (UiTextButton(panel, "Exit To Desktop")) { 
				Data->quit = true;
			}
		}
		UiEnd(panel);
		if (IsActionPressed(ACTION_2)) {
			Snake->paused = false;
		}

	} else if (Snake->paused && options) {
		UiStates state = options_screen(Data);
		if (state == BACK) {
			options = false;
		}
	}
}

static void move_snake_body(V2 new_head_pos, i32 add_body)
{
	i32 i;
	V2  tmp = Snake->snake[0];
	for (i = 1; i < Snake->snake_size; i++) {
		if (Snake->snake[i].x == 0) break ;
		V2 tmp2 = Snake->snake[i];
		Snake->snake[i] = tmp;
		tmp = tmp2;
	}
	assert(i != Snake->snake_size);
	if (add_body) {
		//Vector2	_dir = Vector2Subtract(snake[i - 1], snake[i - 2]);
		//printf("dir: %f,%f\n", _dir.x, _dir.y);
		Snake->snake[i] = tmp;
	}
	Snake->snake[0] = new_head_pos;
}

static void spawn_apple()
{
	V2 *apple = NULL;
	for (i32 i = 0; i < Snake->apples_max; i++) {
		if (V2Compare(Snake->apples[i], V2Zero())) {
			apple = &Snake->apples[i];
			break ;
		}
	}
	if (apple == NULL) return ;

	V2 pos;
	do {
		pos = (V2) {GetRandomValue(1, Snake->board_size.x - 1), GetRandomValue(1, Snake->board_size.y - 1)};
	} while (check_collision(pos) != 0);
	*apple = pos;
}

static i32 check_collision(V2 pos)
{
	// Check collision with game board
	if (!Snake->easy_mode && (pos.x == 0 || pos.x == Snake->board_size.x || pos.y == 0 || pos.y == Snake->board_size.y)) {
		return (1);
	}

	for (i32 i = 0; i < Snake->snake_size; i++) {
		if (Snake->snake[i].x == 0) break ;
		if (pos.x == Snake->snake[i].x && pos.y == Snake->snake[i].y) {
			return (1); // Colliding
		}
	}

	for (i32 i = 0; i < Snake->apples_max; i++) {
		if (Snake->apples[i].x == 0) continue;
		if (pos.x == Snake->apples[i].x && pos.y == Snake->apples[i].y) {
			Snake->apples[i] = (V2) {0,0};
			return (2); // Colliding with apple
		}
	}
	return (0);
}

static void draw_game()
{
	ColorPalette palette = Data->palette;

	// Grid
	V2 board_pos = {Snake->board_offset.x + Snake->tile_size, Snake->board_offset.y + Snake->tile_size};
	V2 board_size = {Snake->board_size.x -1, Snake->board_size.y -1};
	draw_grid_ex(board_pos, board_size, Snake->tile_size, 2, ColorAlpha(GRAY, 0.1f));

	// snake
	for (i32 i = 0; i < Snake->snake_size; i++) {
		if (Snake->snake[i].x == 0) break ;
		V2 pos = V2Scale(Snake->snake[i], Snake->tile_size);
		pos = V2Add(pos, Snake->board_offset);
		DrawRectangle(pos.x, pos.y, Snake->tile_size, Snake->tile_size, palette.green);
		// Head Eyes
		if (i == 0) {
			f32 offset = Snake->tile_size * 0.35f;
			f32 size = 2.5;
			V2  v1 = {pos.x + offset - size, pos.y + offset - size}; // upper left
			V2  v2 = {(pos.x + Snake->tile_size) - offset, pos.y + offset - size}; // upper right
			V2  v3 = {pos.x + offset - size, (pos.y + Snake->tile_size) - offset}; // down left
			V2  v4 = {(pos.x + Snake->tile_size) - offset, (pos.y + Snake->tile_size) - offset}; // down right
			V2  eye1;
			V2  eye2;
			if (Snake->dir.x == 1) {
				eye1 = v2;
				eye2 = v4;
			}
			if (Snake->dir.x == -1) {
				eye1 = v1;
				eye2 = v3;
			}
			if (Snake->dir.y == 1) {
				eye1 = v3;
				eye2 = v4;
			}
			if (Snake->dir.y == -1) {
				eye1 = v1;
				eye2 = v2;
			}
			DrawRectangle(eye1.x, eye1.y, size, size, palette.black);
			DrawRectangle(eye2.x, eye2.y, size, size, palette.black);
		}
	}

	// Apples
	for (i32 i = 0; i < Snake->apples_max; i++) {
		if (Snake->apples[i].x == 0) continue;
		V2 pos = V2Scale(Snake->apples[i], Snake->tile_size);
		pos = V2Add(pos, Snake->board_offset);
		DrawRectangle(pos.x, pos.y, Snake->tile_size, Snake->tile_size, palette.red);
	}

	// Board border
	f32  line_thickness = 4;
	Rect board = {
		.x = Snake->board_offset.x + Snake->tile_size - line_thickness,
		.y = Snake->board_offset.y + Snake->tile_size - line_thickness,
		.width = ((Snake->board_size.x - 1) * Snake->tile_size) + Snake->tile_size * 0.5f,
		.height= ((Snake->board_size.y - 1) * Snake->tile_size) + Snake->tile_size * 0.5f
	};
	DrawRectangleLinesEx(board, line_thickness, palette.red);
}
