###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.LUAJIT_BUILDVM EXTERN.LUAJIT_LIB EXTERN.LUAJIT_UTILITY EXTERN.LUAJIT_TESTS

LUAJIT_BUILD_INTERNALS_DIR := ../../$(TMP_DIR_SHORT_NAME)/$(CURRENT_TOOLSET)/EXTERN.LUAJIT_BUILDVM/build-internals

#Цель №1 - build vm
EXTERN.LUAJIT_BUILDVM.TYPE                := application
EXTERN.LUAJIT_BUILDVM.NAME                := buildvm
EXTERN.LUAJIT_BUILDVM.OUT_DIR             := $(LUAJIT_BUILD_INTERNALS_DIR)
EXTERN.LUAJIT_BUILDVM.SOURCE_DIRS         := sources/buildvm
EXTERN.LUAJIT_BUILDVM.INCLUDE_DIRS        := include sources/ljit
EXTERN.LUAJIT_BUILDVM.g++.COMPILER_CFLAGS := -fomit-frame-pointer

#Цель №2 - build luajit
EXTERN.LUAJIT_LIB.TYPE                   := static-lib
EXTERN.LUAJIT_LIB.NAME                   := funner.extern.luajit
EXTERN.LUAJIT_LIB.SOURCE_DIRS            := sources/ljit
EXTERN.LUAJIT_LIB.INCLUDE_DIRS           := include $(LUAJIT_BUILD_INTERNALS_DIR)
EXTERN.LUAJIT_LIB.g++.COMPILER_CFLAGS    := -fomit-frame-pointer -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -fno-stack-protector
EXTERN.LUAJIT_LIB.g++x86.COMPILER_CFLAGS := -march=i686
EXTERN.LUAJIT_LIB.msvc.OBJECT_FILES      := $(LUAJIT_BUILD_INTERNALS_DIR)/lj_vm.obj

#Цель №3 - build luajit
EXTERN.LUAJIT_UTILITY.TYPE                   := test-suite
EXTERN.LUAJIT_UTILITY.SOURCE_DIRS            := utils
EXTERN.LUAJIT_UTILITY.INCLUDE_DIRS           := include
EXTERN.LUAJIT_UTILITY.LIBS                   := funner.extern.luajit
EXTERN.LUAJIT_UTILITY.g++.COMPILER_CFLAGS    := -fomit-frame-pointer -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -fno-stack-protector
EXTERN.LUAJIT_UTILITY.g++x86.COMPILER_CFLAGS := -march=i686
EXTERN.LUAJIT_UTILITY.TARGET_DIR              = $(DIST_BIN_DIR)

#Цель №4 - tests
EXTERN.LUAJIT_TESTS.TYPE         := test-suite
EXTERN.LUAJIT_TESTS.SOURCE_DIRS  := tests

#Генерация кода для виртуальной машины
LUAJIT_SOURCES         := $(wildcard $(COMPONENT_DIR)sources/ljit/*.c)
LUAJIT_GENERATED_FILES := lj_bcdef.h lj_ffdef.h lj_folddef.h lj_recdef.h lj_libdef.h vmdef.lua

ifeq (msvc,$(CURRENT_TOOLSET))
LUAJIT_GENERATED_FILES += lj_vm.obj
else
LUAJIT_GENERATED_FILES += lj_vm.s
endif

LUAJIT_GENERATED_FILES := $(LUAJIT_GENERATED_FILES:%=$(LUAJIT_BUILD_INTERNALS_DIR)/%)

$(LUAJIT_SOURCES): $(LUAJIT_GENERATED_FILES)

$(LUAJIT_BUILD_INTERNALS_DIR)/lj_vm.obj:
	@echo Generate $(notdir $@)...
	@$(COMPONENT_DIR)$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m peobj -o $@
	
$(LUAJIT_BUILD_INTERNALS_DIR)/lj_vm.s:
	@echo Generate $(notdir $@)...
	@$(COMPONENT_DIR)$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m coffasm -o $@

$(LUAJIT_BUILD_INTERNALS_DIR)/lj_%.h:
	@echo Generate $(notdir $@)...
	@$(COMPONENT_DIR)$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m $(patsubst $(LUAJIT_BUILD_INTERNALS_DIR)/lj_%.h,%,$@) -o $@ $(addprefix $(COMPONENT_DIR)sources/ljit/,lib_base.c lib_math.c lib_bit.c lib_string.c lib_table.c lib_io.c lib_os.c lib_package.c lib_debug.c lib_jit.c)
	
$(LUAJIT_BUILD_INTERNALS_DIR)/lj_folddef.h:
	@echo Generate $(notdir $@)...
	@$(COMPONENT_DIR)$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m folddef -o $@ $(COMPONENT_DIR)sources/ljit/lj_opt_fold.c

$(LUAJIT_BUILD_INTERNALS_DIR)/vmdef.lua:
	@echo Generate $(notdir $@)...
	@$(COMPONENT_DIR)$(LUAJIT_BUILD_INTERNALS_DIR)/buildvm -m vmdef -o $@ $(addprefix $(COMPONENT_DIR)sources/ljit/,lib_base.c lib_math.c lib_bit.c lib_string.c lib_table.c lib_io.c lib_os.c lib_package.c lib_debug.c lib_jit.c)
