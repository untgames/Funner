###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := EXTERN.THEORA.SOURCES EXTERN.THEORA.TESTS

#Target - THEORA sources
EXTERN.THEORA.SOURCES.TYPE                     := static-lib
EXTERN.THEORA.SOURCES.NAME                     := funner.extern.theora
EXTERN.THEORA.SOURCES.SOURCE_DIRS              := sources
EXTERN.THEORA.SOURCES.IMPORTS                  := compile.extern.theora
EXTERN.THEORA.SOURCES.g++.COMPILER_CFLAGS      := --no-warn
EXTERN.THEORA.SOURCES.clang.COMPILER_CFLAGS    := -w
EXTERN.THEORA.SOURCES.msvc.COMPILER_CFLAGS     := -wd4018 -wd4554 -wd4700 -wd4244 -wd4405 -wd4101
EXTERN.THEORA.SOURCES.vcx86-64.COMPILER_CFLAGS := -wd4267 -wd4334
EXTERN.THEORA.SOURCES.x86.COMPILER_DEFINES     := OC_X86_ASM
EXTERN.THEORA.SOURCES.emscripten.COMPILER_CFLAGS := -Wno-logical-op-parentheses -Wno-parentheses -Wno-shift-negative-value -Wno-shift-op-parentheses -Wno-tautological-compare -Wno-unused-function

ifneq (,$(filter msvc,$(PROFILES)))
  EXTERN.THEORA.SOURCES.x86.SOURCE_DIRS := sources/x86_vc
else
  EXTERN.THEORA.SOURCES.x86.SOURCE_DIRS := sources/x86
endif

#Target - THEORA tests
EXTERN.THEORA.TESTS.TYPE                  := test-suite
EXTERN.THEORA.TESTS.SOURCE_DIRS           := tests
EXTERN.THEORA.TESTS.IMPORTS               := compile.extern.theora link.extern.theora
EXTERN.THEORA.TESTS.g++.COMPILER_CFLAGS   := --no-warn
EXTERN.THEORA.TESTS.clang.COMPILER_CFLAGS := -w
EXTERN.THEORA.TESTS.msvc.COMPILER_CFLAGS  := -wd4244
EXTERN.THEORA.TESTS.emscripten.COMPILER_CFLAGS := -Wno-unused-function