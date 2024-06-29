#include "game.h"
#define XI_INPUT_ACTIONS_IMPLEMENTATION
#define XI_UI_IMPLEMENTATION 
#include "ui.h"
#include "input.h"

void	_Testfunc(char *str, GameData data) 
{
	printf("str: %s, data- quit: %d, music_vol: %f\n", str, data.quit, data.music_vol);
}
#define Testfunc(str, ...) _Testfunc((str), (GameData){.quit = false, __VA_ARGS__})

static void	load_assets(GameData *data);
static void	unload_assets(GameData *data);

static GameFunctions	games[GAME_COUNT] = {0};
static Games_e	game;
static RenderTexture2D	screen;

int	main()
{
	GameData	data = {0};

	data.window_size = (V2) {640, 360};
	data.music_vol = 0.5f;
	data.effects_vol = 0.3f;
	data.assets = (Assets) {0};
	data.palette = (ColorPalette) {
		.black = {0, 0, 0, 255},
		.white = {233, 228, 205, 255},
		.red   = {249, 39, 0, 255},
		.pink  = {211, 0, 92, 255},
		.blue  = {49, 21, 172, 255},
		.yellow= {224, 245, 0, 255},
		.green = {249, 111, 0, 255},
		.orange= {244, 93, 1, 255},
		.purple= {130, 5, 165, 255},
		.background = {237, 191, 198, 255},
	};
	data.current_game = MAIN_MENU;
	game = -1;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED );
	InitWindow(data.window_size.x, data.window_size.y, "Raylib Bricks games");
	SetWindowState(FLAG_WINDOW_MAXIMIZED);
	InitAudioDevice();
	SetTargetFPS(60);
	SetExitKey(0);

	screen = LoadRenderTexture(data.window_size.x, data.window_size.y);
	//SetTextureFilter(screen.texture, TEXTURE_FILTER_BILINEAR);  
	SetTextureFilter(screen.texture, TEXTURE_FILTER_ANISOTROPIC_16X);  

	{
		SetGamePadId(0);

		RegisterActionName(RIGHT, "right");
		RegisterInputKeyAction(RIGHT, KEY_D);
		RegisterInputKeyAction(RIGHT, KEY_RIGHT);
		RegisterGamePadButtonAction(RIGHT, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
		RegisterGamePadAxisAction(RIGHT, GAMEPAD_AXIS_LEFT_X, 0.5f);


		RegisterActionName(LEFT, "left");
		RegisterInputKeyAction(LEFT, KEY_A);
		RegisterInputKeyAction(LEFT, KEY_LEFT);
		RegisterGamePadButtonAction(LEFT, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
		RegisterGamePadAxisAction(LEFT, GAMEPAD_AXIS_LEFT_X, -0.5f);


		RegisterActionName(UP, "up");
		RegisterInputKeyAction(UP, KEY_W);
		RegisterInputKeyAction(UP, KEY_UP);
		RegisterGamePadButtonAction(UP, GAMEPAD_BUTTON_LEFT_FACE_UP);
		RegisterGamePadAxisAction(UP, GAMEPAD_AXIS_LEFT_Y, -0.5f);


		RegisterActionName(DOWN, "down");
		RegisterInputKeyAction(DOWN, KEY_S);
		RegisterInputKeyAction(DOWN, KEY_DOWN);
		RegisterGamePadButtonAction(DOWN, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
		RegisterGamePadAxisAction(DOWN, GAMEPAD_AXIS_LEFT_Y, 0.5f);


		RegisterActionName(ACTION_1, "action_1");
		RegisterInputKeyAction(ACTION_1, KEY_J);
		RegisterInputKeyAction(ACTION_1, KEY_X);
		RegisterGamePadButtonAction(ACTION_1, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
		RegisterGamePadAxisAction(ACTION_1, GAMEPAD_AXIS_RIGHT_TRIGGER, 0.7f);


		RegisterActionName(ACTION_2, "action_2");
		RegisterInputKeyAction(ACTION_2, KEY_K);
		RegisterInputKeyAction(ACTION_2, KEY_Z);
		RegisterGamePadButtonAction(ACTION_2, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);


		RegisterActionName(ACTION_3, "action_3");
		RegisterInputKeyAction(ACTION_3, KEY_SPACE);
		RegisterGamePadButtonAction(ACTION_3, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);


		RegisterActionName(OPEN_MENU, "open_menu");
		RegisterInputKeyAction(OPEN_MENU, KEY_ESCAPE);
		RegisterInputKeyAction(OPEN_MENU, KEY_E);
		RegisterGamePadButtonAction(OPEN_MENU, GAMEPAD_BUTTON_MIDDLE_RIGHT);
	}

	load_assets(&data);
	update_volume(&data);

	set_selector_texture(&data.assets.textures[0]);
	set_clicked_sound(&data.assets.sounds[2]);

	games[SNAKE_GAME] = snake_game_init(&data);
	games[TETRIS] = tetris_init(&data);
	games[MAIN_MENU] = main_menu_init(&data);
	games[PONG] = pong_init(&data);
	games[BREAKOUT] = breakout_init(&data);
	games[TEST] = test_game_init(&data);

	while (!WindowShouldClose() && !data.quit) {
		PoolActions();
		//printf("MouseMoving %d | WasInput %d \n", IsMouseMoving(), WasAnyActionDown());

		float screen_scale = MIN((float)GetScreenWidth()/data.window_size.x, (float)GetScreenHeight() / data.window_size.y);
		// Update virtual mouse (clamped mouse value behind game screen)
		Vector2 mouse = GetMousePosition();
		Vector2 virtualMouse = { 0 };
		virtualMouse.x = (mouse.x - (GetScreenWidth() - (data.window_size.x *screen_scale))*0.5f)/screen_scale;
		virtualMouse.y = (mouse.y - (GetScreenHeight() - (data.window_size.y *screen_scale))*0.5f)/screen_scale;
		virtualMouse = Vector2Clamp(virtualMouse, (Vector2){ 0, 0 }, (Vector2){ (float)data.window_size.x, (float)data.window_size.y});

		//Apply the same transformation as the virtual mouse to the real mouse (i.e. to work with raygui)
		SetMouseOffset(-(GetScreenWidth() - (data.window_size.x*screen_scale))*0.5f, -(GetScreenHeight() - (data.window_size.y*screen_scale))*0.5f);
		SetMouseScale(1 / screen_scale, 1 / screen_scale);


		if (data.current_game < 0 || data.current_game >= GAME_COUNT) {
//			TraceLog(LOG_INFO, "main.c: Invalid option for current_game, won't change.\n");
		}
		else if (data.current_game != game) {
			printf("changed game\n");
			game = data.current_game;
			games[game].start();
		}

		games[game].update();

		BeginTextureMode(screen);
		{
			ClearBackground(RAYWHITE);
			games[game].draw();
		}
		EndTextureMode();

		BeginDrawing();
		{
			ClearBackground(BLACK);
			// Draw render texture to screen, properly scaled
			DrawTexturePro(screen.texture,
		  (Rect){0.0f, 0.0f, (float) screen.texture.width, (float) -screen.texture.height},
		  (Rect){(GetScreenWidth() - ((float) data.window_size.x*screen_scale)) * 0.5f, (GetScreenHeight() - ((float) data.window_size.y*screen_scale)) * 0.5f,
		  (float)data.window_size.x * screen_scale, (float)data.window_size.y * screen_scale },
		  (Vector2){ 0, 0 },
		  0.0f,
			WHITE);
			FontConfig	font = data.assets.fonts[1];
			DrawTextEx(font.font, TextFormat("%d", GetFPS()), (V2){30, 30}, font.size, font.spacing, font.tint);
		}
		EndDrawing();

	}

	games[TETRIS].de_init();
	games[SNAKE_GAME].de_init();
	games[PONG].de_init();
	games[BREAKOUT].de_init();
	games[TEST].de_init();
	games[MAIN_MENU].de_init();

	unload_assets(&data);

	CloseWindow();
	CloseAudioDevice();
	return (0);
}

void	update_volume(GameData *data)
{
	for (int i = 0; i < MAX_ASSET; i++) {
		SetMusicVolume(data->assets.music[i], data->music_vol);
		SetSoundVolume(data->assets.sounds[i], data->effects_vol);
	}
}

static void	load_assets(GameData *data) {
	data->assets.music[0] = LoadMusicStream("./assets/retro_comedy.ogg");
	data->assets.sounds[0] = LoadSound("./assets/upgrade4.ogg");
	data->assets.sounds[1] = LoadSound("./assets/gameover3.ogg");
	data->assets.sounds[2] = LoadSound("./assets/select_007.ogg");

	data->assets.textures[0] = LoadTexture("./assets/arrow_e.png");

	data->assets.fonts[0] = (FontConfig) {
		.font = LoadFontEx("./assets/kenney_future_square.ttf", 20, NULL, 0),
		.size = 20,
		.spacing = 2,
		.tint = data->palette.red,
		.tint_hover = data->palette.black,
		};
	data->assets.fonts[1] = (FontConfig) {
		.font = LoadFontEx("./assets/PixeloidSans-Bold.ttf", 22, NULL, 0),
		.size = 22,
		.spacing = 2,
		.tint = data->palette.red,
		.tint_hover = data->palette.black,
		};
	data->assets.fonts[2] = (FontConfig) {
		.font = LoadFontEx("./assets/kenney_future_square.ttf", 40, NULL, 0),
		.size = 40,
		.spacing = 2,
		.tint = data->palette.red,
		.tint_hover = data->palette.black,
	};

}

static void	unload_assets(GameData *data) {
	for (int i = 0; i < MAX_ASSET; i++) {
		UnloadSound(data->assets.sounds[i]);
		UnloadMusicStream(data->assets.music[i]);
		UnloadFont(data->assets.fonts[i].font);
	}
}

// TODO change raylib window flags to window unfocused
void	pause_game() 
{
	printf("Pause game called\n");
}
