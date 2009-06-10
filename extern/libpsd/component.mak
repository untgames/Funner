TARGETS := EXTERN.LIBPSD

EXTERN.LIBPSD.TYPE                := static-lib
EXTERN.LIBPSD.NAME                := funner.extern.libpsd
EXTERN.LIBPSD.SOURCE_DIRS         := sources
EXTERN.LIBPSD.IMPORTS             := compile.static.mak ../zlib/compile.static.mak ../jpeg/compile.static.mak
EXTERN.LIBPSD.g++.COMPILER_CFLAGS := --no-warn

