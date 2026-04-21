SHELL = /bin/sh

CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Werror
CFLAGS += -Wno-empty-translation-unit -Wno-unused-parameter -Wno-unused-function

LDFLAGS = -lm

# Assuming required frameworks for MacOS
#ifeq ($(UNAME_S), Darwin)
	#LDFLAGS += -framework IOKit -framework -CoreVideo -framework Cocoa
#endif

SRC = src/actor.c src/world.c src/state.c src/log.c
# Every source file %.c must have a test test_%.c
TEST_SRC = $(SRC) $(patsubst src/%,src/test_%,$(SRC))
#
# main and test both contain an entry point,
#  so they should not be linked together
OBJ = src/main.o lib/tomlc17.o $(patsubst %.c,%.o,$(SRC))
TEST_OBJ = src/test.o $(patsubst %.c,%.o,$(TEST_SRC))

LIB = lib
BIN = bin
EXEC = ctrwgt
TEST = tests

# Need a better way to do this---maybe install SDL to ~
CFLAGS += -I/usr/local/Cellar/sdl3/3.4.2/include -Ilib
LDFLAGS += -F$(HOME)/SDL
LDFLAGS += -framework SDL3
#LDFLAGS += -framework CoreAudio -framework CoreVideo
#LDFLAGS += -framework Cocoa -framework Metal -framework IOKit
#LDFLAGS += -framework AudioToolbox -framework ForceFeedback
#LDFLAGS += -framework CoreHaptics

.PHONY : run tests clean
.SILENT : clean

# Default
$(BIN)/$(EXEC) : $(BIN) $(OBJ) tests src/main.o
	$(CC) $(OBJ) $(LDFLAGS) -o ./$(BIN)/$(EXEC)

$(BIN)/$(TEST) : $(BIN) $(TEST_OBJ) #$(LIB)/DUMMY_SDL
	$(CC) $(TEST_OBJ) $(LDFLAGS) -o ./$(BIN)/tests

tests : $(BIN)/$(TEST)
	./$(BIN)/$(TEST)

run : $(BIN)/$(EXEC)
	./$(BIN)/$(EXEC)

$(LIB)/DUMMY_SDL : $(LIB)
	cd $(LIB); \
	git clone https://github.com/libsdl-org/SDL; \
	cd SDL; \
	cmake -S . -B build -DSDL_STATIC=ON -DSDL_TESTS=OFF; \
	cmake --build build;
	touch $(LIB)/DUMMY_SDL

$(BIN) :
	mkdir -p ./$(BIN)

$(LIB) :
	mkdir -p ./$(LIB)

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean :
	rm -rf $(BIN) $(OBJ) $(TEST_OBJ)

