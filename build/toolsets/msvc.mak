###################################################################################################
#Сборка под MSVC
###################################################################################################

###################################################################################################
#Выбор конфигурации MSVC
###################################################################################################
ifneq (,$(VS90COMNTOOLS))
  MSVC_PATH        ?= $(VS90COMNTOOLS)../../vc
  MSVS_COMMON_PATH ?= $(VS90COMNTOOLS)../../Common7/Ide
endif

ifneq (,$(VS80COMNTOOLS))
  MSVC_PATH        ?= $(VS80COMNTOOLS)../../vc
  MSVS_COMMON_PATH ?= $(VS80COMNTOOLS)../../Common7/Ide
endif

ifneq (,$(VS71COMNTOOLS))
  MSVC_PATH        ?= $(VS71COMNTOOLS)../../vc7
  MSVS_COMMON_PATH ?= $(VS71COMNTOOLS)../../Common7/Ide
endif

ifneq (,$(VCTOOLKITINSTALLDIR))
  MSVC_PATH        ?= $(VCTOOLKITINSTALLDIR).
  MSVS_COMMON_PATH ?= $(VCTOOLKITINSTALLDIR)bin
endif

ifeq (,$(MSVS_COMMON_PATH))
  $(error 'Microsoft Visual Studio not detected (empty MSVC_PATH)')
endif

ifeq (,$(MSVC_PATH))
  $(error 'Microsoft Visual C++ not detected (empty MSVC_PATH)')
endif

MSVC_PATH        := $(call convert_path,$(MSVC_PATH))
MSVS_COMMON_PATH := $(call convert_path,$(MSVS_COMMON_PATH))
MSVC_PATH        := $(MSVC_PATH);$(MSVS_COMMON_PATH);$$PATH

###################################################################################################
#Константы
###################################################################################################
LIB_SUFFIX := lib
OBJ_SUFFIX := obj
EXE_SUFFIX := exe
DLL_SUFFIX := dll

###################################################################################################
#Конфигурация переменных расположения библиотек
###################################################################################################
INCLUDE := $(MSVC_DIR)/include;$(MSVC_DIR)/platformsdk/include;$(INCLUDE)
LIB     := $(MSVC_DIR)/lib;$(MSVC_DIR)/platformsdk/lib;$(LIB)

export INCLUDE
export LIB

###################################################################################################
#Компиляция исходников (список исходников, список подключаемых каталогов, каталог с объектными файлами,
#список дефайнов, флаги компиляции, pch файл)
###################################################################################################
define tools.c++compile
export PATH="$(MSVC_PATH)" && cl -nologo -c -EHsc -W3 -wd4996 $(if $(analyze),-analyze) -FC -Fo"$3\\" $(patsubst %,-I"%",$2) $5 $(patsubst %,-D%,$4) $1 $(if $6,-FI"$6" -Yc"$6" -Fp"$3\\")
endef

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки)
###################################################################################################
define tools.link
export PATH="$(MSVC_PATH)" && link -nologo -out:"$1" $(if $(filter %.dll,$1),-dll) $(patsubst %,-libpath:"%",$3) $(patsubst %,-include:"_%",$4) $5 $2
endef

###################################################################################################
#Сборка библиотеки (имя выходного файла, список файлов)
###################################################################################################
define tools.lib
export PATH="$(MSVC_PATH)" && lib -nologo -out:$1 $2
endef
