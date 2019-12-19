INCLUDE_DIR := `pwd`/../include
LIB_DIR := `pwd`/../lib/bin
BIN_DIR := bin

# GCC autocompletes to libyarr.so
YARRLIB := yarr

TESTS := hidepid_test

all: $(BIN_DIR) $(TESTS)

clean:
	rm -fr $(BIN_DIR)

# TODO: TESTS should contain names of targets we want to build. Make should
# auto-generate the different targets with this form:
hidepid_test: hidepid_test.c
	gcc -o $(BIN_DIR)/$@ -I$(INCLUDE_DIR) -L$(LIB_DIR) $< -l$(YARRLIB)

$(BIN_DIR):
	mkdir $(BIN_DIR)

