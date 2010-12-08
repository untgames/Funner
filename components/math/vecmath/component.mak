TARGETS := MATH.VECMATH.TESTS MATH.VECMATH.INFO

MATH.VECMATH.TESTS.TYPE         := test-suite
MATH.VECMATH.TESTS.SOURCE_DIRS  := tests
MATH.VECMATH.TESTS.IMPORTS      := compile.math.vecmath
#MATH.VECMATH.TESTS.msvc.COMPILER_CFLAGS += -Fa

#Цель - сборка документации
MATH.VECMATH.INFO.TYPE        := doxygen-info
MATH.VECMATH.INFO.CHM_NAME    := funner.math.vecmath
MATH.VECMATH.INFO.SOURCE_DIRS := include
MATH.VECMATH.INFO.IMPORTS     := compile.math.vecmath
