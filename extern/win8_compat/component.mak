TARGETS.wp8 := EXTERN.WIN8_COMPAT 
TARGETS     := EXTERN.WIN8_COMPAT.STDIO

EXTERN.WIN8_COMPAT.TYPE            := static-lib
EXTERN.WIN8_COMPAT.NAME            := funner.extern.win8_compat
EXTERN.WIN8_COMPAT.SOURCE_DIRS     := sources/common
EXTERN.WIN8_COMPAT.COMPILER_CFLAGS := -ZW

EXTERN.WIN8_COMPAT.STDIO.TYPE         := static-lib
EXTERN.WIN8_COMPAT.STDIO.NAME         := funner.extern.win8_compat.stdio
EXTERN.WIN8_COMPAT.STDIO.SOURCE_DIRS  := sources/stdio
