###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := EXTERN.LUALIB

ifeq (,$(filter iphone,$(PROFILES))$(filter wince,$(PROFILES)))
TARGETS += EXTERN.LUA_UTILS EXTERN.LUA_TESTS
endif

#Target - LUALIB
EXTERN.LUALIB.TYPE                     := static-lib
EXTERN.LUALIB.NAME                     := funner.extern.lua
EXTERN.LUALIB.INCLUDE_DIRS             := include
EXTERN.LUALIB.SOURCE_DIRS              := sources
EXTERN.LUALIB.msvc.COMPILER_CFLAGS     := -TP
EXTERN.LUALIB.vcx86-64.COMPILER_CFLAGS := -wd4334
EXTERN.LUALIB.g++.COMPILER_CFLAGS      := -x c++
EXTERN.LUALIB.clang.COMPILER_CFLAGS    := -w -x c++
EXTERN.LUALIB.android.COMPILER_CFLAGS  := -include android_lconv_wrapper.h
EXTERN.LUALIB.wince.COMPILER_CFLAGS    := -FIwince_wrapper.h -DLUA_ANSI
EXTERN.LUALIB.wp8.COMPILER_CFLAGS      := -FIwp8_wrapper.h

#Target - LUA_UTILS
EXTERN.LUA_UTILS.TYPE                   := test-suite
EXTERN.LUA_UTILS.INCLUDE_DIRS           := include sources
EXTERN.LUA_UTILS.SOURCE_DIRS            := utils
EXTERN.LUA_UTILS.LIBS                   := funner.extern.lua
EXTERN.LUA_UTILS.TARGET_DIR              = $(DIST_BIN_DIR)
EXTERN.LUA_UTILS.msvc.COMPILER_CFLAGS   := -TP
EXTERN.LUA_UTILS.g++.COMPILER_CFLAGS    := -x c++
EXTERN.LUA_UTILS.clang.COMPILER_CFLAGS  := -x c++
EXTERN.LUA_UTILS.wp8.COMPILER_CFLAGS    := -FIwp8_wrapper.h

#Target - LUA_TESTS
EXTERN.LUA_TESTS.TYPE              := test-suite
EXTERN.LUA_TESTS.SOURCE_DIRS       := tests
EXTERN.LUA_TESTS.USED_APPLICATIONS := lua