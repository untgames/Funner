TARGETS.wp8 := EXTERN.WP8COMPAT 
TARGETS     := EXTERN.WP8COMPAT.STDIO

EXTERN.WP8COMPAT.TYPE            := static-lib
EXTERN.WP8COMPAT.NAME            := funner.extern.wp8compat
EXTERN.WP8COMPAT.SOURCE_DIRS     := sources/common
EXTERN.WP8COMPAT.COMPILER_CFLAGS := -ZW

EXTERN.WP8COMPAT.STDIO.TYPE         := static-lib
EXTERN.WP8COMPAT.STDIO.NAME         := funner.extern.wp8compat.stdio
EXTERN.WP8COMPAT.STDIO.SOURCE_DIRS  := sources/stdio
