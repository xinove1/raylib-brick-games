NAME= game

RAYLIB= ./raylib-5.0/src

CC= gcc
CC_WINDOWS= x86_64-w64-mingw32-gcc


CFLAGS= -I$(RAYLIB) -I$(RAYLIB)/src/external #-Wall -Wextra -Werror
DEBUG_FLAGS= -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wdouble-promotion \
             -fsanitize=address -fsanitize=undefined -fsanitize-trap \
             -DRAYGUI_DEBUG_RECS_BOUNDS -DRAYGUI_DEBUG_TEXT_BOUNDS \
             #-Wconversion  -Wno-sign-conversion \

RFLAGS= -lGL -lm -lpthread -ldl -lrt -lX11
RFLAGS_WINDOWS= -lopengl32 -lgdi32 -lwinmm

WEB_DATA_DIR= --preload-file assets
WEB_HTML_TEMPLATE= #--shell-file ./shell_minimal.html
WEB_EXPORTED_FUNCTIONS= -sEXPORTED_FUNCTIONS=_pause_game,_main
WEBFLAGS = $(WEB_EXPORTED_FUNCTIONS) $(WEB_HTML_TEMPLATE) $(WEB_DATA_DIR) -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=ccall,cwrap -s STACK_SIZE=1mb -Os -s ASYNCIFY -s USE_GLFW=3 -DPLATFORM_WEB -sGL_ENABLE_GET_PROC_ADDRESS

RM= rm -f

SRC= $(wildcard src/*.c)

OBJ=$(notdir $(SRC:.c=.o))

DEPENDACIES= $(SRC) $(wildcard src/*.h)

$(NAME): $(DEPENDACIES)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $(SRC)
	$(CC) $(OBJ) $(CFLAGS) $(DEBUG_FLAGS) -lraylib $(RFLAGS) -o $(NAME)

static: $(DEPENDACIES)
	@make -C $(RAYLIB)
	$(CC) $(CFLAGS) -c $(SRC)
	$(CC) $(OBJ) $(RAYLIB)/libraylib.a $(CFLAGS) $(RFLAGS) -o $(NAME)

cosmos: $(DEPENDACIES)
	@make -C $(RAYLIB) CC=cosmocc
	cosmocc $(CFLAGS) -c $(SRC)
	cosmocc $(OBJ) $(RAYLIB)/libraylib.a $(CFLAGS) $(RFLAGS) -o $(NAME)

windows: $(DEPENDACIES)
	@make -C $(RAYLIB) OS=Windows_NT CC=x86_64-w64-mingw32-gcc AR=x86_64-w64-mingw32-ar
	$(CC_WINDOWS) $(CFLAGS) -c $(SRC)
	$(CC_WINDOWS) $(OBJ) $(RAYLIB)/libraylib.a $(RFLAGS_WINDOWS) -o $(NAME).exe

web: $(DEPENDACIES)
	make -C $(RAYLIB) PLATFORM=PLATFORM_WEB -B EMSDK_PATH=/home/xinove/stuff/emsdk  PYTHON_PATH=/usr/bin/python NODE_PATH=/home/xinove/stuff/emsdk/node/16.20.0_64bit/bin
	emcc $(CFLAGS) -c $(SRC)
	emcc $(OBJ) $(RAYLIB)/libraylib.a $(CFLAGS) $(RFLAGS) $(WEBFLAGS) -o $(NAME).js
#emcc -o game.html game.c -Os -Wall ./path-to/libraylib.a -I. -Ipath-to-raylib-h -L. -Lpath-to-libraylib-a  --shell-file path-to/shell.html

web_run: $(web)
	emrun ./$(NAME).html

run: $(NAME)
	./$(NAME)

all: $(NAME)

bear: 
	bear -- make

clean:
	@make -C $(RAYLIB) clean
	@$(RM) $(OBJ)

fclean: clean
	@make -C $(RAYLIB) clean
	$(RM) $(NAME)
	$(RM) $(NAME).html
	$(RM) $(NAME).js
	$(RM) $(NAME).wasm
	$(RM) $(NAME).exe
	$(RM) $(NAME).data

re: fclean all

.PHONY: clean fclean e ew
