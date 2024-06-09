#ifndef XI_INPUT_ACTIONS_H
# define XI_INPUT_ACTIONS_H

# include "raylib.h"
# include "stdio.h"

void	RegisterInputKeyAction(char *action_name, int action_keycode);
bool	IsActionPressed(char *action_name);
bool	IsActionReleased(char *action_name);
bool	IsActionDown(char *action_name);
void	PrintActions();

# define MAX_ACTION_KEYCODES 10
# define MAX_ACTIONS 10

#endif

#ifdef XI_INPUT_ACTIONS_IMPLEMENTATION
// NOLINTBEGIN(misc-definitions-in-headers)
typedef struct {
	char	*name;
	int	keycodes[MAX_ACTION_KEYCODES];
} Action;

Action	actions[MAX_ACTIONS] = {0};

Action	*FindInputAction(char *action_name) 
{
	for (int i = 0; i < MAX_ACTIONS; i++) {
		if (TextIsEqual(actions[i].name, action_name)) {
			return &actions[i];
		} else if (actions[i].name  == NULL) {
			break ;
		}
	}
	return NULL;
}

void	PrintActions() {
	printf("Actions list\n");
	printf("~~~~~~~~~~~~\n");
	for (int i = 0; i < MAX_ACTIONS; i++) {
		if (actions[i].name == NULL) {
			break;
		}
		printf("%s: \n", actions[i].name);
		for (int k = 0; k < MAX_ACTION_KEYCODES; k++) {
			if (actions[i].keycodes[k] == -1){
				break ;
			}
			printf("  %d\n", actions[i].keycodes[k]);
		}
	}
}

void	RegisterInputKeyAction(char *action_name, int action_keycode)
{
	if (action_name == NULL) {
		TraceLog(LOG_WARNING, "Tryng to register a action with null name, aborting.\n");
		return ;
	}
	int	i;
	for (i = 0; i < MAX_ACTIONS; i++) {
		if (TextIsEqual(actions[i].name, action_name)) {
			TraceLog(LOG_DEBUG, "Action \"%s\" already exists, registering the new keycode.\n", action_name);
			
			for (int k = 0; k < MAX_ACTION_KEYCODES; k++) {
				if (actions[i].keycodes[k] == -1) {
					actions[i].keycodes[k] = action_keycode;
					return ;
				}
			}

			TraceLog(LOG_WARNING, "Action \"%s\" keycodes are full. See MAX_ACTIONS_KEYCODES macro.\n", action_name);
			return ;
		} else if (actions[i].name == NULL) {
			break ;
		}
	}

	TraceLog(LOG_DEBUG, "Action \"%s\" does not exists, registering new action.\n", action_name);
	actions[i].name = action_name;
	actions[i].keycodes[0] = action_keycode;
	for (int k = 1; k < MAX_ACTION_KEYCODES; k++) {
		actions[i].keycodes[k] = -1;
	}
}

bool	IsActionPressed(char *action_name)
{
	Action	*action = FindInputAction(action_name);
	if (action == NULL) {
		TraceLog(LOG_WARNING, "IsActionPressed: Action \"%s\" does not exists.\n", action_name);
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

bool	IsActionReleased(char *action_name)
{
	Action	*action = FindInputAction(action_name);
	if (action == NULL) {
		TraceLog(LOG_WARNING, "IsActionReleased: Action \"%s\" does not exists.\n", action_name);
		return 0;
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

bool	IsActionDown(char *action_name)
{
	Action	*action = FindInputAction(action_name);
	if (action == NULL) {
		TraceLog(LOG_WARNING, "IsActionDown: Action \"%s\" does not exists.\n", action_name);
		return 0;
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
