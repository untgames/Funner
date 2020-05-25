###################################################################################################
#Build for iPhone
###################################################################################################

ifndef IPHONE_SDK_PATH
  $(error "Environment variable 'IPHONE_SDK_PATH' not found (use iphone-version.g++ toolset)")
endif

###################################################################################################
#Constants
###################################################################################################
EXE_SUFFIX     :=
DLL_SUFFIX     := .dylib
DLL_LIB_SUFFIX := .dylib

PROFILES += iphone cocoa unistd has_windows haswchar gles2 no_dll has_iconv
DLL_PATH := DYLD_LIBRARY_PATH

COMMON_CFLAGS               += -Os -isysroot $(IPHONE_SDK_PATH) -DIPHONE -fvisibility=hidden -Wno-unused-function -DOS_OBJECT_USE_OBJC=0
COMMON_LINK_FLAGS           += -isysroot $(IPHONE_SDK_PATH) -dead_strip
MAP_FILE_LINK_OPTION_PREFIX := -Wl,-map,

include $(TOOLSETS_DIR)/g++.mak

LINKER_START_GROUP_FLAG :=
LINKER_END_GROUP_FLAG   :=

PROFILES := $(filter-out g++,$(PROFILES))

SOURCE_FILES_SUFFIXES += mm         #Source files extension

###################################################################################################
#Shared-library linking (output file name)
###################################################################################################
define tools.link.dll
-dynamiclib -Wl,-undefined -Wl,error -install_name @loader_path/$(notdir $(basename $1))$(DLL_SUFFIX)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9) $(if $(filter %$(DLL_SUFFIX),$1), && cp $1 $(DIST_LIB_DIR) && mv $(DIST_LIB_DIR)/$(notdir $1) $(DIST_LIB_DIR)/$(LIB_PREFIX)$(notdir $1))
endef

###################################################################################################
#Build single static library
###################################################################################################
VALID_TARGET_TYPES += fat-static-lib lipo-lib
PACKAGE_COMMANDS   += lipo

#Get path to library
define get_full_library_path
$(strip $(firstword $(wildcard $(patsubst %,%/$(notdir $(strip $1)),$2))))
endef

#Search for library (library name, search paths)
define find_library
$(if $(call get_full_library_path,$1,$2),$(call get_full_library_path,$1,$2),$1)
endef

#Process target combining libraries (target name)
define process_target.fat-static-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty fat static library name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif  
  
  $1.LIB_FILE                      := $(DIST_LIB_DIR)/$(LIB_PREFIX)$$($1.NAME)$(LIB_SUFFIX)
  $1.LIB_DIRS                      := $$(strip $$($1.LIB_DIRS))
  $1.LIB_DIRS                      := $$(call specialize_paths,$$($1.LIB_DIRS)) $(DIST_LIB_DIR)
  TARGET_FILES                     := $$(TARGET_FILES) $$($1.LIB_FILE)
  DIST_DIRS                        := $$(DIST_DIRS) $$(DIST_LIB_DIR)
  $1.SOURCE_INSTALLATION_LIB_FILES := $$($1.LIB_FILE)
  $1.LIBS                          := $$($1.LIBS:%=$(LIB_PREFIX)%$(LIB_SUFFIX))
  $1.LIBS                          := $$(foreach lib,$$($1.LIBS),$$(call find_library,$$(lib),$$($1.LIB_DIRS)))
  $1.LIB_DEPS                      := $$(filter $$(addprefix %/,$$(sort $$(notdir $$($1.LIBS)))),$$(wildcard $$($1.LIB_DIRS:%=%/*)))  

  build: $$($1.LIB_FILE)

  $$($1.LIB_FILE): $$($1.LIB_DEPS)
		@echo Create fat static library $$(notdir $$($1.LIB_FILE))..
		@libtool -c -o $$@ $$(sort $$($1.LIB_DEPS)) -arch_only $(FAT_LIB_ARCH_TYPE) 
endef

#Process target combining libraries built for different archs (target name)
define process_target.lipo-lib
  $1.NAME := $$(strip $$($1.NAME))
  
  ifeq (,$$($1.NAME))
    $$(error Empty lipo library name at build target '$1' component-dir='$(COMPONENT_DIR)')
  endif

  $1.LIB_FILE                      := $(DIST_LIB_DIR)/../$(LIB_PREFIX)$$($1.NAME)$(LIB_SUFFIX)
  TARGET_FILES                     := $$(TARGET_FILES) $$($1.LIB_FILE)
  DIST_DIRS                        := $$(DIST_DIRS) $$(DIST_LIB_DIR)
  $1.SOURCE_INSTALLATION_LIB_FILES := $$($1.LIB_FILE)
  $1.LIBS                          := $$(foreach lib,$$($1.LIBS),$$(foreach profile,$$($1.PROFILES),$(DIST_LIB_DIR)/../../$$(profile)/lib/$(LIB_PREFIX)$$(lib)$(LIB_SUFFIX)))

  lipo: $$($1.LIB_FILE)

  $$($1.LIB_FILE): $$($1.LIBS)
		@echo Create lipo library $$(notdir $$@)..	
		@lipo $$($1.LIBS) -create -output $$@
endef

.PHONY: lipo
