#ifndef XI_INPUT_ACTIONS_H
# define XI_INPUT_ACTIONS_H

# include "raylib.h"
# include "stdio.h"

void	register_input_action(char *action_name, int action_keycode);
bool	IsActionPressed(char *action_name);
bool	IsActionReleased(char *action_name);
bool	IsActionDown(char *action_name);
void	print_actions();

# define MAX_ACTION_KEYCODES 10
# define MAX_ACTIONS 10
# define ACTIONS_DEBUG_INFO false

#endif

#ifdef XI_INPUT_ACTIONS_IMPLEMENTATION

typedef struct {
	char	*name;
	int	keycodes[MAX_ACTION_KEYCODES];
} Action;

Action	actions[MAX_ACTIONS] = {0};

Action	*find_action(char *action_name) 
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

void	print_actions() {
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

void	register_input_action(char *action_name, int action_keycode)
{
	if (action_name == NULL) {
		#ifdef ACTIONS_DEBUG_INFO
			printf("Tryng to register a action with null name, aborting.\n");
		#endif
		return ;
	}
	int	i;
	for (i = 0; i < MAX_ACTIONS; i++) {
		if (TextIsEqual(actions[i].name, action_name)) {
			#ifdef ACTIONS_DEBUG_INFO
				printf("Action \"%s\" already exists, registering the new keycode.\n", action_name);
			#endif
			
			for (int k = 0; k < MAX_ACTION_KEYCODES; k++) {
				if (actions[i].keycodes[k] == -1) {
					actions[i].keycodes[k] = action_keycode;
					return ;
				}
			}
			#ifdef ACTIONS_DEBUG_INFO
				printf("Action \"%s\" keycodes are full. See MAX_ACTIONS_KEYCODES macro.\n", action_name);
			#endif
			return ;
		} else if (actions[i].name == NULL) {
			break ;
		}
	}
	#ifdef ACTIONS_DEBUG_INFO
		printf("Action \"%s\" does not exists, registering new action.\n", action_name);
	#endif
	
	actions[i].name = action_name;
	actions[i].keycodes[0] = action_keycode;
	for (int k = 1; k < MAX_ACTION_KEYCODES; k++) {
		actions[i].keycodes[k] = -1;
	}
}

bool	IsActionPressed(char *action_name)
{
	Action	*action = find_action(action_name);
	if (action == NULL) {
		#ifdef ACTIONS_DEBUG_INFO
			printf("IsActionPressed: Action \"%s\" does not exists.\n", action_name);
		#endif
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
	Action	*action = find_action(action_name);
	if (action == NULL) {
		#ifdef ACTIONS_DEBUG_INFO
			printf("IsActionReleased: Action \"%s\" does not exists.\n", action_name);
		#endif
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
	Action	*action = find_action(action_name);
	if (action == NULL) {
		#ifdef ACTIONS_DEBUG_INFO
			printf("IsActionDown: Action \"%s\" does not exists.\n", action_name);
		#endif
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

#endif 
