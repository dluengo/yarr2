YARR_INCLUDES := `pwd`/../include

all: yarrcall_access \
    hide_self

clean: yarrcall_access_clean \
    hide_self_clean

yarrcall_access: yarrcall_access.c
	gcc -o $@ $<

yarrcall_access_clean: yarrcall_access
	rm -f $<

hide_self: hide_self.c
	gcc -o $@ -I$(YARR_INCLUDES) $<

hide_self_clean: hide_self
	rm -f $<

