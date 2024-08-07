#ifndef XI_UI_H
# define XI_UI_H

# include "raylib.h"
# include "raymath_short.h"
# include "stdio.h"
# include "input.h"
# include "raymath.h"
# include "types.h"
# include <string.h>
# include <stdlib.h>

// TODO  Remove font config and add size and spacing to UiConfig?
typedef struct {
	Font  font;
	i32   size;
	i32   spacing;
	Color tint;
	Color tint_hover;
} FontConfig;

typedef enum {UiAlignCentralized, UiAlignLeft, UiAlignRight, UiAlignmentCount} UiAlignmentTypes_e;

typedef struct
{
	UiAlignmentTypes_e alignment;
	FontConfig font;
	b32	   draw_container_bounds;
	b32	   play_sound;
	b32	   draw_selector;
	b32	   take_key_input;
	//float	   padding_;
	f32	   padding_row;
	f32	   padding_collumn;
	f32	   padding_element; // Padding between element and its box, ex: the text in a button and the rec representing it
	f32	   padding_border; // Padding between element and border of container
	//Color	   color_;
	Color	   color_font;
	Color	   color_font_highlight;
	Color	   color_background;
	Color	   color_borders;
} UiConfig;

typedef struct
{
	V2	 pos;
	f32	 width; // Width of the longest element added
	f32	 height; // Height of the elements  NOTE  not being used, only in sliderbar
	// Position after last lement added
	f32	 at_x;
	f32	 at_y;
	i32	 id_count; 
	i32	 id_current;
	i32	 column_count; // Used for adding elements on same "line", aka when non 0, don't update at_y but at_x  TODO better name
	UiConfig config;
	b32	 hide;
	// For When Title Bar is used
	b32	 mouse_dragging;
	V2	 mouse_last_pos;
} UiContainer;

void SetClickedSound(Sound *sound);
void SetSelectorTexture(Texture2D *texture);
void SetSelectorTextureTint(Color tint);

// TODO  Change Ex functions to not accept font, as it can be easely passed in config 
// TODO  Change draw_bounds flag in funcs to be an option in config?

UiContainer CreateContainer(V2 pos, f32 width, UiConfig config);
UiConfig    GetDefaultUiConfig(); 
void        SetDefaultUiConfig(UiConfig config);
void        UiBegin(UiContainer *container);
void        UiEnd(UiContainer *container);
void        UiTitleBarEx(UiContainer *container, UiConfig config, byte *title, FontConfig font, Color bounds_color);
void        UiText(UiContainer *container, byte *text, b32 draw_bounds);
void        UiTextEx(UiContainer *container, UiConfig config, byte *text, b32 draw_bounds, FontConfig font);
b32         UiTextOptionsEx(UiContainer *container, UiConfig config, b32 draw_bounds, byte *text_pre, byte **options, u32 options_size, i32 *selected);
b32         UiTextButton(UiContainer *container, byte *text);
b32         UiTextButtonEx(UiContainer *container, UiConfig config, byte *text, FontConfig font);
b32         UiSlider(UiContainer *container, f32 *value, f32 min, f32 max);
b32         UiSliderEx(UiContainer *container, UiConfig config, V2 size, f32 *value, f32 min, f32 max);

#endif

#ifdef XI_UI_IMPLEMENTATION

// NOLINTBEGIN(misc-definitions-in-headers)

static void _draw_selector(V2 offset, V2 target_bounds);
static void _take_input(UiContainer *container);
static void _play_clicked_sound();
static void _update_at_pos(UiContainer *container, UiConfig config, V2 element_pos, V2 element_size); // TODO better name
static V2 _get_next_pos(UiContainer *container, UiConfig config);
static byte *_strjoin(const byte *s1, const byte *s2);

static Texture2D *SelectorTexture = NULL; 
static Color     SelectorTint = RAYWHITE;
static Sound     *ClickedSound = NULL;
static b32       CheckMouse = false;
static UiConfig  DefaultConfig = {}; // TODO  Fill with defaults

void SetSelectorTexture(Texture2D *texture)
{
	SelectorTexture = texture;
}

void SetSelectorTextureTint(Color tint)
{
	SelectorTint = tint;
}

void SetClickedSound(Sound *sound)
{
	ClickedSound = sound;
}

UiConfig	GetDefaultUiConfig()
{
	return (DefaultConfig);
}

void SetDefaultUiConfig(UiConfig config) 
{
	DefaultConfig = config;
}

UiContainer CreateContainer(V2 pos, f32 width, UiConfig config)
{
	return ((UiContainer) {
		.pos = pos,
		.width = width,
		.height = 0,
		.at_x = 0,
		.at_y = 0,
		.id_count = 0,
		.id_current = 0,
		.column_count = 0,
		.config = config,
		.hide = false,
		.mouse_dragging = false,
		.mouse_last_pos = {0, 0},
	});
}

void UiBegin(UiContainer *container) 
{
	// TODO  Implement AlignLeft
	// TODO  Implement Collumn
	if (container->config.draw_container_bounds && !container->hide) {
		V2	pos = container->pos;
		if (container->config.alignment == UiAlignCentralized) {
			f32 padding_remove = (container->config.padding_border * 2) + (container->config.padding_element * 2); // Brain not working so i don't get why it works but it works
			pos.x -= (container->width - padding_remove) * 0.5f;
		}
		V2	size = {container->width, container->at_y - pos.y};
		DrawRectangleV(pos, size, container->config.color_background);
	}

	// Reseting
	container->width = 0; //  TODO Optional dynamic width
	container->at_x = container->pos.x;
	container->at_y = container->pos.y;
	container->id_count = 0;

	if (!IsMouseMoving() && WasAnyActionDown()) {
		CheckMouse = false;
	} else if (IsMouseMoving()) {
		CheckMouse = true;
	}
}

void UiEnd(UiContainer *container)
{
	if (container->config.take_key_input) _take_input(container);

	if (container->id_current >= container->id_count) {
		container->id_current = container->id_count - 1;
	}
}


void UiTitleBarEx(UiContainer *container, UiConfig config, byte *title, FontConfig font, Color bounds_color)
{
	V2 pos = {container->at_x, container->at_y};
	V2 text_size = MeasureTextEx(font.font, title, font.size, font.spacing);
	if (config.alignment == UiAlignCentralized) {
		pos.x -= text_size.x * 0.5f;
	}
	V2 text_pos  = {pos.x + config.padding_element, pos.y + config.padding_element};

	V2    hide_size = {text_size.x * 0.1f, text_size.y * 0.6f};
	V2    hide_pos = {text_pos.x + text_size.x + config.padding_collumn, text_pos.y + ((text_size.y - hide_size.y) * 0.5f)};
	Color hide_color = config.color_font;
	Rect  hide_rect = {hide_pos.x, hide_pos.y, hide_size.x, hide_size.y};

	V2 size = {.x = text_size.x + (config.padding_element * 2) + ((hide_pos.x + hide_size.x) - (text_pos.x + text_size.x)),
		   .y = text_size.y + (config.padding_element * 2)};
	Rect rec = {pos.x, pos.y, size.x, size.y};

	V2 mouse_pos = GetMousePosition();
	if (CheckCollisionPointRec(mouse_pos, hide_rect)) {
		hide_color = config.color_font_highlight;
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			container->hide = container->hide ? false : true;
		}
	}
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		if (container->mouse_dragging) {
			container->pos = V2Add(container->pos, V2Subtract(mouse_pos, container->mouse_last_pos));
			container->mouse_last_pos = mouse_pos;
		} else if (CheckCollisionPointRec(mouse_pos, rec)) {
			container->mouse_dragging = true;
			container->mouse_last_pos = mouse_pos;
		}
	} else {
		container->mouse_dragging = false;
	}

	DrawRectangleRec(rec, bounds_color);
	DrawRectangleLinesEx(rec, 1, config.color_borders);
	DrawRectangleRec(hide_rect, hide_color);
	DrawTextEx(font.font, title, text_pos, font.size, font.spacing, config.color_font);

	container->at_y += size.y;
	container->at_y += config.padding_row; 
	if (container->width < rec.width){
		container->width  = rec.width;
	}
}

void UiText(UiContainer *container, byte *text, b32 draw_bounds) 
{
	if (container->hide == true) { return; }
	if (!IsFontReady(container->config.font.font)) {
		TraceLog(LOG_WARNING, "UiText: No default font set.");
	}
	UiTextEx(container, container->config, text, draw_bounds, container->config.font);
}

//#define Testfunc(str, ...) _Testfunc((str), (GameData){.quit = false, __VA_ARGS__})

void UiTextEx(UiContainer *container, UiConfig config, byte *text, b32 draw_bounds, FontConfig font) 
{
	if (container->hide == true) { return; }
	V2 text_size = MeasureTextEx(font.font, text, font.size, font.spacing);
	V2 pos = _get_next_pos(container, config);
	if (config.alignment == UiAlignCentralized) {
		pos.x -= text_size.x * 0.5f;
	}
	V2 text_pos = {pos.x + config.padding_element, pos.y + config.padding_element};
	V2 size = {.x = text_size.x + (config.padding_element * 2),
		   .y = text_size.y + (config.padding_element * 2) };

	if (draw_bounds) {
		DrawRectangle(pos.x, pos.y, size.x, size.y, config.color_borders);
	}

	DrawTextEx(font.font, text, text_pos, font.size, font.spacing, config.color_font);

	_update_at_pos(container, config, pos, size);
}

b32 UiTextButton(UiContainer *container, byte *text)
{
	if (container->hide == true) { return(false); }
	if (!IsFontReady(container->config.font.font)) {
		TraceLog(LOG_WARNING, "UiTextButton: No default font set.");
		return (false);
	}
	return (UiTextButtonEx(container, container->config, text, container->config.font));
}

b32 UiTextButtonEx(UiContainer *container, UiConfig config, byte *text, FontConfig font)
{
	if (container->hide == true) { return(false); }
	V2 text_size = MeasureTextEx(font.font, text, font.size, font.spacing);
	V2 pos = _get_next_pos(container, config);
	if (config.alignment == UiAlignCentralized) {
		pos.x -= text_size.x * 0.5f;
	}
	V2 text_pos = {pos.x + config.padding_element, pos.y + config.padding_element};
	V2 size = {.x = text_size.x + (config.padding_element * 2),
		   .y = text_size.y + (config.padding_element * 2) };
	Rect rect = {pos.x, pos.y, size.x, size.y};

	Color color = config.color_font;
	b32 pressed = false;
	b32 mouse_inside = false;
	i32 id = container->id_count; 
	container->id_count++;

	if (CheckMouse && CheckCollisionPointRec(GetMousePosition(), rect)) {
		container->id_current = id;
		mouse_inside = true;
	}
	if (container->id_current == id) {
		DrawRectangleRec(rect, config.color_font);
		color = config.color_font_highlight;
		if (config.draw_selector) _draw_selector(pos, text_size);

		if ((mouse_inside && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) || IsActionPressed(ACTION_1)) {
			pressed = true;
			if (config.play_sound) _play_clicked_sound();
		}
	} else {
		DrawRectangleRec(rect, config.color_font_highlight);
	}

	DrawTextEx(font.font, text, text_pos, font.size, font.spacing, color);

	_update_at_pos(container, config, pos, size);
	return (pressed);
}

b32        UiTextOptionsEx(UiContainer *container, UiConfig config, b32 draw_bounds, byte *text_pre, byte **options, u32 options_size, i32 *selected)
{
	if (container->hide == true) { return (false); }
	if (*selected < 0 || *selected >= options_size) {
		TraceLog(LOG_WARNING, "UiTextOptionsEx: selected arg [%d] is out of bounds of options [size: %lu], aborting.", *selected, options_size);
		return (false);
	}
	byte *text = _strjoin(text_pre, options[*selected]);
	V2 text_size = MeasureTextEx(config.font.font, text, config.font.size, config.font.spacing);

	V2 pos = _get_next_pos(container, config);
	if (config.alignment == UiAlignCentralized) {
		pos.x -= text_size.x * 0.5f;
	}
	V2 text_pos = {pos.x + config.padding_element, pos.y + config.padding_element};

	V2   size = {.x = text_size.x + (config.padding_element * 2), .y = text_size.y + (config.padding_element * 2) };
	Rect rect = {pos.x, pos.y, size.x, size.y};

	Color color = config.color_font;
	b32 pressed = false;
	b32 mouse_inside = false;
	i32 id = container->id_count; 
	container->id_count++;

	if (CheckMouse && CheckCollisionPointRec(GetMousePosition(), rect)) {
		container->id_current = id;
		mouse_inside = true;
	}
	if (container->id_current == id) {
		DrawRectangleRec(rect, config.color_font);
		color = config.color_font_highlight;
		if (config.draw_selector) _draw_selector(pos, text_size);

		if (mouse_inside) {
			f32 wheel = GetMouseWheelMove();
			if (wheel == 1.0f || wheel == -1.0f) {
				*selected += (i32) wheel;
				pressed = true;
			}
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { 
				*selected += 1;
				pressed = true;
			}
		}

		if (IsActionPressed(RIGHT) || IsActionPressed(ACTION_1)) {
			*selected += 1;
			pressed = true;
		}
		if (IsActionPressed(LEFT)) {
			*selected -= 1;
			pressed = true;
		}
	} else {
		DrawRectangleRec(rect, config.color_font_highlight);
	}

	if (*selected < 0) *selected = options_size - 1; // Checking if it's negative first because a -i32 >= u32 will result to true
	if (*selected >= options_size) *selected = 0;

	DrawTextEx(config.font.font, text, text_pos, config.font.size, config.font.spacing, color);

	free(text);
	if (pressed && config.play_sound) _play_clicked_sound();
	_update_at_pos(container, config, pos, size);
	return (pressed);
}

b32 UiSlider(UiContainer *container, f32 *value, f32 min, f32 max)
{
	if (container->hide == true) { return(false); }
	V2 size = {container->width * 0.8f, container->height};
	return (UiSliderEx(container, container->config, size, value, min, max));
}

b32 UiSliderEx(UiContainer *container, UiConfig config, V2 size, f32 *value, f32 min, f32 max)
{
	if (container->hide == true) { return(false); }
	b32  pressed = false;
	b32  mouse_inside = false;
	V2   pos = _get_next_pos(container, config);
	Rect rect = {pos.x, pos.y, size.x, size.y};
	i32  id = container->id_count; 
	f32  step = 0.1f;
	container->id_count++;

	if (config.alignment == UiAlignCentralized) {
		pos.x -= size.x * 0.5f;
		rect.x = pos.x;
	}

	if (CheckMouse && CheckCollisionPointRec(GetMousePosition(), rect)) {
		container->id_current = id;
		mouse_inside = true;
	}
	if (container->id_current == id) {
		if (config.draw_selector) _draw_selector(pos, size);
		
		if (mouse_inside) {
			f32 wheel = GetMouseWheelMove();
			if (wheel == 1.0f || wheel == -1.0f) {
				*value += step * wheel;	
				pressed = true;
			}
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				V2	pos = Vector2Subtract(GetMousePosition(), (V2){rect.x, rect.y});
				*value = Remap(pos.x, 0, rect.width, 0, 1);
				pressed = true;
			}
		}

		if (IsActionPressed(RIGHT)) {
			*value += step;	
			pressed = true;
		}
		if (IsActionPressed(LEFT)) {
			*value -= step;	
			pressed = true;
		}
	}

	if (*value > max) *value = max;
	if (*value < min) *value = min;

	Rect rect2 = {rect.x, rect.y, rect.width * (*value), rect.height};

	DrawRectanglePro(rect, (V2){0,0}, 0, container->config.color_font);
	DrawRectanglePro(rect2, (V2){0,0}, 0, container->config.color_borders);

	_update_at_pos(container, config, pos, size);
	if (pressed && config.play_sound) _play_clicked_sound();
	return (pressed);
}

static void _draw_selector(V2 offset, V2 target_bounds)
{
	if (SelectorTexture == NULL) { 
		TraceLog(LOG_WARNING, "Ui: draw_selector requested but no texture set.");
		return ;
	}
	DrawTexturePro(
		*SelectorTexture,
		(Rect){0, 0, (float)SelectorTexture->width, (float)SelectorTexture->height},
		(Rect){offset.x - (SelectorTexture->width * 0.50f) - 5, offset.y + (target_bounds.y * 0.5f) - (SelectorTexture->height * 0.25f), SelectorTexture->width * 0.5f, SelectorTexture->height * 0.5f},
		(V2){0,0},
		0,
	SelectorTint);
}

static void _play_clicked_sound()
{
	if (ClickedSound == NULL) {
		TraceLog(LOG_WARNING, "Ui: play_sound requested but no sound set.");
		return ;
	}
	PlaySound(*ClickedSound);
}

static void _take_input(UiContainer *container)
{
	i32 *current = &container->id_current;
	i32 *count = &container->id_count;
	if (IsActionPressed(UP)) {
		*current -= 1;
		if (*current < 0) {
			*current = *count - 1;
		}
	}
	if (IsActionPressed(DOWN)) {
		*current += 1;
		if (*current >= *count) {
			*current = 0;
		}
	}
}

static void _update_at_pos(UiContainer *container, UiConfig config, V2 element_pos, V2 element_size)
{
	if (container->column_count > 0) {
		container->at_x += element_size.x;
		if (container->column_count != 1) { // There's Still Elements to be added
			container->at_x += config.padding_collumn;
		} else { // Last Element being added
			f32 width_so_far = (container->at_x - container->pos.x) + config.padding_border;
			if (width_so_far > container->width){
				container->width  = width_so_far;
			}
			container->at_x = container->pos.x;
		}
		container->column_count -= 1;
	} else {
		container->at_y += element_size.y;
		container->at_y += config.padding_row; 

		if (config.alignment == UiAlignCentralized) {
			element_pos.x += element_size.x * 0.5f;
		}
		f32 element_width = (element_pos.x + element_size.x) - container->at_x;
		// element_size.x += element_pos.x - container->at_x; // Account for any padding at beggining of element
		// element_size.x += config.padding_border; // Account for padding at end of elemnent
		if (element_width > container->width){
			container->width = element_width;
		}
	}
}

static V2 _get_next_pos(UiContainer *container, UiConfig config) 
{
	V2 pos = {0, 0};

	pos = (V2) {container->at_x + config.padding_border, container->at_y};

	return (pos);
}

// TODO  Change when a i write proper string lib
static byte *_strjoin(const byte *s1, const byte *s2)
{
	byte *join;
	i32  s1_lenght = strlen(s1);
	i32  s2_lenght = strlen(s2);

	join = (byte *) malloc(s1_lenght + s2_lenght + 1);
	if (join == 0) return (join);
	memcpy(join, s1, s1_lenght);
	memcpy(&join[s1_lenght], s2, s2_lenght);
	join[s2_lenght + s1_lenght] = '\0';
	return (join);
}

// void _Testfunc(char *str, GameData data) 
// {
// 	printf("str: %s, data- quit: %d, music_vol: %f\n", str, data.quit, data.music_vol);
// }

// #define Testfunc(str, ...) _Testfunc((str), (GameData){.quit = false, __VA_ARGS__})
// NOLINTEND(misc-definitions-in-headers)
#endif
