###################################################################################################
#Сборка под MSVC
###################################################################################################

###################################################################################################
#Выбор конфигурации MSVC
###################################################################################################
ifneq (,$(VS90COMNTOOLS))
  MSVC_PATH         ?= $(VS90COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS90COMNTOOLS)../../Common7/Ide
  PROFILES          += vc9 haswchar
endif

ifneq (,$(VS80COMNTOOLS))
  MSVC_PATH         ?= $(VS80COMNTOOLS)../../vc
  MSVS_COMMON_PATH  ?= $(VS80COMNTOOLS)../../Common7/Ide
  PLATFORM_SDK_PATH ?= $(MSVC_PATH)/platformsdk
  PROFILES          += vc8 haswchar
endif

ifneq (,$(VS71COMNTOOLS))
  MSVC_PATH         ?= $(VS71COMNTOOLS)../../vc7
  MSVS_COMMON_PATH  ?= $(VS71COMNTOOLS)../../Common7/Ide
  PLATFORM_SDK_PATH ?= $(MSVC_PATH)/platformsdk
  PROFILES          += vc7
endif

ifneq (,$(VCTOOLKITINSTALLDIR))
  MSVC_PATH         ?= $(VCTOOLKITINSTALLDIR).
  MSVS_COMMON_PATH  ?= $(VCTOOLKITINSTALLDIR)bin
  PROFILES          += vc7
endif

ifneq (,$(filter vc7,$(PROFILES)))
  COMMON_CFLAGS += -wd4675 -GR
  PROFILES      += nowchar
endif

ifeq (,$(MSVS_COMMON_PATH))
  $(error 'Microsoft Visual Studio not detected (empty MSVC_PATH)')
endif

ifeq (,$(MSVC_PATH))
  $(error 'Microsoft Visual C++ not detected (empty MSVC_PATH)')
endif

MSVC_PATH        := $(call convert_path,$(MSVC_PATH))
MSVC_BIN_PATH    := $(MSVC_PATH)/bin
MSVS_COMMON_PATH := $(call convert_path,$(MSVS_COMMON_PATH))
COMMON_CFLAGS    += -W3 -Ox -wd4996 $(if $(analyze),-analyze) -nologo -FC

###################################################################################################
#Константы
###################################################################################################
LIB_SUFFIX := lib
OBJ_SUFFIX := obj
EXE_SUFFIX := exe
DLL_SUFFIX := dll
PROFILES   += msvc win32
COMMON_LINK_FLAGS += -stack:128000

###################################################################################################
#Конфигурация переменных расположения библиотек
###################################################################################################
INCLUDE := $(MSVC_PATH)/include;$(INCLUDE)
LIB     := $(MSVC_PATH)/lib;$(LIB)

ifneq (,$(PLATFORM_SDK_PATH))

INCLUDE := $(PLATFORM_SDK_PATH)/include;$(INCLUDE)
LIB     := $(PLATFORM_SDK_PATH)/lib;$(LIB)

endif

export INCLUDE
export LIB

###################################################################################################
#Компиляция исходников (список исходников, список подключаемых каталогов, каталог с объектными файлами,
#список дефайнов, флаги компиляции, pch файл, список каталогов с dll)
###################################################################################################
define tools.c++compile
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/cl" -c -Fo"$3\\" $(patsubst %,-I"%",$2) $(COMMON_CFLAGS) $5 $(if $(filter -clr,$5),$(foreach dir,$7,-AI $(dir)),-EHsc)  $(patsubst %,-D%,$4) $1 $(if $6,-FI"$6" -Yc"$6" -Fp"$3\\")
endef

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки)
###################################################################################################
define tools.link
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/link" -nologo -out:"$1" $(if $(filter %.dll,$1),-dll) $(patsubst %,-libpath:"%",$3) $(patsubst %,-include:"_%",$4) $5 $2 $(COMMON_LINK_FLAGS) $(if $(map),-MAP:$(basename $1).map -MAPINFO:EXPORTS)
endef

###################################################################################################
#Сборка библиотеки (имя выходного файла, список файлов)
###################################################################################################
define tools.lib
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/lib" -nologo -out:$1 $2
endef
