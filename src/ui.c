#include "ui.h"
#include "input.h"

void	panel_begin(UiPanel *panel) 
{

	panel->width = 0;
	panel->at_x = panel->pos.x;
	panel->at_y = panel->pos.y;
	panel->id_count = 0;
}

void	panel_end(UiPanel *panel)
{
	panel_take_key_input(panel);
	if (panel->centralized) {
		panel->pos.x -= panel->width * 0.5f;
	}
}

void	panel_take_key_input(UiPanel *panel)
{
	int	*current = &panel->id_current;
	int	*count = &panel->id_count;
	if (IsActionPressed("up")) {
		*current -= 1;
		if (*current < 0) {
			*current = *count - 1;
		}
	}
	if (IsActionPressed("down")) {
		*current += 1;
		if (*current >= *count) {
			*current = 0;
		}
	}
}

void	panel_title(UiPanel *panel, char *text, FontConfig font) 
{
	V2	text_size = MeasureTextEx(font.font, text, font.size, font.spacing);
	V2	offset = {panel->at_x, panel->at_y};
	if (panel->centralized) {
		offset.x -= text_size.x * 0.5f;
	}

	DrawRectangle(offset.x, offset.y, text_size.x, text_size.y, font.tint_hover);
	DrawTextEx(font.font, text, offset, font.size, font.spacing, font.tint);

	panel->at_y += text_size.y;
	panel->at_y += 15; // Add padding // NOTE  Hard coded value
	if (panel->width < text_size.x){
		panel->width  = text_size.x;
	}
}

bool	panel_text_button(UiPanel *panel, char *text, FontConfig config)
{
	bool	r = false;
	bool	mouse_inside = false;
	V2	text_size = MeasureTextEx(config.font, text, config.size, config.spacing);
	V2	offset = {panel->at_x, panel->at_y};
	Rect	rect = {offset.x, offset.y, text_size.x, text_size.y};
	Color	color = config.tint;
	int	id = panel->id_count; 
	panel->id_count++;

	if (panel->centralized) {
		offset.x -= text_size.x * 0.5f;
	}

	// TODO Add flag if last change in current ui selected was with keys, and then not check mouse pos until mouse movement
	if (CheckCollisionPointRec(GetMousePosition(), rect)) {
		panel->id_current = id;
		mouse_inside = true;
	}
	if (panel->id_current == id) {
		color = config.tint_hover;
		DrawRectangle(offset.x - 10, offset.y + (text_size.y * 0.5f) - 2.5f, 5, 5, RED); // Temp
		if ((mouse_inside && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsActionPressed("action_1")) {
			r = true;
		}
	}

	DrawTextEx(config.font, text, offset, config.size, config.spacing, color);

	panel->at_y += text_size.y;
	panel->at_y += 10; // Padding // NOTE  Hard coded
	if (panel->width < text_size.x){
		panel->width  = text_size.x;
	}
	return (r);
}
