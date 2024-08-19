#include "game.h"

#ifdef PLATFORM_WEB
	#include <emscripten/emscripten.h>
#endif

internal_inline void load_assets(GameData *data);
internal_inline void unload_assets(GameData *data);
internal_inline void register_key_actions();
internal void update_and_draw(void);

global GameFunctions Gamesfuncs[GAME_COUNT] = {0};
global Games_e Game;
global RenderTexture2D ScreenTexture;
global GameData Data = {0};
global b32 PauseRequested = false;

i32 main()
{

	Data.window_size = (V2) {640, 360};
	Data.music_vol = 0.5f;
	Data.effects_vol = 0.3f;
	Data.assets = (Assets) {0};
	Data.palette = (ColorPalette) {
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
	Data.current_game = MAIN_MENU;
	Game = -1;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MAXIMIZED );
	InitWindow(Data.window_size.x, Data.window_size.y, "Bricks games");
	SetWindowState(FLAG_WINDOW_MAXIMIZED);
	InitAudioDevice();
	SetTargetFPS(60);
	SetExitKey(0);

	ScreenTexture = LoadRenderTexture(Data.window_size.x, Data.window_size.y);
	//SetTextureFilter(screen.texture, TEXTURE_FILTER_BILINEAR);  
	SetTextureFilter(ScreenTexture.texture, TEXTURE_FILTER_ANISOTROPIC_16X);  

	register_key_actions();
	load_assets(&Data);

	update_volume(&Data);

	Data.ui_config = (UiConfig) {
		.alignment = UiAlignCentralized,
		.font = Data.assets.fonts[1],
		.draw_container_bounds = true,
		.play_sound = true,
		.draw_selector = true,
		.take_key_input = true,
		.padding_row = 10,
		.padding_collumn = 5,
		.padding_border  = 5,
		.padding_element = 3,
		.color_background = YELLOW,
		.color_font = RED,
		.color_font_highlight = BLACK,
		.color_borders = BLACK,
	};

	SetSelectorTexture(&Data.assets.textures[0]);
	SetSelectorTextureTint(WHITE);
	SetClickedSound(&Data.assets.sounds[2]);

	Gamesfuncs[MAIN_MENU] = main_menu_init(&Data);

	Data.scores = LoadScores("./data");

	Gamesfuncs[SNAKE_GAME] = snake_game_init(&Data);
	Gamesfuncs[TETRIS] = tetris_init(&Data);
	Gamesfuncs[PONG] = pong_init(&Data);
	Gamesfuncs[BREAKOUT] = breakout_init(&Data);
	Gamesfuncs[TEST] = test_game_init(&Data);

	#ifdef PLATFORM_WEB
		emscripten_set_main_loop(update_and_draw, 0, 1);
	#else
		while (!WindowShouldClose() && !Data.quit) {
			update_and_draw();
		}
	#endif

	Gamesfuncs[TETRIS].de_init();
	Gamesfuncs[SNAKE_GAME].de_init();
	Gamesfuncs[PONG].de_init();
	Gamesfuncs[BREAKOUT].de_init();
	Gamesfuncs[TEST].de_init();
	Gamesfuncs[MAIN_MENU].de_init();

	SaveScores(SCORES_SAVE_LOCATION , Data.scores);

	unload_assets(&Data);
	CloseWindow();
	CloseAudioDevice();
	return (0);
}

internal void update_and_draw(void)
{
	#ifdef PLATFORM_WEB
		if (Data.quit) {
			emscripten_cancel_main_loop();
		}
	#endif

	PoolActions();
	if (IsWindowMinimized()) {
		TraceLog(LOG_INFO, "Window Minimized \n");
		return ;
	}

	f32 screen_scale = MIN((f32)GetScreenWidth()/Data.window_size.x, (f32)GetScreenHeight() / Data.window_size.y);
	// Update virtual mouse (clamped mouse value behind game screen)
	V2 mouse = GetMousePosition();
	V2 virtualMouse = { 0 };
	virtualMouse.x = (mouse.x - (GetScreenWidth() - (Data.window_size.x *screen_scale))*0.5f)/screen_scale;
	virtualMouse.y = (mouse.y - (GetScreenHeight() - (Data.window_size.y *screen_scale))*0.5f)/screen_scale;
	virtualMouse = V2Clamp(virtualMouse, (V2){ 0, 0 }, (V2){ (f32)Data.window_size.x, (f32)Data.window_size.y});

	//Apply the same transformation as the virtual mouse to the real mouse (i.e. to work with raygui)
	SetMouseOffset(-(GetScreenWidth() - (Data.window_size.x*screen_scale))*0.5f, -(GetScreenHeight() - (Data.window_size.y*screen_scale))*0.5f);
	SetMouseScale(1 / screen_scale, 1 / screen_scale);


	if (Data.current_game < 0 || Data.current_game >= GAME_COUNT) {
		TraceLog(LOG_WARNING, "main.c: Invalid option for current_game, won't change.\n");
	} else if (Data.current_game != Game) {
		TraceLog(LOG_INFO, "Changing game\n");
		Game = Data.current_game;
		Gamesfuncs[Game].start();
	}

	Gamesfuncs[Game].update();

	BeginTextureMode(ScreenTexture);
	{
		ClearBackground(RAYWHITE);
		Gamesfuncs[Game].draw();
	}
	EndTextureMode();

	BeginDrawing();
	{
		ClearBackground(BLACK);
		// Draw render texture to screen, properly scaled
		DrawTexturePro(ScreenTexture.texture,
		 (Rect){0.0f, 0.0f, (f32) ScreenTexture.texture.width, (f32) -ScreenTexture.texture.height},
		 (Rect){(GetScreenWidth() - ((f32) Data.window_size.x*screen_scale)) * 0.5f, (GetScreenHeight() - ((f32) Data.window_size.y*screen_scale)) * 0.5f,
		 (f32)Data.window_size.x * screen_scale, (f32)Data.window_size.y * screen_scale },
		 (V2){ 0, 0 },
		 0.0f,
		 WHITE);
		FontConfig font = Data.assets.fonts[1];
		//DrawTextEx(font.font, TextFormat("%d", GetFPS()), (V2){30, 30}, font.size, font.spacing, font.tint);
	}
	EndDrawing();
}

void update_volume(GameData *data)
{
	for (i32 i = 0; i < MAX_ASSET; i++) {
		SetMusicVolume(data->assets.music[i], data->music_vol);
		SetSoundVolume(data->assets.sounds[i], data->effects_vol);
	}
}

internal_inline void load_assets(GameData *data) {
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

internal_inline void unload_assets(GameData *data) {
	for (i32 i = 0; i < MAX_ASSET; i++) {
		UnloadSound(data->assets.sounds[i]);
		UnloadMusicStream(data->assets.music[i]);
		UnloadFont(data->assets.fonts[i].font);
	}
}

internal_inline void register_key_actions()
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

// Only called from js on the web version
void pause_game() 
{
	//TraceLog(LOG_DEBUG, "Pause game called. \n");
	PauseRequested = true;
}

b32 IsPauseRequested()
{
	if (PauseRequested) {
		PauseRequested = false; // Consume request
		return (true);
	}
	return (PauseRequested);
}
