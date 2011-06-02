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
