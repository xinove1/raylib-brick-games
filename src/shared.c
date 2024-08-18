#include "game.h"
#include "raylib.h"
#include "raymath_short.h"
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#ifdef PLATFORM_WEB
	#include <emscripten/emscripten.h>
	extern void save_scores(void *ptr, i32 size, i32 padding);
	extern void load_scores(void *ptr, i32 size, i32 padding);
	extern void save_data(cstr *where, void *ptr, i32 size);
	extern void load_data(cstr *where, void *ptr, i32 size);
#endif

V2 ExpDecayV2(V2 a, V2 b, f32 decay) 
{
	V2 d = V2Subtract(a, b);
	d = V2Scale(d, expf(-decay * GetFrameTime()));
	return V2Add(b, d);
}

// Crate Rectangle from to Vectors
inline Rect RectV2(V2 pos, V2 size) {
	return ((Rect) {.x = pos.x, .y = pos.y, .width = size.x, .height = size.y});
}

inline V2 V2RectPos(Rect rect) {
	return ((V2) {rect.x, rect.y});
}
inline V2 V2RectSize(Rect rect) {
	return ((V2) {rect.width, rect.height});
}

void draw_grid(V2 position, V2 grid_size, i32 tile_size)
{
	draw_grid_ex(position, grid_size, tile_size, 1, ColorAlpha(BLACK, 0.1));
}

void draw_grid_ex(V2 position, V2 grid_size, i32 tile_size, f32 line_thickness, Color color) 
{
	for (i32 y = 0; y < grid_size.y + 1; y++) {
		V2 pos = {position.x, position.y + y * (tile_size )};
		V2 end_pos = {position.x + (grid_size.x * tile_size), position.y + y * (tile_size )};
		DrawLineEx(pos, end_pos, line_thickness, color);
	}
	for (i32 x = 0; x < grid_size.x + 1; x++) {
		V2 pos = {position.x + x * tile_size, position.y};
		V2 end_pos = {position.x + x * tile_size, position.y + grid_size.y * tile_size};
		if (x == 0) {
			// printf("position: %f, %f \n", position.x, position.y);
			// printf("pos: %f, %f \n", pos.x, pos.y);
		}
		DrawLineEx(pos, end_pos, line_thickness, color);
	}
}

// idk
b32 ShouldGameRun(b32 *play_screen, b32 *paused, b32 *game_over)
{
	if (play_screen == NULL || paused == NULL || game_over == NULL) {
		TraceLog(LOG_WARNING, "ShouldGameRun: was provided with a null value, will always return true\n");
		return (true);
	}

	if (!*play_screen && !*game_over) {
		if (IsActionPressed(OPEN_MENU)) { 
			*paused = *paused ? false : true;
		}
		if (!IsWindowFocused() || IsPauseRequested()) {
			*paused = true;
		}
	}

	if (*play_screen || *paused || *game_over) {
		return (false);
	}

	return (true);
}

HighScores LoadScores(cstr *where) 
{
	#ifdef PLATFORM_WEB
		HighScores scores = {0};
		load_data(where, &scores, sizeof(scores));
		return (scores);
	#else
		i32 fd = open(where, O_RDONLY);
		if (fd < 0) {
			TraceLog(LOG_WARNING, "LoadData: %s. \n", strerror(errno));
			return ((HighScores) {0});
		}
		byte *buf[sizeof(HighScores)];
		i32 amount_read = read(fd, buf, sizeof(HighScores));
		if (amount_read < 0) {
			TraceLog(LOG_WARNING, "LoadData: %s. \n", strerror(errno));
			return ((HighScores) {0});
		}
		HighScores scores = *(HighScores *)buf;
		return (scores);
	#endif
}

b32 SaveScores(cstr *where, HighScores scores) 
{
	#ifdef PLATFORM_WEB
		save_data(where, &scores, sizeof(scores));
		return (true);
	#else
		i32 fd = open(where, O_CREAT | O_WRONLY, 448);
		if (fd < 0) {
			TraceLog(LOG_WARNING, "SaveData: %s. \n", strerror(errno));
			return (false);
		}
		i32 amount_written = write(fd, (byte *)&scores, sizeof(scores));
		if (amount_written < 0) {
			TraceLog(LOG_WARNING, "SaveData: %s. \n", strerror(errno));
			return (false);
		}
		return (true);
	#endif
}

