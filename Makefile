SRC_DIR := `pwd`/src
EXAMPLE_DIR := `pwd`/example
TEST_DIR := `pwd`/test
LIB_DIR := `pwd`/lib

# TODO: Consider building examples here.
all: yarr2 yarrlib test

clean: yarr2_clean yarrlib_clean test_clean

yarr2:
	make -C $(SRC_DIR)

yarr2_clean:
	make -C $(SRC_DIR) clean

yarrlib:
	make -C $(LIB_DIR)

yarrlib_clean:
	make -C $(LIB_DIR) clean

# TODO: Not working, why?
test:
	make -C $(TEST_DIR)

test_clean:
	make -C $(TEST_DIR) clean

# TODO: Not working, why?
example:
	make -C $(EXAMPLE_DIR)

example_clean:
	make -C $(EXAMPLE_DIR) clean

