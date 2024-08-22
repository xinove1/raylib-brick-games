NAME= brick_games

RAYLIB= ./raylib-5.0/src
CC= gcc
RM= rm -f

SRC= $(wildcard src/*.c)
OBJ= $(notdir $(SRC:.c=.o))
DEPENDACIES= $(SRC) $(wildcard src/*.h) $(wildcard src/modules/*.h)


# ----------------------------------------------------------------------------------------------
# linux build 

CFLAGS= -I$(RAYLIB) -I$(RAYLIB)/external -std=c99 -Isrc/modules/
DEBUG_FLAGS= -ggdb -g3 -Wall -Wextra -Wno-unused-parameter -Wno-unused-function \
             -fsanitize-trap -fsanitize=address -fsanitize=undefined -fsanitize=unreachable \
             -DBUILD_DEBUG \
             #-Wconversion  -Wno-sign-conversion  -Wdouble-promotion\

RFLAGS= -lGL -lm -lpthread -ldl -lrt -lX11

$(NAME): $(DEPENDACIES)
	@make -C $(RAYLIB) RAYLIB_LIBTYPE=SHARED
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $(SRC)
	$(CC) $(OBJ) $(CFLAGS) $(DEBUG_FLAGS) -Wl,-rpath=$(RAYLIB)/ -lraylib $(RFLAGS) -o $(NAME)

static: $(DEPENDACIES)
	@make -C $(RAYLIB)
	$(CC) $(CFLAGS) -c $(SRC)
	$(CC) $(OBJ) $(RAYLIB)/libraylib.a $(CFLAGS) $(RFLAGS) -o $(NAME)

run: $(NAME)
	./$(NAME)

all: $(NAME)

# ----------------------------------------------------------------------------------------------
# windows build 

CC_WINDOWS= x86_64-w64-mingw32-gcc
AR_WINDOWS= x86_64-w64-mingw32-ar
RFLAGS_WINDOWS= -lopengl32 -lgdi32 -lwinmm
CFLAGS_WINDOWS= -I$(RAYLIB) -I$(RAYLIB)/external -std=c99 -Isrc/modules/

windows: $(DEPENDACIES)
	@make -C $(RAYLIB) OS=Windows_NT CC=$(CC_WINDOWS) AR=$(AR_WINDOWS)
	#$(CC_WINDOWS) $(CFLAGS_WINDOWS) -c $(SRC)
	$(CC_WINDOWS) $(SRC) $(RAYLIB)/libraylib.a $(CFLAGS_WINDOWS) $(RFLAGS_WINDOWS) -o $(NAME).exe

# ----------------------------------------------------------------------------------------------
# WEB build 

WEB_CFLAGS= -I$(RAYLIB) -I$(RAYLIB)/external -Isrc/modules/ -DPLATFORM_WEB 
WEB_DATA_DIR= --preload-file assets
WEB_HTML_TEMPLATE= --shell-file ./template.html
WEB_EXPORTED_FUNCTIONS= -sEXPORTED_FUNCTIONS=_pause_game,_main
WEB_OUTPUT_EXT= .html
WEBFLAGS = $(WEB_EXPORTED_FUNCTIONS) $(WEB_HTML_TEMPLATE) $(WEB_DATA_DIR) --js-library save_load.js -DPLATFORM_WEB  -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=ccall,cwrap -s STACK_SIZE=1mb -Os -s USE_GLFW=3 -sGL_ENABLE_GET_PROC_ADDRESS

web: $(DEPENDACIES)
	make -C $(RAYLIB) PLATFORM=PLATFORM_WEB -B EMSDK_PATH=/home/xinove/stuff/emsdk  PYTHON_PATH=/usr/bin/python NODE_PATH=/home/xinove/stuff/emsdk/node/16.20.0_64bit/bin
	emcc $(WEB_CFLAGS) $(WEBFLAGS) -c $(SRC)
	emcc $(OBJ) $(RAYLIB)/libraylib.a $(WEB_CFLAGS) $(WEBFLAGS) $(RFLAGS) -o $(NAME)$(WEB_OUTPUT_EXT)

web_re:
	emcc $(WEB_CFLAGS) $(WEBFLAGS) -c $(SRC)
	emcc $(OBJ) $(RAYLIB)/libraylib.a $(WEB_CFLAGS) $(WEBFLAGS) $(RFLAGS) -o $(NAME)$(WEB_OUTPUT_EXT)

web_run: $(web)
	emrun ./$(NAME).html

itch:
	cp $(NAME).html index.html
	zip brick_games.zip index.html $(NAME).js $(NAME).data $(NAME).wasm
	rm index.html

# ----------------------------------------------------------------------------------------------
# Utilities

bear: 
	bear -- make

clean:
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
