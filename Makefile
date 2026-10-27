CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Iinclude -std=c11 -fsanitize=address 

SRC = src/main.c src/my_free.c src/my_malloc.c src/create_arena.c src/coallease.c src/helpers.c	
OBJ = $(SRC:src/%.c=build/%.o)

TEST_SRC = tests/main_tests.c
TEST_OBJ = $(TEST_SRC:tests/%.c=build/tests/%.o)
LIB_OBJ = $(filter-out build/main.o, $(OBJ))

malloc: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) $< -c -o $@

build/tests/%.o: tests/%.c
	@mkdir -p build/tests
	$(CC) $(CFLAGS) $< -c -o $@

test: $(TEST_OBJ) $(LIB_OBJ)
	$(CC) $(CFLAGS) $^ -o build/test_runner
	./build/test_runner


.PHONY: clean test

clean:
	rm -rf build malloc
