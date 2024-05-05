#include "raylib.h"
#include "raymath.h"
#include "input.h"
#include "raygui.h"
#include "style_candy.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	float	effects_volume;
	float	music_volume;
	Music	music;
	Sound	made_line;
	Sound	game_over;
} GameSounds;

Vector2	window_size;
char	*pieces[7] = {0};
const Vector2	board_size = {12, 18};
char	*board = NULL;
int	made_lines[18] = {0}; // NOTE hard coded
bool	is_made_lines = false;
bool	paused = false;
bool	game_over = false;
bool	quit_game = false;
GameSounds	game_sounds = {1, 1, 0, 0, 0};

// Drawing related
Vector2	board_offset = {0, 0};
int	tile_size = 16;
Color	piece_colors[9] = {BLUE, YELLOW, RED, GREEN, ORANGE, PINK, PURPLE, BLACK, DARKPURPLE};
Font	font;
Vector2	stored_piece_offset;

// Game
int	rotate_count = 0;
int	rotate_rate = 3; // Rate in ticks to rotate when holding down rotate button

Vector2	pos = {board_size.x / 2 - 2, 0};
int	piece = 0;
int	next_piece = 0;
int	stored_piece = -1;
bool	stored_piece_flag = false; // Flag to track if there already being a swap of current piece
int	rotation = 0;


int	score = 0;
int	high_score = 0; //  TODO  Save and restore highscore

//  FIXME  Better name for speed variables
int	speed = 20; // How fast the pieces drop
int	speed_count = 0;
int	speed_limit = 5; // How low the speed can get
int	speed_increase_rate = 5; // How many Pieces until a speed increase
int	piece_count = 0; 
float	frame_count = 0; 
int	tick_time = 5; // How many frames until a game tick

int	rotate(Vector2 pos, int r);
int	check_piece_collision(int piece, Vector2 pos, int rotation);
void	draw_piece(int piece, Vector2 pos, int rotation);
void	draw_piece_ex(int piece, Vector2 pos, int rotation, int scale, Vector2 offset);
void	draw_board();
int	check_line_made(Vector2 pos);
void	cleanup_made_lines();
void	force_piece_down();
void	draw_config_window();
void	clean_board();
void	pause_game();

static int	update()
{
	pieces[0] = "..X."
				"..X."
				"..X."
				"..X.";

	pieces[1] = "...."
				".XX."
				".XX."
				"....";

	pieces[2] = "..X."
				".XX."
				".X.."
				"....";

	pieces[3] = ".X.."
				".XX."
				"..X."
				"....";

	pieces[4] = ".X.."
				".X.."
				".XX."
				"....";

	pieces[5] = "..X."
				"..X."
				".XX."
				"....";

	pieces[6] = "..X."
				"..XX"
				"..X."
				"....";
	
	board = malloc(board_size.x * board_size.y);
	clean_board();

	window_size = (Vector2){600, 600};
	board_offset = (Vector2){window_size.x/2 - (board_size.x * tile_size)/2, window_size.y/2 - (board_size.y * tile_size)/2};
	stored_piece_offset = (Vector2){board_offset.x - 100, board_offset.y};

	InitWindow(window_size.x, window_size.y, "Tetris");
	InitAudioDevice();
	SetTargetFPS(60);
	SetExitKey(0);

	font = LoadFont("./assets/kenney_blocks.ttf");
	game_sounds.music = LoadMusicStream("./assets/retro_comedy.ogg");
	game_sounds.game_over = LoadSound("./assets/gameover3.ogg");
	game_sounds.made_line = LoadSound("./assets/upgrade4.ogg");

	PlayMusicStream(game_sounds.music);
	SetMusicVolume(game_sounds.music, game_sounds.music_volume);
	SetSoundVolume(game_sounds.made_line, game_sounds.effects_volume);
	SetSoundVolume(game_sounds.game_over, game_sounds.effects_volume);

	GuiLoadStyleCandy();

	register_input_action("right", KEY_D);
	register_input_action("right", KEY_RIGHT);
	register_input_action("left", KEY_A);
	register_input_action("left", KEY_LEFT);
	register_input_action("down", KEY_S);
	register_input_action("down", KEY_DOWN);
	register_input_action("rotate", KEY_J);
	register_input_action("rotate", KEY_X);
	register_input_action("store_piece", KEY_K);
	register_input_action("store_piece", KEY_Z);
	register_input_action("open_menu", KEY_ESCAPE);
	register_input_action("open_menu", KEY_E);

	piece = rand() % 7;
	next_piece = rand() % 7;
	stored_piece = -1;

	while (!WindowShouldClose() && !quit_game)
	{
		UpdateMusicStream(game_sounds.music);

		// if (!IsWindowFocused()) {
		// 	printf("windows not focused\n");
		// }
		if (IsWindowMinimized()) {
			printf("windows minimized\n");
		}

		if (!paused && !IsWindowFocused()) {
			paused = true;
		}
		if (IsActionPressed("open_menu")) {
			paused = paused ? false : true;
		}

		if (frame_count < tick_time)
			frame_count++;
		else if (!paused && !game_over) { // Game Logic
			frame_count = 0;

			speed_count++;
			//pos = (Vector2) {pos.x, pos.y + 1};
			if (speed_count == speed || IsActionDown("down")) 
				force_piece_down();

			if (IsActionDown("left") && !check_piece_collision(piece, (Vector2){pos.x - 1, pos.y}, rotation)) 
				pos = (Vector2) {pos.x - 1, pos.y};
			if (IsActionDown("right")  && !check_piece_collision(piece, (Vector2){pos.x + 1, pos.y}, rotation)) 
				pos = (Vector2) {pos.x + 1, pos.y};

			if (IsActionDown("store_piece")) {
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
			if (IsActionDown("rotate")) {
				if (rotate_count % rotate_rate == 0 && !check_piece_collision(piece, (Vector2){pos.x, pos.y}, rotation + 1)) {
					rotation += 1;
					if (piece == 0 && rotation == 2) {
						rotation = 0;
					}
					if (rotation == 4) {
						rotation = 0;
					}
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
		// Drawing
		BeginDrawing();
		ClearBackground(RAYWHITE);
		draw_board();
		draw_piece(piece, pos, rotation);

		const char	*score_text = TextFormat("Score: %d", score);
		MeasureTextEx(font, score_text, 18, 3);
		DrawTextEx(font, score_text, stored_piece_offset, 18, 3, GREEN);
		if (stored_piece != -1) 
			draw_piece_ex(stored_piece, (Vector2){1,1}, 0, tile_size, stored_piece_offset);

		if (paused) {
			draw_config_window();
		} else if (game_over) {
			// TODO Display score & high_score
			if (score > high_score) {
				high_score = score;
				//  TODO  Custom text for new highScore
			}
			DrawRectangleV((Vector2){0,0}, window_size, (Color){ 100, 100, 100, 100}); 
			char	*str = "Game Over";
			Vector2	str_size = MeasureTextEx(font, str, 30, 4);
			Vector2	pos = (Vector2){window_size.x / 2 - str_size.x / 2, window_size.y / 2 - str_size.y / 2};
			DrawRectangleV(pos, str_size, (Color){ 100, 200, 100, 255}); 
			DrawTextEx(font, str, pos, 30, 4, RED);
		}

		EndDrawing();
	}

	UnloadMusicStream(game_sounds.music);
	UnloadSound(game_sounds.game_over);
	UnloadSound(game_sounds.made_line);

	CloseWindow();
	CloseAudioDevice();
	free(board);
	UnloadFont(font);
	return (0);
}

void draw_config_window() {
	DrawRectangleV((Vector2){0,0}, window_size, (Color){ 100, 100, 100, 100}); 
	char	*str = "Game Paused";
	Vector2	str_size = MeasureTextEx(font, str, 30, 4);
	Vector2	offset = {window_size.x / 2 - str_size.x / 2, window_size.y / 2 - str_size.y / 2};
	Rectangle	rect = {offset.x , offset.y + 50, str_size.x, str_size.y};
	//GuiWindowBox((Rectangle){offset.x. offset.y, ;
	DrawRectangleV(offset, str_size, (Color){ 100, 200, 100, 255}); 
	DrawTextEx(font, str, offset, 30, 4, RED);
	if (GuiSliderBar(rect, NULL, NULL, &game_sounds.music_volume, 0.0, 1.0)) { 
		SetMusicVolume(game_sounds.music, game_sounds.music_volume);
	}
	rect = (Rectangle){rect.x, rect.y + 25, rect.width, rect.height};
	if (GuiSliderBar(rect, NULL, NULL, &game_sounds.effects_volume, 0.0, 1.0)) { 
		SetSoundVolume(game_sounds.made_line, game_sounds.effects_volume);
		SetSoundVolume(game_sounds.game_over, game_sounds.effects_volume);
	}
}

void	force_piece_down() {
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

void	clean_board()
{
	for (int y = 0; y < board_size.y; y++) {
		for (int x = 0; x < board_size.x; x++) {
			board[y * (int)board_size.x + x] = (x == 0 || x == board_size.x - 1 || y == board_size.y -1) ? 8 : 0;
		}
	}
}

void	draw_board()
{
	for (int y = 0; y < board_size.y; y++)
	{
		for (int x = 0; x < board_size.x; x++)
		{
			Vector2	draw_pos = Vector2Scale((Vector2){x, y}, tile_size); // Add Position in the board + position inside the 4x4 of the piece
			draw_pos = Vector2Add(draw_pos, board_offset);

			int	piece = board[y * (int)board_size.x + x];
			if (piece != 0)
				DrawRectangle(draw_pos.x, draw_pos.y, tile_size, tile_size, piece_colors[piece - 1]);
		}
	}
}

void	draw_piece(int piece, Vector2 pos, int rotation)
{
	draw_piece_ex(piece, pos, rotation, tile_size, board_offset);
	return;
	if (piece < 0 && piece > 6)
		return ;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (pieces[piece][rotate((Vector2){x,y}, rotation)] != 'X')
				continue;
			Vector2	draw_pos = Vector2Add(pos,  (Vector2){x, y}); // Add Position in the board + position inside the 4x4 of the piece
			draw_pos = Vector2Scale(draw_pos, tile_size);
			draw_pos = Vector2Add(draw_pos, board_offset);
			DrawRectangle(draw_pos.x, draw_pos.y, tile_size, tile_size, piece_colors[piece]);
			//DrawRectangleLines(draw_pos.x, draw_pos.y, tile_size, tile_size, BLACK);
		}
	}
}

void	draw_piece_ex(int piece, Vector2 pos, int rotation, int scale, Vector2 offset)
{
	if (piece < 0 && piece > 6)
		return ;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (pieces[piece][rotate((Vector2){x,y}, rotation)] != 'X')
				continue;
			Vector2	draw_pos = Vector2Add(pos,  (Vector2){x, y}); // Add Position in the board + position inside the 4x4 of the piece
			draw_pos = Vector2Scale(draw_pos, scale);
			draw_pos = Vector2Add(draw_pos, offset);
			DrawRectangle(draw_pos.x, draw_pos.y, tile_size, tile_size, piece_colors[piece]);
			//DrawRectangleLines(draw_pos.x, draw_pos.y, tile_size, tile_size, BLACK);
		}
	}
}

int	check_line_made(Vector2 pos)
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

void	cleanup_made_lines()
{
	for (int line = 0; line < board_size.y; line++)
	{
		//printf("made_lines:%d\n", made_lines[line]);
		if (made_lines[line] != 1)
			continue;
		for (int x = 1; x < board_size.x - 1; x++)
		{
		//	printf("line:%d, x:%d\n", line, x);
			board[line * (int)board_size.x + x] = 0;
			for (int y = line; y > 0; y--)
				board[y * (int)board_size.x + x] = board[(y - 1) * (int)board_size.x + x];
		}
		made_lines[line] = -1;
	}
	is_made_lines = false;
}

int	check_piece_collision(int piece, Vector2 pos, int rotation)
{
	if (piece < 0 && piece > 6)
		return (1);
	for (int y = 0; y < 4; y++)
	{
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

int	rotate(Vector2 pos, int r)
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

void	pause_game() {
	printf("Pause game called\n");
	paused = true;
}
