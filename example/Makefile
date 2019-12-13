YARR_INCLUDES := `pwd`/../include
LIB_DIR := `pwd`/../lib/objs
BIN_DIR := bin

# GCC auto-completes with libyarr.so
YARRLIB := yarr

all: $(BIN_DIR) \
    hide_self

clean: yarrcall_access_clean \
    hide_self_clean

hide_self: hide_self.c
	gcc -o $(BIN_DIR)/$@ -I$(YARR_INCLUDES) -L$(LIB_DIR) $< -l$(YARRLIB)

hide_self_clean: hide_self
	rm -f $<

$(BIN_DIR):
	mkdir $(BIN_DIR)

