TARGETS := EXTERN.ZLIB

EXTERN.ZLIB.TYPE        := static-lib
EXTERN.ZLIB.NAME        := funner.extern.zlib
EXTERN.ZLIB.SOURCE_DIRS := sources
EXTERN.ZLIB.IMPORTS     := compile.extern.zlib

EXTERN.ZLIB.emscripten.COMPILER_CFLAGS += -Wno-shift-negative-value
