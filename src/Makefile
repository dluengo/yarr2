KRNL_LIBS = /lib/modules/`uname -r`
BUILD_DIR = $(KRNL_LIBS)/build

ifneq ($(DEBUG),0)
    EXTRA_CFLAGS += -DDEBUG
endif

kbuild:
	make -C $(BUILD_DIR) M=`pwd` modules

clean:
	make -C $(BUILD_DIR) M=`pwd` clean

