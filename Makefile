CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Iinclude -std=c11 -fsanitize=address 
TEST_CFLAGS = $(CFLAGS) -Itests/include
SRC = src/main.c src/my_free.c src/my_malloc.c src/create_arena.c src/coallease.c src/helpers.c
OBJ = $(SRC:src/%.c=build/%.o)

TEST_PATH = tests/src

TEST_SRC = $(TEST_PATH)/main_tests.c \
           $(TEST_PATH)/test_basics.c \
           $(TEST_PATH)/test_coalesce_prev_block.c \
           $(TEST_PATH)/test_coalesce_next_block.c \
           $(TEST_PATH)/test_free_single_allocation.c \
           $(TEST_PATH)/test_single_allocation.c

TEST_OBJ = $(TEST_SRC:tests/src/%.c=build/tests/%.o)
LIB_OBJ = $(filter-out build/main.o, $(OBJ))

malloc: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) $< -c -o $@

build/tests/%.o: tests/src/%.c
	@mkdir -p build/tests
	$(CC) $(TEST_CFLAGS) $< -c -o $@

test: $(TEST_OBJ) $(LIB_OBJ)
	$(CC) $(TEST_CFLAGS) $^ -o build/test_runner
	./build/test_runner


.PHONY: clean test

clean:
	rm -rf build malloc
