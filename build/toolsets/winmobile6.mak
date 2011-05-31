###################################################################################################
#Сборка под MSVC
###################################################################################################

###################################################################################################
#Выбор конфигурации MSVC
###################################################################################################
ifneq (,$(VS90COMNTOOLS))
  MSVC_PATH         ?= $(VS90COMNTOOLS)../../vc/ce
  MSVS_COMMON_PATH  ?= $(VS90COMNTOOLS)../../Common7/Ide
  PROFILES          += vc9
endif

ifneq (,$(VS80COMNTOOLS))
  MSVC_PATH         ?= $(VS80COMNTOOLS)../../vc/ce
  MSVS_COMMON_PATH  ?= $(VS80COMNTOOLS)../../Common7/Ide
  PROFILES          += vc8
endif

ifeq (,$(MSVS_COMMON_PATH))
  $(error 'Microsoft Visual Studio not detected (empty MSVC_PATH)')
endif

ifeq (,$(MSVC_PATH))
  $(error 'Microsoft Visual C++ not detected (empty MSVC_PATH)')
endif

ifeq (,$(WINCE_POWERTOYS))
  $(error 'WinCE PowerToys not detected (empty WINCE_POWERTOYS)')
endif

MSVC_PATH          := $(call convert_path,$(MSVC_PATH))
MSVC_BIN_PATH      := $(MSVC_PATH)/bin/x86_arm
MSVS_COMMON_PATH   := $(call convert_path,$(MSVS_COMMON_PATH))
COMMON_CFLAGS      += -W3 -Ox -wd4996 -nologo -FC -D "_WIN32_WCE=0x502" -D "UNDER_CE" -D "WIN32_PLATFORM_PSPC" -D "WINCE" -D "ARM" -D "_ARM_" -D "POCKETPC2003_UI_MODEL"
COMMON_LINK_FLAGS  += -NODEFAULTLIB:oldnames.lib coredll.lib corelibc.lib
CPU_ARCH           := armv4i
REMOTE_INSTALL_DIR := \\funner
WINCE_POWERTOYS    := $(call convert_path,$(WINCE_POWERTOYS))
CECOPY             := $(WINCE_POWERTOYS)/CEcopy/cecopy
RAPISTART          := $(WINCE_POWERTOYS)/RAPI_Start/rapistart
WINCE_LAUNCHER     := $(REMOTE_INSTALL_DIR)/dist/winmobile6/bin/wince-launcher.exe

###################################################################################################
#Константы
###################################################################################################
LIB_SUFFIX              := .lib
OBJ_SUFFIX              := .obj
EXE_SUFFIX              := .exe
DLL_SUFFIX              := .dll
DLL_LIB_SUFFIX          := .lib
DLL_PREFIX              :=
PROFILES                += msvc wince win32 has_windows arm
COMMON_LINK_FLAGS       += -subsystem:windowsce

###################################################################################################
#Конфигурация переменных расположения библиотек
###################################################################################################
INCLUDE := $(MSVC_PATH)/include
LIB     := $(MSVC_PATH)/lib/$(CPU_ARCH)

ifeq (,$(WIN_MOBILE_SDK6))
  $(error 'Microsoft SDKs not detected (empty WIN_MOBILE_SDK6)')  
endif

WIN_MOBILE_SDK6 := $(call convert_path,$(WIN_MOBILE_SDK6))
INCLUDE         := $(ROOT)/extern/wcecompat/include;$(WIN_MOBILE_SDK6)/PocketPC/include/$(CPU_ARCH);$(INCLUDE)
LIB             := $(WIN_MOBILE_SDK6)/PocketPC/lib/$(CPU_ARCH);$(LIB)

export INCLUDE
export LIB

###################################################################################################
#Компиляция исходников (список исходников, список подключаемых каталогов, список подключаемых файлов, каталог с объектными файлами,
#список дефайнов, флаги компиляции, pch файл, список каталогов с dll)
###################################################################################################
define tools.c++compile
export PATH="$(MSVS_COMMON_PATH);$(MSVC_PATH)/bin;$$PATH" \
&& "$(MSVC_BIN_PATH)/cl" -c -Fo"$4\\" $(patsubst %,-I"%",$2) $(patsubst %,-FI"%",$3) $(COMMON_CFLAGS) $6 $(if $(filter -clr,$6),$(foreach dir,$8 $(DIST_BIN_DIR),-AI $(dir)) -MD,-EHsc -MT) $(foreach def,$5,-D$(subst %,$(SPACE),$(def))) $(filter %.c,$1) $(filter %.cpp,$1) $(if $7,-FI"$7" -Yc"$7" -Fp"$4\\")
endef

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки, def файл)
###################################################################################################
define tools.link
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/link" -nologo -out:"$1" $(if $(filter %.dll,$1),-dll,-entry:mainACRTStartup) $(patsubst %,-libpath:"%",$3) $(patsubst %,-include:"_%",$4) $5 $2 $(COMMON_LINK_FLAGS) $(if $(map),-MAP:$(basename $1).map -MAPINFO:EXPORTS) $(if $6,-DEF:"$6")
endef

###################################################################################################
#Сборка библиотеки (имя выходного файла, список файлов)
###################################################################################################
define tools.lib
export PATH="$(MSVS_COMMON_PATH);$$PATH" && "$(MSVC_BIN_PATH)/lib" -nologo -out:$1 $2
endef

#Копирование файла на устройство (имя локальных файлов, имя удалённого каталога)
define tools.install
  export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
  for file in $(subst /,\\,$1); do "$(CECOPY)" //s //is $$file dev:$(REMOTE_INSTALL_DIR)\\$$(echo $$SUBST_RESULT | sed 's/\//\\/g')\\$$(basename $$file); done
endef

###################################################################################################
#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений)
###################################################################################################
define tools.run
   "$(RAPISTART)" $(subst /,\\,$(WINCE_LAUNCHER)) "$(subst /,\\,$(strip $1))" "$(subst /,\\,$(strip $1).stdout)" "$(REMOTE_INSTALL_DIR)\\$(subst /,\\,$(patsubst ./%,%,$2))" && \
   plink -P 1663 -telnet $(WINMOBILE_HOST)
endef


#$(call prepare_to_execute,$2,$3) && chmod u+x "$(CURDIR)/$(firstword $1)" && "$(CURDIR)/$(firstword $1)"
