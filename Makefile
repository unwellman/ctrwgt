SHELL = /bin/sh

LIB = lib
BIN = bin
EXEC = ctrwgt
TEST = tests

SRC = src/actor.c src/world.c src/state.c src/log.c src/render.c src/geometry.c
# Every source file %.c must have a test test_%.c
TEST_SRC = $(SRC) $(patsubst src/%,src/test_%,$(SRC))

# main and test both contain an entry point,
# so they should not be linked together
OBJ = src/main.o lib/tomlc17.o $(patsubst %.c,%.o,$(SRC))
TEST_OBJ = src/test.o $(patsubst %.c,%.o,$(TEST_SRC))

# Need a better way to do this---maybe install SDL to ~
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Werror
CFLAGS += -Wno-empty-translation-unit -Wno-unused-parameter -Wno-unused-function
CFLAGS += -I$(HOME)/SDL/include -Ilib

LDFLAGS = -lm
LDFLAGS += -F/Library/Frameworks -rpath /Library/Frameworks
LDFLAGS += -framework SDL3

.PHONY : run tests clean
.SILENT : clean

# Default target
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

