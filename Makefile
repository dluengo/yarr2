SRC_DIR := `pwd`/src
EXAMPLE_DIR := `pwd`/example
TEST_DIR := `pwd`/test
LIB_DIR := `pwd`/lib

all: yarr2 yarrlib test example

clean: yarr2_clean yarrlib_clean test_clean example_clean

.PHONY: test \
    example

yarr2:
	make -C $(SRC_DIR)

yarr2_clean:
	make -C $(SRC_DIR) clean

yarrlib:
	make -C $(LIB_DIR)

yarrlib_clean:
	make -C $(LIB_DIR) clean

test:
	make -C $(TEST_DIR)

test_clean:
	make -C $(TEST_DIR) clean

example:
	make -C $(EXAMPLE_DIR)

example_clean:
	make -C $(EXAMPLE_DIR) clean

