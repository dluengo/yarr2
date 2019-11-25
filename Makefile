SRC_DIR := `pwd`/src

all:
	make -C $(SRC_DIR)

clean:
	make -C $(SRC_DIR) clean

