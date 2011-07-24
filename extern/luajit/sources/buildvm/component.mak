###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.LUAJIT_BUILDVM

LUAJIT_BUILDVM_TOOLSET     ?= $(CURRENT_TOOLSET)
LUAJIT_BUILD_INTERNALS_DIR := $(COMPONENT_DIR)../../../../$(TMP_DIR_SHORT_NAME)/$(LUAJIT_BUILDVM_TOOLSET)/EXTERN.LUAJIT_BUILDVM/build-internals

#Цель №1 - build vm
EXTERN.LUAJIT_BUILDVM.TYPE                := application
EXTERN.LUAJIT_BUILDVM.NAME                := buildvm
EXTERN.LUAJIT_BUILDVM.OUT_DIR             := $(LUAJIT_BUILD_INTERNALS_DIR)
EXTERN.LUAJIT_BUILDVM.SOURCE_DIRS         := .
EXTERN.LUAJIT_BUILDVM.INCLUDE_DIRS        := ../../include ../ljit
EXTERN.LUAJIT_BUILDVM.g++.COMPILER_CFLAGS := -fomit-frame-pointer

#По идее тоже самое должно быть для armv7, но armv7 сейчас не поддерживается в luajit
#Сборка идет в папку мака, конфликт версий!!!!!!!!
ifeq (iphone-device-armv6,$(LUAJIT_BUILDVM_TOOLSET))
EXTERN.LUAJIT_BUILDVM.COMPILER_CFLAGS := -fomit-frame-pointer -DLUAJIT_OS=LUAJIT_OS_OSX -DLUAJIT_TARGET=LUAJIT_ARCH_arm
endif