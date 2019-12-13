INCLUDE_DIR := `pwd`/../include
BIN_DIR := bin
SOURCE := yarrlib.c
TARGET := libyarr.so

all: $(TARGET)

clean:
	rm -rf $(BIN_DIR)

$(TARGET): $(BIN_DIR) $(SOURCE)
	gcc -shared -fPIC -o $(BIN_DIR)/$@ -I$(INCLUDE_DIR) $(SOURCE)

$(BIN_DIR):
	mkdir $@

