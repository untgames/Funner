###################################################################################################
#Build for WindowsPhone8
###################################################################################################

###################################################################################################
#Choose MSVC configuration
###################################################################################################
ifneq (,$(WP8_VC))
  MSVC_PATH ?= $(call convert_path,$(WP8_VC))
endif

ifneq (,$(WP8_SDK))
  SDK_DIR ?= $(call convert_path,$(WP8_SDK))
endif

ifeq (,$(MSVC_PATH))
  $(error 'Microsoft Visual C++ not detected (empty MSVC_PATH)')
endif

ifeq (,$(SDK_DIR))
  $(error 'Microsoft Windows Phone SDK not detected (empty WP8_SDK)')
endif

MSVC_BIN_PATH      := $(MSVC_PATH)/bin/x86_arm
COMMON_CFLAGS      += -W3 -Ox -wd4996 -nologo -FC -D "WP8" -D "ARM" -D "WINAPI_FAMILY=WINAPI_FAMILY_PHONE_APP" -MD -arch:ARMV7VE -AI "$(WP8_SDK)\Windows MetaData"
COMMON_CFLAGS      += -AI "$(WP8_VC)\lib\arm"
COMMON_LINK_FLAGS  += -machine:arm WindowsPhoneCore.lib funner.extern.win8_compat.lib
COMMON_IMPORTs     += link.extern.win8_compat

###################################################################################################
#Constants
###################################################################################################
LIB_SUFFIX              := .lib
OBJ_SUFFIX              := .obj
EXE_SUFFIX              := .exe
DLL_SUFFIX              := .dll
DLL_LIB_SUFFIX          := .lib
DLL_PREFIX              :=
PROFILES                += msvc wp8 has_windows arm no_dll win8
CPU_ARCH                := arm

###################################################################################################
#Configuration of libraries location variables
###################################################################################################
INCLUDE := $(MSVC_PATH)/include
LIB     := $(MSVC_PATH)/lib/$(CPU_ARCH)

INCLUDE := $(ROOT)/extern/win8_compat/include;$(WP8_SDK)/include;$(WP8_SDK)/include/mincore;$(WP8_SDK)/include/minwin;$(WP8_SDK)/include/abi;$(INCLUDE)
LIB     := $(WP8_SDK)/lib/$(CPU_ARCH);$(LIB)

export INCLUDE
export LIB

###################################################################################################
#Sources compilation (sources list, include directories list, include files list, object files directory,
#defines list, compilation flags, pch file, dlls directories list)
###################################################################################################
define tools.c++compile
export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" \
&& "$(MSVC_BIN_PATH)/cl" -c -Fo"$4\\" $(patsubst %,-I"%",$2) $(patsubst %,-FI"%",$3) $(COMMON_CFLAGS) $6 $(if $(filter -clr,$6),$(foreach dir,$8 $(DIST_BIN_DIR),-AI $(dir)),-EHsc) $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.c,$1) $(filter %.cpp,$1) $(if $7,-FI"$7" -Yc"$7" -Fp"$4\\")
endef

###################################################################################################
#Files linking (output file name, files list, static libraries directories list,
#include link symbols list, link flags, def file)
###################################################################################################
define tools.link
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/link" -nologo -out:"$1" $(if $(filter %.dll,$1),-dll) $(patsubst %,-libpath:"%",$3) $(patsubst %,-include:"%",$4) $5 $2 $(COMMON_LINK_FLAGS) $(if $(map),-MAP:$(basename $1).map -MAPINFO:EXPORTS) $(if $6,-DEF:"$6")
endef

###################################################################################################
#Library build (output file name, files list)
###################################################################################################
define tools.lib.generic
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/lib" -nologo -out:$1 $2
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && export FILE_COUNTER=0 FILE_LIST="" && for file in $2; do export FILE_COUNTER=$$(($$FILE_COUNTER + 1)) && FILE_LIST="$$FILE_LIST $$file"; if [ $$FILE_COUNTER -eq 256 ]; then $(call tools.lib.generic,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9); export FILE_COUNTER=0 FILE_LIST="$1"; fi; done && $(call tools.lib.generic,$1,$$FILE_LIST,$3,$4,$5,$6,$7,$8,$9)
endef
