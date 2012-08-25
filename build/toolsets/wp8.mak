###################################################################################################
#Сборка под WindowsPhone8
###################################################################################################

###################################################################################################
#Выбор конфигурации MSVC
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
COMMON_CFLAGS      += -W3 -Ox -wd4996 -nologo -FC -D "WP8" -D "ARM" -D "WINAPI_FAMILY=WINAPI_FAMILY_PHONE_APP" -MD -arch:ARMV7VE
COMMON_LINK_FLAGS  += -machine:arm

###################################################################################################
#Константы
###################################################################################################
LIB_SUFFIX              := .lib
OBJ_SUFFIX              := .obj
EXE_SUFFIX              := .exe
DLL_SUFFIX              := .dll
DLL_LIB_SUFFIX          := .lib
DLL_PREFIX              :=
PROFILES                += msvc wp8 has_windows arm
CPU_ARCH                := arm

###################################################################################################
#Конфигурация переменных расположения библиотек
###################################################################################################
INCLUDE := $(MSVC_PATH)/include
LIB     := $(MSVC_PATH)/lib/$(CPU_ARCH)

INCLUDE         := $(WP8_SDK)/include;$(WP8_SDK)/include/mincore;$(WP8_SDK)/include/minwin;$(INCLUDE)
LIB             := $(WP8_SDK)/lib/$(CPU_ARCH);$(LIB)

export INCLUDE
export LIB

###################################################################################################
#Компиляция исходников (список исходников, список подключаемых каталогов, список подключаемых файлов, каталог с объектными файлами,
#список дефайнов, флаги компиляции, pch файл, список каталогов с dll)
###################################################################################################
define tools.c++compile
export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" \
&& "$(MSVC_BIN_PATH)/cl" -c -Fo"$4\\" $(patsubst %,-I"%",$2) $(patsubst %,-FI"%",$3) $(COMMON_CFLAGS) $6 $(if $(filter -clr,$6),$(foreach dir,$8 $(DIST_BIN_DIR),-AI $(dir)),-EHsc) $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.c,$1) $(filter %.cpp,$1) $(if $7,-FI"$7" -Yc"$7" -Fp"$4\\")
endef

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки, def файл)
###################################################################################################
define tools.link
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/link" -nologo -out:"$1" $(if $(filter %.dll,$1),-dll,-entry:mainACRTStartup) $(patsubst %,-libpath:"%",$3) $(patsubst %,-include:"%",$4) $5 $2 $(COMMON_LINK_FLAGS) $(if $(map),-MAP:$(basename $1).map -MAPINFO:EXPORTS) $(if $6,-DEF:"$6")
endef

###################################################################################################
#Сборка библиотеки (имя выходного файла, список файлов)
###################################################################################################
define tools.lib
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/lib" -nologo -out:$1 $2
endef
