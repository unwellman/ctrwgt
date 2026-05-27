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

ctrwgt.app : $(BIN)/$(EXEC) Info.plist res/ctrwgt.icns
	mkdir -p ctrwgt.app ctrwgt.app/Contents ctrwgt.app/Contents/MacOS \
		ctrwgt.app/Contents/Resources
	mv $(BIN)/$(EXEC) ctrwgt.app/Contents/MacOS
	cp Info.plist ctrwgt.app/Contents
	cp -R res/* ctrwgt.app/Contents/Resources

# Script taken from StackOverflow.
# Source - https://stackoverflow.com/a/20703594
# Posted by Aidan, modified by community. See post 'Timeline' for change history
# Retrieved 2026-05-26, License - CC BY-SA 4.0
res/ctrwgt.icns : res/ctrwgt.png
	mkdir -p ctrwgt.iconset
	sips -z 16 16     res/ctrwgt.png --out ctrwgt.iconset/icon_16x16.png
	sips -z 32 32     res/ctrwgt.png --out ctrwgt.iconset/icon_16x16@2x.png
	sips -z 32 32     res/ctrwgt.png --out ctrwgt.iconset/icon_32x32.png
	sips -z 64 64     res/ctrwgt.png --out ctrwgt.iconset/icon_32x32@2x.png
	sips -z 128 128   res/ctrwgt.png --out ctrwgt.iconset/icon_128x128.png
	sips -z 256 256   res/ctrwgt.png --out ctrwgt.iconset/icon_128x128@2x.png
	sips -z 256 256   res/ctrwgt.png --out ctrwgt.iconset/icon_256x256.png
	sips -z 512 512   res/ctrwgt.png --out ctrwgt.iconset/icon_256x256@2x.png
	sips -z 512 512   res/ctrwgt.png --out ctrwgt.iconset/icon_512x512.png
	cp res/ctrwgt.png ctrwgt.iconset/icon_512x512@2x.png
	iconutil -c icns ctrwgt.iconset
	rm -R ctrwgt.iconset
	mv ctrwgt.icns res

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
	rm -rf ctrwgt.app

