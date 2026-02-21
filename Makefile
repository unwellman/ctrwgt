SHELL = /bin/sh

CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -Wstrict-aliasing -Werror
CFLAGS += -Wno-empty-translation-unit

LDFLAGS = -lm

# Assuming required frameworks for MacOS
ifeq ($(UNAME_S), Darwin)
	LDFLAGS += -framework IOKit -framework -CoreVideo -framework Cocoa
endif

SRC = src/actor.c
# Every source file %.c must have a test test_%.c
TEST_SRC = $(SRC) $(patsubst src/%,src/test_%,$(SRC))

# main and test both contain an entry point,
#  so they should not be linked together
OBJ = src/main.o $(patsubst %.c,%.o,$(SRC))
TEST_OBJ = src/test.o $(patsubst %.c,%.o,$(TEST_SRC))

BIN = bin
EXEC = ctrwgt

.PHONY : exec tests libs dirs clean
.SILENT : clean


exec : libs dirs $(OBJ) src/main.o tests
	$(CC) $(OBJ) $(LDFLAGS) -o ./$(BIN)/$(EXEC)

tests : libs dirs $(TEST_OBJ)
	$(CC) $(TEST_OBJ) $(LDFLAGS) -o ./$(BIN)/tests
	./$(BIN)/tests

run : exec
	./$(BIN)/$(EXEC)

libs :

dirs :
	mkdir -p ./$(BIN)

%.o : %.c
	$(CC) $(CFLAGS) $< -c -o $@

clean :
	rm -rf $(BIN) $(OBJ)

