SRC_DIR := `pwd`/src
EXAMPLE_DIR := `pwd`/example
TEST_DIR := `pwd`/test

# TODO: Consider building examples here.
all: yarr2 test

clean: yarr2_clean test_clean

yarr2:
	make -C $(SRC_DIR)

yarr2_clean:
	make -C $(SRC_DIR) clean

test:
	make -C $(TEST_DIR)

test_clean:
	make -C $(TEST_DIR) clean

# TODO: Not working, why?
example:
	make -C $(EXAMPLE_DIR)

example_clean:
	make -C $(EXAMPLE_DIR) clean

