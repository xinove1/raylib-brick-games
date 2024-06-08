#ifndef XI_UI_H
# define XI_UI_H
# include "raylib.h"

// TODO  Create header for types
typedef Vector2 V2;
typedef Rectangle Rect;

typedef struct {
	Font	font;
	int	size;
	int	spacing;
	Color	tint;
	Color	tint_hover;
} FontConfig;

typedef struct UiPanel
{
	V2	pos; // NOTE  Remember initialize this
	float	width; // Width of the longest element added
	//float	height;
	float	at_x; // Position after last lement added
	float	at_y;
	int	id_count; 
	int	id_current; // NOTE  Remember to 0 initialize this
	bool	centralized; // if true centralizes elements on given pos and at panel_end changes pos.x to give a proper bounding box of the panel
} UiPanel;


void	set_clicked_sound(Sound *sound);
void	set_selector_texture(Texture2D *texture);
void	set_selector_texture_tint(Color tint);

void	panel_begin(UiPanel *panel);
void	panel_end(UiPanel *panel);
void	panel_take_key_input(UiPanel *panel);
void	panel_title(UiPanel *panel, char *text, FontConfig font);
bool	panel_text_button(UiPanel *panel, char *text, FontConfig config);

#endif
