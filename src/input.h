#ifndef XI_INPUT_ACTIONS_H
# define XI_INPUT_ACTIONS_H

# include "raylib.h"
# include "stdio.h"

// Comment out or redefine if needed
typedef enum {LEFT, RIGHT, UP, DOWN, ACTION_1, ACTION_2, ACTION_3, OPEN_MENU, INPUT_ACTION_COUNT} InputActions_e;

void	RegisterActionName(int action_id, char *action_name);
void	RegisterInputKeyAction(int action_id, int action_keycode);
bool	IsActionPressed(int action_id);
bool	IsActionReleased(int action_id);
bool	IsActionDown(int action_id);
void	PrintActions();

# ifndef MAX_ACTION_KEYCODES 
#  define MAX_ACTION_KEYCODES 10
# endif
# ifndef MAX_ACTION
#  define MAX_ACTIONS 10
# endif

#endif

#ifdef XI_INPUT_ACTIONS_IMPLEMENTATION
// NOLINTBEGIN(misc-definitions-in-headers)
typedef struct {
	char	*name;
	int	keycodes[MAX_ACTION_KEYCODES];
} Action;

Action	actions[MAX_ACTIONS] = {0};

void	PrintActions() {
	printf("Actions list\n");
	printf("~~~~~~~~~~~~\n");
	for (int i = 0; i < MAX_ACTIONS; i++) {
		if (actions[i].name == NULL) {
			printf("Unamned action: \n");
		} else {
			printf("%s: \n", actions[i].name);
		}
		for (int k = 0; k < MAX_ACTION_KEYCODES; k++) {
			if (actions[i].keycodes[k] == -1){
				break ;
			}
			printf("  %d\n", actions[i].keycodes[k]);
		}
	}
}

Action	*get_action(int id) 
{
	// TODO  Better error message to account to error on registering and getting input
	if (id < 0) {
		TraceLog(LOG_WARNING, "Tryng to register a Key action with negative value, aborting.\n");
		return (NULL);
	} else if (id >= MAX_ACTIONS) {
		TraceLog(LOG_WARNING, "Tryng to register a Key action with value greater/equal than MAX_ACTIONS, aborting.\n");
		return (NULL);
	}
	return (&actions[id]);
}

void	RegisterActionName(int action_id, char *action_name)
{
	if (action_name == NULL) {
		TraceLog(LOG_WARNING, "Tryng to register a action with null name, aborting.\n");
		return ;
	}
	
	Action	*action = get_action(action_id);
	if (action == NULL) {
		return ;
	}

	if (action->name == NULL) {
		TraceLog(LOG_DEBUG, "Action \"%s\" does not exists, registering new action.\n", action_name);
		action->name = action_name;
	} else {
		TraceLog(LOG_DEBUG, "Overwriting action name \"%s\" with \"%s\".\n", action->name, action_name);
		action->name = action_name;
	}

	for (int k = 1; k < MAX_ACTION_KEYCODES; k++) {
		action->keycodes[k] = -1;
	}

}

// Register a keycode to a action, only works on action already named (so the keycodes can be set to -1)
void	RegisterInputKeyAction(int action_id, int action_keycode)
{
	Action	*action = get_action(action_id);
	if (action == NULL) {
		return ;
	}

	for (int k = 0; k < MAX_ACTION_KEYCODES; k++) {
		if (action->keycodes[k] == -1) {
			action->keycodes[k] = action_keycode;
			return ;
		}
	}
	
	char	*action_name = action->name;
	if (action_name == NULL) {
		action_name = (char *)"Unamned action";
	}

	TraceLog(LOG_WARNING, "Action \"%s\" keycodes are full. See MAX_ACTIONS_KEYCODES macro.\n", action_name);
	return ;
}

bool	IsActionPressed(int action_id)
{
	Action	*action = get_action(action_id);
	if (action == NULL) {
		return false;
	}

	for (int k = 0; k < MAX_ACTION_KEYCODES; k++) {
		if (action->keycodes[k] == -1) {
			break ;
		}
		if (IsKeyPressed(action->keycodes[k])) {
			return true;
		}
	}

	return false;
}

bool	IsActionReleased(int action_id)
{
	Action	*action = get_action(action_id);
	if (action == NULL) {
		return false;
	}

	for (int k = 0; k < MAX_ACTION_KEYCODES; k++) {
		if (action->keycodes[k] == -1) {
			break ;
		}
		if (IsKeyReleased(action->keycodes[k])) {
			return true;
		}
	}

	return false;
}

bool	IsActionDown(int action_id)
{
	Action	*action = get_action(action_id);
	if (action == NULL) {
		return false;
	}

	for (int k = 0; k < MAX_ACTION_KEYCODES; k++) {
		if (action->keycodes[k] == -1) {
			break ;
		}
		if (IsKeyDown(action->keycodes[k])) {
			return true;
		}
	}

	return false;
}

// NOLINTEND(misc-definitions-in-headers)
#endif 
