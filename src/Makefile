# TODO: All the files that the compilation generates are stored in the current
# folder. I hate that, it's ugly, it's not elegant and it makes the folder a
# fuzz... but I'm super duper stupid when it comes to makefiles and kbuild
# files. I would like to store all that shit in a obj/ folder created
# on-the-fly under src/, but I have no idea how to do it for now and I rather
# invest my time on keep developing yarr2. If you know how to do it... please
# contact me :-*

# Use it when you want to compile against a specific kernel installed in your
# system but not the running one.
ifneq ($(KVERSION),)
    KDIR := /lib/modules/$(KVERSION)/build
else
    KDIR := /lib/modules/`uname -r`/build
endif

kbuild:
	make -C $(KDIR) M=`pwd` modules

clean:
	make -C $(KDIR) M=`pwd` clean

#$(ODIR):
#	mkdir $(ODIR)

