#compile exports
export.compile.extern.spine_3.8.INCLUDE_DIRS          := include
export.compile.extern.spine_3.8.g++.COMPILER_CFLAGS   := -include spine/funner_defines.h
export.compile.extern.spine_3.8.clang.COMPILER_CFLAGS := -include spine/funner_defines.h
export.compile.extern.spine_3.8.msvc.COMPILER_CFLAGS  := -FIspine/funner_defines.h
export.compile.extern.spine_3.8.COMPILER_DEFINES      := SPINE_VERSION=38

#link exports
export.link.extern.spine_3.8.LIBS := funner.extern.spine_3.8
