###################################################################################################
#Build for g++
###################################################################################################

###################################################################################################
#Constants
###################################################################################################
LIB_SUFFIX                  ?= .a
OBJ_SUFFIX                  ?= .o
EXE_SUFFIX                  ?= .exe
DLL_SUFFIX                  ?= .dll
DLL_PREFIX                  ?=
DLL_LIB_SUFFIX              ?= .a
LIB_PREFIX                  ?= lib
COMPILER_GCC                ?= gcc
LINKER_GCC                  ?= g++
LIB_GCC                     ?= libtool
PROFILES                    += g++
DEFAULT_LIBS                +=
COMMON_CFLAGS               := -Os -Wall -Wno-format -ffunction-sections -fdata-sections $(COMMON_CFLAGS)
MAP_FILE_LINK_OPTION_PREFIX ?= -Wl,-Map=
DISABLE_CPP_WARNINGS        += -Wno-invalid-offsetof
SOURCE_FILES_SUFFIXES       += s

###################################################################################################
#Sources compilation (source file, include directories list, include files list, object files directory,
#defines list, compilation flags, pch file, dlls directories list)
###################################################################################################
define tools.gcc.compile
echo $1 && $(COMPILER_GCC) -c -o "$4/$$(basename $$src $${src##*.})$(OBJ_SUFFIX:.%=%)" $(patsubst %,-I "%",$2) $(patsubst %,-include "%",$3) $6 $(foreach def,$5,-D $(subst %,$(SPACE),$(def))) $(patsubst %,-F%,$9) $1
endef

define tools.g++.c++compile
$(call for_each_file,src,$(if $(filter -x c++,$6),,$(filter %.c,$1)),$(call tools.gcc.compile,$$src,$2,$3,$4,$5,$(COMMON_CFLAGS) $6,$7,$8,$9)) && \
$(call for_each_file,src,$(filter %.cpp,$1)$(if $(filter -x c++,$6), $(filter %.c,$1)),$(call tools.gcc.compile,$$src,$2,$3,$4,$5,$(COMMON_CFLAGS) $(COMMON_CPPFLAGS) $(DISABLE_CPP_WARNINGS) $6,$7,$8,$9)) && \
$(call for_each_file,src,$(filter %.mm,$1),$(call tools.gcc.compile,$$src,$2,$3,$4,$5,$(COMMON_CFLAGS) $(COMMON_CPPFLAGS) $(COMMON_MMFLAGS) $(DISABLE_CPP_WARNINGS) $6,$7,$8,$9)) && \
$(call for_each_file,src,$(filter %.s,$1),$(call tools.gcc.compile,$$src,$2,$3,$4,$5,$(COMMON_CFLAGS) $6,$7,$8,$9)) 
endef

###################################################################################################
#Shared-library linking (output file name)
###################################################################################################
define tools.link.dll
-shared -Wl,--out-implib,$(dir $1)$(LIB_PREFIX)$(notdir $(basename $1))$(LIB_SUFFIX)
endef

define tools.link.deffile
-Wl,--output-def,$1
endef

###################################################################################################
#Linking (output file name, files list, static libraries directories list,
#include link symbols list, link flags, def-file, errors file)
###################################################################################################
define tools.g++.link
$(LINKER_GCC) -o "$1" $(if $(filter %$(DLL_SUFFIX),$1),$(call tools.link.dll,$1)) $(filter-out lib%.a,$2) $(foreach dir,$3,-L$(dir)) $(patsubst lib%.a,-l%,$(filter lib%.a,$(filter-out $(EXCLUDE_LIBS:%=lib%.a),$2)) $(DEFAULT_LIBS) $(COMMON_LINK_FLAGS) $5 $(patsubst %,-u _%,$4)) $(patsubst %,-F%,$8) $(if $6,$(call tools.link.deffile,$6)) $(if $7, 2> $7) && chmod u+x "$1"
endef
#$(LINKER_GCC) -o "$1" $(if $(filter %$(DLL_SUFFIX),$1),$(call tools.link.dll,$1)) $(filter-out lib%.a,$2) $(foreach dir,$3,-L$(dir)) $(patsubst lib%.a,-l%,$(filter lib%.a,$(filter-out $(EXCLUDE_LIBS:%=lib%.a),$2)) $(DEFAULT_LIBS) $(COMMON_LINK_FLAGS) $5 $(patsubst %,-u _%,$4)) $(patsubst %,-F%,$8) $(if $6,$(call tools.link.deffile,$6)) $(MAP_FILE_LINK_OPTION_PREFIX)$(basename $1).map $(if $7, 2> $7) && chmod u+x "$1"

###################################################################################################
#Commands execution (command, launch directory, additional search paths for libraries and applications)
###################################################################################################
define tools.run
$(call prepare_to_execute,$2,$3) && chmod u+x "$(CURDIR)/$(firstword $1)" && "$(CURDIR)/$(firstword $1)"
endef

###################################################################################################
#Library build (output file name, files list)
###################################################################################################
ifneq (,$(filter libtool,$(LIB_GCC)))
  define tools.g++.lib
    $(LIB_GCC) -static -o $1 $2
  endef
else
  define tools.g++.lib
    $(LIB_GCC) rcus $1 $2
  endef
endif

define tools.c++compile
$(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.link
$(call tools.g++.link,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef

define tools.lib
$(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)
endef
