TARGETS := EXTERN.PNG

EXTERN.PNG.TYPE                    := static-lib
EXTERN.PNG.NAME                    := funner.extern.libpng
EXTERN.PNG.INCLUDE_DIRS            := include ../zlib/include
EXTERN.PNG.SOURCE_DIRS             := sources
EXTERN.PNG.arm.SOURCE_DIRS         := sources/arm
EXTERN.PNG.x86-64.SOURCE_DIRS      := sources/intel
EXTERN.PNG.x86-64.COMPILER_DEFINES := PNG_INTEL_SSE
