TARGETS := EXTERN.LIB64

EXTERN.LIB64.TYPE                     := static-lib
EXTERN.LIB64.NAME                     := funner.extern.lib64
EXTERN.LIB64.SOURCE_DIRS              := sources
EXTERN.LIB64.vcx86-64.COMPILER_CFLAGS := -wd4244
EXTERN.LIB64.IMPORTS                  := compile.extern.lib64
