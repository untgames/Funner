ifeq (,$(filter iphone,$(PROFILES)))
SOURCE_FILES := *.c
else
SOURCE_FILES := lcode.c ldo.c lgc.c loadlib.c lstring.c ltablib.c lvm.c print.c lapi.c lobject.c lparser.c ltm.c ldblib.c ldump.c linit.c lmathlib.c lstrlib.c lzio.c lauxlib.c ldebug.c lfunc.c liolib.c lmem.c lopcodes.c lstate.c ltable.c lundump.c lbaselib.c llex.c
endif
