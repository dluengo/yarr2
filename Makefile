SRC_DIR := `pwd`/src
EXAMPLES_DIR := `pwd`/examples

# TODO: Consider building examples here.
all: yarr2

clean: yarr2_clean

yarr2:
	make -C $(SRC_DIR)

yarr2_clean:
	make -C $(SRC_DIR) clean

# TODO: Not working, why?
examples:
	make -C $(EXAMPLES_DIR)

examples_clean:
	make -C $(EXAMPLES_DIR) clean

