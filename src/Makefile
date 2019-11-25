# TODO: All the files that the compilation generates are stored in the current
# folder. I hate that, it's ugly, it's not elegant and it makes the folder a
# fuzz... but I'm super duper stupid when it comes to makefiles and kbuild
# files. I would like to store all that shit in a obj/ folder created
# on-the-fly under src/, but I have no idea how to do it for now and I rather
# invest my time on keep developing yarr2. If you know how to do it... please
# contact me :-*

KRNL_LIBS := /lib/modules/`uname -r`
BUILD_DIR := $(KRNL_LIBS)/build

kbuild:
	make -C $(BUILD_DIR) M=`pwd` modules

clean:
	make -C $(BUILD_DIR) M=`pwd` clean

