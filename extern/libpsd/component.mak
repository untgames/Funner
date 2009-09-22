TARGETS := EXTERN.LIBPSD

EXTERN.LIBPSD.TYPE                := static-lib
EXTERN.LIBPSD.NAME                := funner.extern.libpsd
EXTERN.LIBPSD.SOURCE_DIRS         := sources
EXTERN.LIBPSD.IMPORTS             := compile.extern.libpsd compile.extern.zlib compile.extern.jpeg
EXTERN.LIBPSD.g++.COMPILER_CFLAGS := --no-warn
