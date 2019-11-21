KRNL_LIBS = /lib/modules/`uname -r`
BUILD_DIR = $(KRNL_LIBS)/build

kbuild:
	make -C $(BUILD_DIR) M=`pwd` modules

clean:
	make -C $(BUILD_DIR) M=`pwd` clean

