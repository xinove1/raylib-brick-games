#include "game.h"
#include "raylib.h"

typedef struct {
	Music	music;
	Sound	made_line;
	Sound	game_over;
} TetrisSounds;

static GameData	*data = 0;
static Vector2	window_size;
static char	*pieces[7] = {0};
static const Vector2	board_size = {12, 18};
static char	*board = NULL;
static int	made_lines[18] = {0}; // NOTE hard coded
static bool	is_made_lines = false;
static bool	paused = false;
static bool	game_over = false;
static bool	play_screen = false;
static TetrisSounds	game_sounds;

// Drawing related
static Vector2	board_offset = {0, 0};
static int	tileSize = TILE_SIZE;
static Color	piece_colors[9];
static FontConfig	font;
static UiContainer Container;

// Game
static Vector2	pos = {board_size.x / 2 - 2, 0};
static int	piece = 0;
static int	next_piece = 0;
static int	stored_piece = -1;
static bool	stored_piece_flag = false; // Flag to track if there already being a swap of current piece
static int	rotation = 0;


static int	score = 0;
static int	high_score = 0; //  TODO  Save and restore highscore

//  FIXME  Better name for speed variables
static float	tick_time_count = 0; // counting time
static float	tick_time = 0.100; // How much time until a game tick in seconds
static int	speed = 10; // How many game ticks until force drowdown
static int	speed_count = 0; // counting game ticks
static int	speed_limit = 2; // How low the speed can get
static int	speed_increase_rate = 5; // How many Pieces until a speed increase
static int	rotate_count = 0;
static int	rotate_rate = 2; // Rate in ticks to rotate when holding down rotate button
static int	piece_count = 0; 

static int	rotate(Vector2 pos, int r);
static int	check_piece_collision(int piece, Vector2 pos, int rotation);
static void	draw();
static void	draw_piece(int piece, Vector2 pos, int rotation, int scale, Vector2 offset);
static void	draw_board();
static int	check_line_made(Vector2 pos);
static void	cleanup_made_lines();
static void	force_piece_down();
static void	clean_board();

static void start()
{
	clean_board();
	paused = false;
	game_over = false;
	play_screen = true;
	piece = rand() % 7;
	next_piece = rand() % 7;
	stored_piece = -1;
	PlayMusicStream(game_sounds.music);
}

static void	update()
{
	UpdateMusicStream(game_sounds.music);

	if (!ShouldGameRun(&play_screen, &paused, &game_over)) {
		return ;
	}

	if (IsActionPressed(ACTION_3)) {
		V2	new_pos = pos;
		while (!check_piece_collision(piece, new_pos, rotation)) {
			new_pos.y += 1;
		}
		pos.y = new_pos.y - 1;
		force_piece_down();
		tick_time_count = 0;
	}

	if (tick_time_count < tick_time) {
		tick_time_count += GetFrameTime();
	} else {
		tick_time_count = 0;
		speed_count++;

		if (speed_count == speed || IsActionDown(DOWN)) 
			force_piece_down();

		if (IsActionDown(LEFT) && !check_piece_collision(piece, (Vector2){pos.x - 1, pos.y}, rotation)) 
			pos = (Vector2) {pos.x - 1, pos.y};
		if (IsActionDown(RIGHT)  && !check_piece_collision(piece, (Vector2){pos.x + 1, pos.y}, rotation)) 
			pos = (Vector2) {pos.x + 1, pos.y};

		if (IsActionDown(ACTION_2)) {
			if (stored_piece == -1) {
				stored_piece = piece;
				piece = next_piece;
				next_piece = rand() % 7;
				stored_piece_flag = true;
			} else if (!stored_piece_flag) {
				stored_piece_flag = true;
				int tmp = piece;
				piece = stored_piece;
				stored_piece = tmp;
			}
		}
		if (IsActionDown(ACTION_1) || IsActionDown(UP)) {
			int	new_rotation = rotation;
			if (IsActionDown(ACTION_1)) {
				new_rotation += -1;
			}
			if (IsActionDown(UP)) {
				new_rotation += 1;
			}
			if (piece == 0 && new_rotation == 2) {
				new_rotation = 0;
			}
			if (new_rotation == 4) {
				new_rotation = 0;
			}
			if (new_rotation == -1) {
				new_rotation = 3;
			}

			if (rotate_count % rotate_rate == 0 && !check_piece_collision(piece, pos, new_rotation)) {
				rotation = new_rotation;
			}
			rotate_count++;
		} 
		else {
			rotate_count = 0;
		}

		//  NOTE  Remove. for debug only
		if (IsKeyDown(KEY_C)) {
			piece += 1;
			if (piece == 7)
				piece = 0;
		}
	}
}

static void	draw() {
	draw_grid(board_offset, (V2){board_size.x -1, board_size.y -1}, tileSize); 
	draw_board();
	draw_piece(piece, pos, rotation, tileSize, board_offset);

	{
		const char	*text = TextFormat("Score: %d", score);
		V2	text_size = MeasureTextEx(font.font, text, font.size, font.spacing);
		V2 offset = {board_offset.x - tileSize * 5, board_offset.y};
		
		
		V2	text_pos = {offset.x, offset.y - text_size.y};
		draw_grid(offset, (V2){4, 4}, tileSize);
		DrawRectangle(offset.x, offset.y, tileSize, tileSize, ColorAlpha(RED, 0.2));
		DrawTextEx(font.font, text, text_pos, font.size, font.spacing, data->palette.green);
		if (stored_piece != -1)  {
			draw_piece(stored_piece, (Vector2){1,1}, 0, tileSize, offset);
		}
	}

	{
		const char	*text = "Coming up:";
		V2	text_size = MeasureTextEx(font.font, text, font.size, font.spacing);
		V2 offset = {board_offset.x + board_size.x * tileSize, board_offset.y};
		
		
		V2	text_pos = {offset.x, offset.y - text_size.y};
		draw_grid(offset, (V2){4, 4}, tileSize);
		DrawTextEx(font.font, text, text_pos, font.size, font.spacing, data->palette.green);
		if (next_piece != -1)  {
			draw_piece(next_piece , (Vector2){1,1}, 0, tileSize, offset);
		}
	}

	if (play_screen) {
		UiContainer *panel = &Container;

		UiBegin(panel);
		{
			UiText(panel, "Tetris", true);

			if (UiTextButton(panel, "Play")) { 
				play_screen = false;
			}

			// BUG  speed is modified on game loop so when playing again this will not work
			char	*mode = "should not be this";
			printf("speed: %d\n", speed);
			if (speed == 10) {
				mode = "Mode: Easy";
			}
			if (speed == 5) {
				mode = "Mode: Normal";
			}
			if (speed == 2) {
				mode = "Mode: Hard";
			}
			if (UiTextButton(panel, mode)) { 
				if (speed == 10) {
					speed = 5;
				} else if (speed == 5) {
					speed = 2;
				} else if (speed == 2) {
					speed = 10;
				}
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

	if (game_over) {
		ui_trasition_from((V2){1, 0});
		UiStates	state = game_over_screen(data);
		if (state == NONE) {
			game_over = false;
			start();
		} else if (state == TITLE_SCREEN) {
			// SAVE?
			data->current_game = MAIN_MENU;
			game_over = false;
		}
		 
		// TODO Display score & high_score
		if (score > high_score) {
			high_score = score;
			//  TODO  Custom text for new highScore
		}
	}

	static bool	options = false;
	if (paused && options == false) {
		UiContainer *panel = &Container;

		UiBegin(panel);
		{
			UiText(panel, "Game Paused", true);

			if (UiTextButton(panel, "Back to Game")) { 
				paused = false;
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
			paused = false;
		}

	} else if (paused && options) {
		UiStates	state = options_screen(data);
		if (state == BACK) {
			options = false;
		}
	}
}

static void	de_init() 
{
	free(board);
}

GameFunctions	tetris_init(GameData *game_data)
{
	pieces[0] =		"..X."
				"..X."
				"..X."
				"..X.";

	pieces[1] =		"...."
				".XX."
				".XX."
				"....";

	pieces[2] =		"..X."
				".XX."
				".X.."
				"....";

	pieces[3] =		".X.."
				".XX."
				"..X."
				"....";

	pieces[4] =		".X.."
				".X.."
				".XX."
				"....";

	pieces[5] =		"..X."
				"..X."
				".XX."
				"....";

	pieces[6] =		"..X."
				"..XX"
				"..X."
				"....";

	data = game_data;
	
	board = malloc(board_size.x * board_size.y);

	window_size = data->window_size;
	board_offset = (Vector2){window_size.x/2 - (board_size.x * tileSize)/2, window_size.y/2 - (board_size.y * tileSize)/2};
	font = data->assets.fonts[0];

	piece_colors[0] = data->palette.blue;
	piece_colors[1] = data->palette.yellow;
	piece_colors[2] = data->palette.red;
	piece_colors[3] = data->palette.green;
	piece_colors[4] = data->palette.orange;
	piece_colors[5] = data->palette.pink;
	piece_colors[6] = data->palette.purple;
	piece_colors[7] = data->palette.black;
	piece_colors[8] = data->palette.white;

	game_sounds.music = data->assets.music[0];
	game_sounds.made_line = data->assets.sounds[0];
	game_sounds.game_over = data->assets.sounds[1];

	V2	center_screen = {data->window_size.x * 0.5f, data->window_size.y * 0.25f}; // Center offset to where to start drawing text
	Container = CreateContainer(center_screen, 0, data->ui_config);
	
	return (GameFunctions) { 
		.name = "Tetris",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

static void	force_piece_down() {
	speed_count = 0;

	if (!check_piece_collision(piece, (Vector2){pos.x, pos.y + 1}, rotation)) {
		pos.y += 1;
	}
	else {
		// NOTE Writing piece to board
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				Vector2	bpos = {pos.x + x, pos.y + y};
				if (pieces[piece][rotate((Vector2){x,y}, rotation)] == 'X')
					board[(int)(bpos.y * board_size.x + bpos.x)] = piece+1;
			}
		}

		// Score counting
		int	lines_made_count = check_line_made(pos);
		score += 25;
		score += (1 << lines_made_count) * 100;

		// Initting new piece
		pos.x = board_size.x/2 - 2;
		pos.y = 0;
		rotation = 0;
		piece = next_piece;
		next_piece = rand() % 7;
		stored_piece_flag = false;

		piece_count++;
		if (piece_count % speed_increase_rate == 0 && speed >= speed_limit) {
			speed--;
		}

		game_over = check_piece_collision(piece, (Vector2){pos.x, pos.y}, rotation);
		if (game_over) {
			PlaySound(game_sounds.game_over);
		}
	}
	if (is_made_lines) {
		PlaySound(game_sounds.made_line);
		WaitTime(0.1);
		cleanup_made_lines();
	}
}

static void	clean_board()
{
	for (int y = 0; y < board_size.y; y++) {
		for (int x = 0; x < board_size.x; x++) {
			board[y * (int)board_size.x + x] = (x == 0 || x == board_size.x - 1 || y == board_size.y -1) ? 8 : 0;
		}
	}
}

static void	draw_board()
{
	for (int y = 0; y < board_size.y; y++) {
		for (int x = 0; x < board_size.x; x++) {
			Vector2	draw_pos = Vector2Scale((Vector2){x, y}, tileSize); // Add Position in the board + position inside the 4x4 of the piece
			draw_pos = Vector2Add(draw_pos, board_offset);
			int	piece = board[y * (int)board_size.x + x];
			if (piece != 0) {
				DrawRectangle(draw_pos.x, draw_pos.y, tileSize, tileSize, piece_colors[piece - 1]);
			}
		}
	}
}

static void	draw_piece(int piece, Vector2 pos, int rotation, int scale, Vector2 offset)
{
	if (piece < 0 || piece > 6) {
		TraceLog(LOG_INFO, "draw_piece_ex: invalid piece value: %d \n", piece);
		return ;
	}
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (pieces[piece][rotate((Vector2){x,y}, rotation)] != 'X')
				continue;
			Vector2	draw_pos = Vector2Add(pos,  (Vector2){x, y}); // Add Position in the board + position inside the 4x4 of the piece
			draw_pos = Vector2Scale(draw_pos, scale);
			draw_pos = Vector2Add(draw_pos, offset);
			DrawRectangle(draw_pos.x, draw_pos.y, tileSize, tileSize, piece_colors[piece]);
			//DrawRectangleLines(draw_pos.x, draw_pos.y, tile_size, tile_size, BLACK);
		}
	}
}

static int	check_line_made(Vector2 pos)
{
	int	lines_made = 0;
	for (int y = 0; y < 4; y++)
	{
		bool	line_made = true;
		if (!(pos.y + y < board_size.y - 1))//NOTE
			continue;
		for (int x = 1; x < board_size.x - 1; x++)
			if (board[(int)((pos.y + y) * board_size.x + x)] == 0)
				line_made = false;
		if (line_made)
		{
			made_lines[(int)pos.y + y] = 1;
			is_made_lines = 1;
			for (int x = 1; x < board_size.x - 1; x++)
				board[(int)((pos.y + y) * board_size.x + x)] = 9;
		}
		lines_made += line_made;
	}
	return (lines_made);
}

static void	cleanup_made_lines()
{
	for (int line = 0; line < board_size.y; line++)
	{
		if (made_lines[line] != 1)
			continue;
		for (int x = 1; x < board_size.x - 1; x++)
		{
			board[line * (int)board_size.x + x] = 0;
			for (int y = line; y > 0; y--)
				board[y * (int)board_size.x + x] = board[(y - 1) * (int)board_size.x + x];
		}
		made_lines[line] = -1;
	}
	is_made_lines = false;
}

static int	check_piece_collision(int piece, Vector2 pos, int rotation)
{
	if (piece < 0 || piece > 6) { 
		TraceLog(LOG_WARNING, "piece id passed to check_piece_collision is not valid\n");
		return (1);
	}
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++)
		{
			if (pieces[piece][rotate((Vector2){x,y}, rotation)] != 'X')
				continue;
			Vector2	check = Vector2Add(pos, (Vector2){x, y});
			if (!(check.x < 0 || check.x > board_size.x || check.y < 0 || check.y > board_size.y))
			{
				if (board[(int)(check.y * board_size.x + check.x)] != 0)
					return (1);
			}
		}
	}
	return (0);
}

static int	rotate(Vector2 pos, int r)
{
	//r = r % 4;
	if (r == 0)
		return (pos.y * 4 + pos.x);
	if (r == 1)
		return (12 + pos.y - (pos.x * 4));
	if (r == 2)
		return (15 - (pos.y * 4) - pos.x);
	if (r == 3)
		return (3 - pos.y + (pos.x * 4));
	return (0);
}
