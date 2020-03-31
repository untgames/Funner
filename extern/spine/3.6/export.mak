#compile exports
export.compile.extern.spine_3.6.INCLUDE_DIRS          := include
export.compile.extern.spine_3.6.g++.COMPILER_CFLAGS   := -include spine/funner_defines.h
export.compile.extern.spine_3.6.clang.COMPILER_CFLAGS := -include spine/funner_defines.h
export.compile.extern.spine_3.6.msvc.COMPILER_CFLAGS  := -FIspine/funner_defines.h
export.compile.extern.spine_3.6.COMPILER_DEFINES      := SPINE_VERSION=36

#link exports
export.link.extern.spine_3.6.LIBS := funner.extern.spine_3.6
