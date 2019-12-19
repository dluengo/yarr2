YARR_INCLUDES := `pwd`/../include
LIB_DIR := `pwd`/../lib/bin
BIN_DIR := bin

# GCC auto-completes with libyarr.so
YARRLIB := yarr

all: $(BIN_DIR) \
    hide_self \
    threads_id

clean:
	rm -rf $(BIN_DIR)

hide_self: hide_self.c
	gcc -o $(BIN_DIR)/$@ -I$(YARR_INCLUDES) -L$(LIB_DIR) $< -l$(YARRLIB)

hide_self_clean: hide_self
	rm -f $(BIN_DIR)/$<

threads_id: threads_id.c
	gcc -pthread -o $(BIN_DIR)/$@ -I$(YARR_INCLUDES) -L$(LIB_DIR) $< -l$(YARRLIB)

threads_id_clean: threads_id
	rm -f $(BIN_DIR)/$<

$(BIN_DIR):
	mkdir $(BIN_DIR)

