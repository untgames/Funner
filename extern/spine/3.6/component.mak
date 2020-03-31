TARGETS := EXTERN.SPINE_3_6

EXTERN.SPINE_3_6.TYPE                  := static-lib
EXTERN.SPINE_3_6.NAME                  := funner.extern.spine_3.6
EXTERN.SPINE_3_6.IMPORTS               := compile.extern.spine_3.6
EXTERN.SPINE_3_6.SOURCE_DIRS           := sources/spine
EXTERN.SPINE_3_6.g++.COMPILER_CFLAGS   := -x c++ --no-warn
EXTERN.SPINE_3_6.clang.COMPILER_CFLAGS := -x c++
EXTERN.SPINE_3_6.msvc.COMPILER_CFLAGS  := -TP -W0
