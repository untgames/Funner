TARGETS := EXTERN.SPINE_3_8

EXTERN.SPINE_3_8.TYPE                  := static-lib
EXTERN.SPINE_3_8.NAME                  := funner.extern.spine_3.8
EXTERN.SPINE_3_8.IMPORTS               := compile.extern.spine_3.8
EXTERN.SPINE_3_8.SOURCE_DIRS           := sources/spine
EXTERN.SPINE_3_8.g++.COMPILER_CFLAGS   := -x c++ --no-warn
EXTERN.SPINE_3_8.clang.COMPILER_CFLAGS := -x c++
EXTERN.SPINE_3_8.msvc.COMPILER_CFLAGS  := -TP -W0
