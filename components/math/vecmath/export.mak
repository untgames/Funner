export.compile.math.vecmath.INCLUDE_DIRS           := include

ifeq (,$(filter $(PROFILES),vc10))
export.compile.math.vecmath.msvc.COMPILER_DEFINES  := VECMATH_SSE
export.compile.math.vecmath.msvc.COMPILER_CFLAGS   := -arch:SSE
endif

export.info.math.vecmath.CHMS := funner.math.vecmath