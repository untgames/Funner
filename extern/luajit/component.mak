###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.LUAJIT_LIB EXTERN.LUAJIT_UTILITY EXTERN.LUAJIT_TESTS

LUAJIT_BUILD_INTERNALS_DIR := $(COMPONENT_DIR)../../$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/EXTERN.LUAJIT_BUILDVM/build-internals

#Цель - build luajit
EXTERN.LUAJIT_LIB.TYPE                   := static-lib
EXTERN.LUAJIT_LIB.NAME                   := funner.extern.luajit
EXTERN.LUAJIT_LIB.SOURCE_DIRS            := sources/ljit
EXTERN.LUAJIT_LIB.INCLUDE_DIRS           := include $(LUAJIT_BUILD_INTERNALS_DIR)
EXTERN.LUAJIT_LIB.IMPORTS                := compile.extern.libffi
EXTERN.LUAJIT_LIB.g++.COMPILER_CFLAGS    := -fomit-frame-pointer -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -fno-stack-protector -funwind-tables
EXTERN.LUAJIT_LIB.g++x86.COMPILER_CFLAGS := -march=i686
EXTERN.LUAJIT_LIB.msvc.OBJECT_FILES      := $(LUAJIT_BUILD_INTERNALS_DIR)/lj_vm.obj
#EXTERN.LUAJIT_LIB.msvc.COMPILER_DEFINES  := LJ_UNWIND_EXT=1
#EXTERN.LUAJIT_LIB.mingw.COMPILER_DEFINES  := LUAJIT_UNWIND_EXTERNAL

#Цель - build luajit
EXTERN.LUAJIT_UTILITY.TYPE                   := test-suite
EXTERN.LUAJIT_UTILITY.SOURCE_DIRS            := utils
EXTERN.LUAJIT_UTILITY.INCLUDE_DIRS           := include sources/ljit
EXTERN.LUAJIT_UTILITY.LIBS                   := funner.extern.luajit
EXTERN.LUAJIT_UTILITY.g++.COMPILER_CFLAGS    := -fomit-frame-pointer -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -fno-stack-protector
EXTERN.LUAJIT_UTILITY.g++x86.COMPILER_CFLAGS := -march=i686
EXTERN.LUAJIT_UTILITY.TARGET_DIR              = $(DIST_BIN_DIR)

#Цель - tests
EXTERN.LUAJIT_TESTS.TYPE         := test-suite
EXTERN.LUAJIT_TESTS.SOURCE_DIRS  := tests

#Генерация кода для виртуальной машины
LUAJIT_SOURCES         := $(wildcard $(COMPONENT_DIR)sources/ljit/*.c)
LUAJIT_GENERATED_FILES := lj_bcdef.h lj_ffdef.h lj_folddef.h lj_recdef.h lj_libdef.h vmdef.lua

ifeq (msvc,$(CURRENT_TOOLSET))
LUAJIT_GENERATED_FILES += lj_vm.obj
LUAJIT_OBJ_FILE_FORMAT := peobj
else
LUAJIT_GENERATED_FILES += lj_vm.s
LUAJIT_OBJ_FILE_FORMAT := machasm
endif

ifneq (,$(filter iphone,$(PROFILES)))
LUAJIT_HOST_TOOLSET := macosx-10.4
else
LUAJIT_HOST_TOOLSET := $(CURRENT_TOOLSET)
endif

LUAJIT_GENERATED_FILES := $(LUAJIT_GENERATED_FILES:%=$(LUAJIT_BUILD_INTERNALS_DIR)/%)

$(LUAJIT_SOURCES): $(LUAJIT_GENERATED_FILES)

$(LUAJIT_BUILD_INTERNALS_DIR)/lj_vm.obj: $(LUAJIT_BUILD_INTERNALS_DIR)
	@echo Generate $(notdir $@)...
	@$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m $(LUAJIT_OBJ_FILE_FORMAT) -o $@
	
$(LUAJIT_BUILD_INTERNALS_DIR)/lj_vm.s: $(LUAJIT_BUILD_INTERNALS_DIR)
	@echo Generate $(notdir $@)...
	@$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m $(LUAJIT_OBJ_FILE_FORMAT) -o $@

$(LUAJIT_BUILD_INTERNALS_DIR)/lj_%.h: $(LUAJIT_BUILD_INTERNALS_DIR)
	@echo Generate $(notdir $@)...
	@$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m $(patsubst lj_%,%,$(notdir $(basename $@))) -o $@ $(addprefix $(COMPONENT_DIR)sources/ljit/,lib_base.c lib_math.c lib_bit.c lib_string.c lib_table.c lib_io.c lib_os.c lib_package.c lib_debug.c lib_jit.c lib_ffi.c)
	
$(LUAJIT_BUILD_INTERNALS_DIR)/lj_folddef.h: $(LUAJIT_BUILD_INTERNALS_DIR)
	@echo Generate $(notdir $@)...
	@$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m folddef -o $@ $(COMPONENT_DIR)sources/ljit/lj_opt_fold.c

$(LUAJIT_BUILD_INTERNALS_DIR)/vmdef.lua: $(LUAJIT_BUILD_INTERNALS_DIR)
	@echo Generate $(notdir $@)...
	@$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m vmdef -o $@ $(addprefix $(COMPONENT_DIR)sources/ljit/,lib_base.c lib_math.c lib_bit.c lib_string.c lib_table.c lib_io.c lib_os.c lib_package.c lib_debug.c lib_jit.c)

.PHONY: $(LUAJIT_BUILD_INTERNALS_DIR)

build: BUILD.EXTERN.LUAJIT_BUILDVM.WRAPPER

BUILD.EXTERN.LUAJIT_BUILDVM.WRAPPER:
	@export LUAJIT_BUILDVM_TOOLSET=$(CURRENT_TOOLSET) TOOLSET=$(LUAJIT_HOST_TOOLSET) && $(MAKE) -C $(COMPONENT_DIR)sources/buildvm

.PHONY: BUILD.EXTERN.LUAJIT_BUILDVM.WRAPPER
