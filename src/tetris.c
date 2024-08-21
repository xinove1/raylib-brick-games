#include "game.h"
#include "raylib.h"

# define BOARD_MAX_SIZE  (20 * 20)
global V2 BoardSize = {12, 18};

typedef struct {
	Music music;
	Sound made_line;
	Sound game_over;
} TetrisSounds;


global GameData *Data = 0;
global TetrisData *Tetris = 0;

struct TetrisData {
	byte *pieces[7];
	byte board[BOARD_MAX_SIZE];
	i32 made_lines[18]; // NOTE hard coded
	b32 is_made_lines;
	b32 paused;
	b32 game_over;
	b32 play_screen;
	b32 options_screen;
	TetrisSounds game_sounds;
	// Drawing related
	V2          board_offset;
	i32         tileSize;
	Color       piece_colors[9];
	FontConfig  font;
	UiContainer Container;
	// Game
	V2  pos; //{BoardSize.x / 2 - 2, 0};
	i32 piece;
	i32 next_piece;
	i32 stored_piece;
	b32 stored_piece_flag; // Flag to track if there already being a swap of current piece
	i32 rotation;
	i32 score;
	i32 high_score; //  TODO  Save and restore highscore
	//  FIXME  Better name for speed variables
	f32 tick_time_count; // counting time
	f32 tick_time; // How much time until a game tick in seconds
	i32 speed; // How many game ticks until force drowdown
	i32 speed_count; // counting game ticks
	i32 speed_limit; // How low the speed can get
	i32 speed_increase_rate; // How many Pieces until a speed increase
	i32 rotate_count;
	i32 rotate_rate; // Rate in ticks to rotate when holding down rotate button
	i32 piece_count; 
};

internal i32  rotate(V2 pos, i32 r);
internal i32  check_piece_collision(i32 piece, V2 pos, i32 rotation);
internal void draw();
internal void draw_piece(i32 piece, V2 pos, i32 rotation, i32 scale, V2 offset);
internal void draw_board();
internal i32  check_line_made(V2 pos);
internal void cleanup_made_lines();
internal void force_piece_down();
internal void clean_board();
internal void start();
internal void update();
internal void draw();
internal void de_init();

GameFunctions tetris_init(GameData *game_data)
{
	Data = game_data;
	game_data->tetris_data = calloc(1, sizeof(TetrisData));
	Tetris = game_data->tetris_data;
	*Tetris = (TetrisData) {
		.is_made_lines = false,
		.paused = false,
		.game_over = false,
		.play_screen = false,
		.options_screen = false,
		.tileSize = TILE_SIZE,
		.pos = {BoardSize.x / 2 - 2, 0},
		.piece = 0,
		.next_piece = 0,
		.stored_piece = -1,
		.stored_piece_flag = false,
		.rotation = 0,
		.score = 0,
		.high_score = 0, 
		.tick_time_count = 0,
		.tick_time = 0.100,
		.speed = 10, 
		.speed_count = 0,
		.speed_limit = 2,
		.speed_increase_rate = 5,
		.rotate_count = 0,
		.rotate_rate = 2, 
		.piece_count = 0, 
	};

	Tetris->pieces[0] =	"..X."
				"..X."
				"..X."
				"..X.";

	Tetris->pieces[1] =	"...."
				".XX."
				".XX."
				"....";

	Tetris->pieces[2] =	"..X."
				".XX."
				".X.."
				"....";

	Tetris->pieces[3] =	".X.."
				".XX."
				"..X."
				"....";

	Tetris->pieces[4] =	".X.."
				".X.."
				".XX."
				"....";

	Tetris->pieces[5] =	"..X."
				"..X."
				".XX."
				"....";

	Tetris->pieces[6] =	"..X."
				".XX."
				"..X."
				"....";
	
	V2 window_size = game_data->window_size;
	Tetris->board_offset = (V2){window_size.x/2 - (BoardSize.x * Tetris->tileSize)/2, window_size.y/2 - (BoardSize.y * Tetris->tileSize)/2},
	Tetris->font = Data->assets.fonts[0];

	Tetris->piece_colors[0] = Data->palette.blue;
	Tetris->piece_colors[1] = Data->palette.yellow;
	Tetris->piece_colors[2] = Data->palette.red;
	Tetris->piece_colors[3] = Data->palette.green;
	Tetris->piece_colors[4] = Data->palette.orange;
	Tetris->piece_colors[5] = Data->palette.pink;
	Tetris->piece_colors[6] = Data->palette.purple;
	Tetris->piece_colors[7] = Data->palette.black;
	Tetris->piece_colors[8] = Data->palette.white;

	Tetris->game_sounds.music = Data->assets.music[0];
	Tetris->game_sounds.made_line = Data->assets.sounds[0];
	Tetris->game_sounds.game_over = Data->assets.sounds[1];

	V2 center_screen = {window_size.x * 0.5f, window_size.y * 0.25f}; // Center offset to where to start drawing text
	Tetris->Container = UiCreateContainer(center_screen, 0, Data->ui_config);
	
	return (GameFunctions) { 
		.name = "Tetris",
		.update = &update,
		.draw = &draw,
		.start = &start,
		.de_init = &de_init,
	};
}

internal void de_init() 
{
	free(Data->tetris_data);
}

internal void start()
{
	clean_board();
	Tetris->paused = false;
	Tetris->game_over = false;
	Tetris->play_screen = true;
	Tetris->piece = rand() % 7;
	Tetris->next_piece = rand() % 7;
	Tetris->stored_piece = -1;
	PlayMusicStream(Tetris->game_sounds.music);
}

internal void update()
{
	UpdateMusicStream(Tetris->game_sounds.music);

	if (!ShouldGameRun(&Tetris->play_screen, &Tetris->paused, &Tetris->game_over)) {
		return ;
	}

	if (IsActionPressed(ACTION_3)) {
		V2 new_pos = Tetris->pos;
		while (!check_piece_collision(Tetris->piece, new_pos, Tetris->rotation)) {
			new_pos.y += 1;
		}
		Tetris->pos.y = new_pos.y - 1;
		force_piece_down();
		Tetris->tick_time_count = 0;
	}

	if (Tetris->tick_time_count < Tetris->tick_time) {
		Tetris->tick_time_count += GetFrameTime();
	} else {
		Tetris->tick_time_count = 0;
		Tetris->speed_count++;

		if (Tetris->speed_count == Tetris->speed || IsActionDown(DOWN)) 
			force_piece_down();

		if (IsActionDown(LEFT) && !check_piece_collision(Tetris->piece, (V2){Tetris->pos.x - 1, Tetris->pos.y}, Tetris->rotation)) 
			Tetris->pos = (V2) {Tetris->pos.x - 1, Tetris->pos.y};
		if (IsActionDown(RIGHT)  && !check_piece_collision(Tetris->piece, (V2){Tetris->pos.x + 1, Tetris->pos.y}, Tetris->rotation)) 
			Tetris->pos = (V2) {Tetris->pos.x + 1, Tetris->pos.y};

		if (IsActionDown(ACTION_2)) {
			if (Tetris->stored_piece == -1) {
				Tetris->stored_piece = Tetris->piece;
				Tetris->piece = Tetris->next_piece;
				Tetris->next_piece = rand() % 7;
				Tetris->stored_piece_flag = true;
			} else if (!Tetris->stored_piece_flag) {
				Tetris->stored_piece_flag = true;
				i32 tmp = Tetris->piece;
				Tetris->piece = Tetris->stored_piece;
				Tetris->stored_piece = tmp;
			}
		}
		if (IsActionDown(ACTION_1) || IsActionDown(UP)) {
			i32	new_rotation = Tetris->rotation;
			if (IsActionDown(ACTION_1)) {
				new_rotation += -1;
			}
			if (IsActionDown(UP)) {
				new_rotation += 1;
			}
			if (Tetris->piece == 0 && new_rotation == 2) {
				new_rotation = 0;
			}
			if (new_rotation == 4) {
				new_rotation = 0;
			}
			if (new_rotation == -1) {
				new_rotation = 3;
			}

			if (Tetris->rotate_count % Tetris->rotate_rate == 0 && !check_piece_collision(Tetris->piece, Tetris->pos, new_rotation)) {
				Tetris->rotation = new_rotation;
			}
			Tetris->rotate_count++;
		} else {
			Tetris->rotate_count = 0;
		}

		//  NOTE  Remove. for debug only
		if (IsKeyDown(KEY_C)) {
			Tetris->piece += 1;
			if (Tetris->piece == 7)
				Tetris->piece = 0;
		}
	}
}

internal void draw() {
	draw_grid(Tetris->board_offset, (V2){BoardSize.x -1, BoardSize.y -1}, Tetris->tileSize); 
	draw_board();
	draw_piece(Tetris->piece, Tetris->pos, Tetris->rotation, Tetris->tileSize, Tetris->board_offset);

	{
		const byte *text = TextFormat("Score: %d", Tetris->score);
		V2 text_size = MeasureTextEx(Tetris->font.font, text, Tetris->font.size, Tetris->font.spacing);
		V2 offset = {Tetris->board_offset.x - Tetris->tileSize * 5, Tetris->board_offset.y};
		
		
		V2 text_pos = {offset.x, offset.y - text_size.y};
		draw_grid(offset, (V2){4, 4}, Tetris->tileSize);
		DrawRectangle(offset.x, offset.y, Tetris->tileSize, Tetris->tileSize, ColorAlpha(RED, 0.2));
		DrawTextEx(Tetris->font.font, text, text_pos, Tetris->font.size, Tetris->font.spacing, Data->palette.green);
		if (Tetris->stored_piece != -1)  {
			draw_piece(Tetris->stored_piece, (V2){1,1}, 0, Tetris->tileSize, offset);
		}
	}

	{
		const byte *text = "Coming up:";
		V2 text_size = MeasureTextEx(Tetris->font.font, text, Tetris->font.size, Tetris->font.spacing);
		V2 offset = {Tetris->board_offset.x + BoardSize.x * Tetris->tileSize, Tetris->board_offset.y};
		
		
		V2 text_pos = {offset.x, offset.y - text_size.y};
		draw_grid(offset, (V2){4, 4}, Tetris->tileSize);
		DrawTextEx(Tetris->font.font, text, text_pos, Tetris->font.size, Tetris->font.spacing, Data->palette.green);
		if (Tetris->next_piece != -1)  {
			draw_piece(Tetris->next_piece , (V2){1,1}, 0, Tetris->tileSize, offset);
		}
	}

	if (Tetris->play_screen) {
		UiContainer *panel = &Tetris->Container;

		UiBegin(panel);
		{
			UiText(panel, "Tetris", true);

			if (UiTextButton(panel, "Play")) { 
				Tetris->play_screen = false;
			}

			// BUG  speed is modified on game loop so when playing again this will not work
			byte *mode = "should not be this";
			printf("speed: %d\n", Tetris->speed);
			if (Tetris->speed == 10) {
				mode = "Mode: Easy";
			}
			if (Tetris->speed == 5) {
				mode = "Mode: Normal";
			}
			if (Tetris->speed == 2) {
				mode = "Mode: Hard";
			}
			if (UiTextButton(panel, mode)) { 
				if (Tetris->speed == 10) {
					Tetris->speed = 5;
				} else if (Tetris->speed == 5) {
					Tetris->speed = 2;
				} else if (Tetris->speed == 2) {
					Tetris->speed = 10;
				}
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

	if (Tetris->game_over) {
		ui_trasition_from((V2){1, 0});
		UiStates state = game_over_screen(Data);
		if (state == NONE) {
			Tetris->game_over = false;
			start();
		} else if (state == TITLE_SCREEN) {
			// SAVE?
			Data->current_game = MAIN_MENU;
			Tetris->game_over = false;
		}
		 
		// TODO Display score & high_score
		if (Tetris->score > Tetris->high_score) {
			Tetris->high_score = Tetris->score;
			//  TODO  Custom text for new highScore
		}
	}

	if (Tetris->paused && Tetris->options_screen == false) {
		UiContainer *panel = &Tetris->Container;

		UiBegin(panel);
		{
			UiText(panel, "Game Paused", true);

			if (UiTextButton(panel, "Back to Game")) { 
				Tetris->paused = false;
			} 

			if (UiTextButton(panel, "Options")) { 
				Tetris->options_screen = true;
			}

			if (UiTextButton(panel, "Exit To Main Menu")) { 
				Data->current_game = MAIN_MENU;
			}

			if (UiTextButton(panel, "Exit To Desktop")) { 
				Data->quit = true;
			}
		}
		UiEnd(panel);
		if (IsActionPressed(ACTION_2)) {
			Tetris->paused = false;
		}

	} else if (Tetris->paused && Tetris->options_screen) {
		UiStates state = options_screen(Data);
		if (state == BACK) {
			Tetris->options_screen = false;
		}
	}
}

internal void force_piece_down() {
	Tetris->speed_count = 0;

	if (!check_piece_collision(Tetris->piece, (V2){Tetris->pos.x, Tetris->pos.y + 1}, Tetris->rotation)) {
		Tetris->pos.y += 1;
	} else {
		// NOTE Writing piece to board
		for (i32 y = 0; y < 4; y++) {
			for (i32 x = 0; x < 4; x++) {
				V2 bpos = {Tetris->pos.x + x, Tetris->pos.y + y};
				if (Tetris->pieces[Tetris->piece][rotate((V2){x,y}, Tetris->rotation)] == 'X')
					Tetris->board[(i32)(bpos.y * BoardSize.x + bpos.x)] = Tetris->piece+1;
			}
		}

		// Score counting
		i32 lines_made_count = check_line_made(Tetris->pos);
		Tetris->score += 25;
		Tetris->score += (1 << lines_made_count) * 100;

		// Initting new piece
		Tetris->pos.x = BoardSize.x/2 - 2;
		Tetris->pos.y = 0;
		Tetris->rotation = 0;
		Tetris->piece = Tetris->next_piece;
		Tetris->next_piece = rand() % 7;
		Tetris->stored_piece_flag = false;

		Tetris->piece_count++;
		if (Tetris->piece_count % Tetris->speed_increase_rate == 0 && Tetris->speed >= Tetris->speed_limit) {
			Tetris->speed--;
		}

		Tetris->game_over = check_piece_collision(Tetris->piece, (V2){Tetris->pos.x, Tetris->pos.y}, Tetris->rotation);
		if (Tetris->game_over) {
			PlaySound(Tetris->game_sounds.game_over);
		}
	}
	if (Tetris->is_made_lines) {
		PlaySound(Tetris->game_sounds.made_line);
		WaitTime(0.1);
		cleanup_made_lines();
	}
}

internal void clean_board()
{
	for (i32 y = 0; y < BoardSize.y; y++) {
		for (i32 x = 0; x < BoardSize.x; x++) {
			Tetris->board[y * (i32)BoardSize.x + x] = (x == 0 || x == BoardSize.x - 1 || y == BoardSize.y -1) ? 8 : 0;
		}
	}
}

internal void draw_board()
{
	for (i32 y = 0; y < BoardSize.y; y++) {
		for (i32 x = 0; x < BoardSize.x; x++) {
			V2 draw_pos = V2Scale((V2){x, y}, Tetris->tileSize); // Add Position in the board + position inside the 4x4 of the piece
			draw_pos = V2Add(draw_pos, Tetris->board_offset);
			i32 piece = Tetris->board[y * (i32)BoardSize.x + x];
			if (piece != 0) {
				DrawRectangle(draw_pos.x, draw_pos.y, Tetris->tileSize, Tetris->tileSize, Tetris->piece_colors[piece - 1]);
			}
		}
	}
}

internal void draw_piece(i32 piece, V2 pos, i32 rotation, i32 scale, V2 offset)
{
	if (piece < 0 || piece > 6) {
		TraceLog(LOG_INFO, "draw_piece_ex: invalid piece value: %d \n", piece);
		return ;
	}
	for (i32 y = 0; y < 4; y++) {
		for (i32 x = 0; x < 4; x++) {
			if (Tetris->pieces[piece][rotate((V2){x,y}, rotation)] != 'X')
				continue;
			V2 draw_pos = V2Add(pos,  (V2){x, y}); // Add Position in the board + position inside the 4x4 of the piece
			draw_pos = V2Scale(draw_pos, scale);
			draw_pos = V2Add(draw_pos, offset);
			DrawRectangle(draw_pos.x, draw_pos.y, Tetris->tileSize, Tetris->tileSize, Tetris->piece_colors[piece]);
			//DrawRectangleLines(draw_pos.x, draw_pos.y, tile_size, tile_size, BLACK);
		}
	}
}

internal i32 check_line_made(V2 pos)
{
	i32 lines_made = 0;
	for (i32 y = 0; y < 4; y++) {
		b32 line_made = true;
		if (!(pos.y + y < BoardSize.y - 1))//NOTE
			continue;
		for (i32 x = 1; x < BoardSize.x - 1; x++) {
			if (Tetris->board[(i32)((pos.y + y) * BoardSize.x + x)] == 0)
				line_made = false;
		}
		if (line_made) {
			Tetris->made_lines[(i32)pos.y + y] = 1;
			Tetris->is_made_lines = 1;
			for (i32 x = 1; x < BoardSize.x - 1; x++) {
				Tetris->board[(i32)((pos.y + y) * BoardSize.x + x)] = 9;
			}
		}
		lines_made += line_made;
	}
	return (lines_made);
}

internal void cleanup_made_lines()
{
	for (i32 line = 0; line < BoardSize.y; line++) {
		if (Tetris->made_lines[line] != 1) continue;

		for (i32 x = 1; x < BoardSize.x - 1; x++) {
			Tetris->board[line * (i32)BoardSize.x + x] = 0;
			for (i32 y = line; y > 0; y--) {
				Tetris->board[y * (i32)BoardSize.x + x] = Tetris->board[(y - 1) * (i32)BoardSize.x + x];
			}
		}
		Tetris->made_lines[line] = -1;
	}
	Tetris->is_made_lines = false;
}

internal i32 check_piece_collision(i32 piece, V2 pos, i32 rotation)
{
	if (piece < 0 || piece > 6) { 
		TraceLog(LOG_WARNING, "piece id passed to check_piece_collision is not valid\n");
		return (1);
	}

	for (i32 y = 0; y < 4; y++) {
		for (i32 x = 0; x < 4; x++) {
			if (Tetris->pieces[piece][rotate((V2){x,y}, rotation)] != 'X') continue;

			V2 check = V2Add(pos, (V2){x, y});
			if (!(check.x < 0 || check.x > BoardSize.x || check.y < 0 || check.y > BoardSize.y)) {
				if (Tetris->board[(i32)(check.y * BoardSize.x + check.x)] != 0)
					return (1);
			}
		}
	}
	return (0);
}

internal i32 rotate(V2 pos, i32 r)
{
	//r = r % 4;
	if (r == 0) return (pos.y * 4 + pos.x);
	if (r == 1) return (12 + pos.y - (pos.x * 4));
	if (r == 2) return (15 - (pos.y * 4) - pos.x);
	if (r == 3) return (3 - pos.y + (pos.x * 4));
	return (0);
}
