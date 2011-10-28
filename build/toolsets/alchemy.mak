###################################################################################################
#Сборка под Alchemy
###################################################################################################
ifeq ($(strip $(ALCHEMY_HOME)),)
  $(error "Please set ALCHEMY_HOME variable in your environment")
endif

ifeq ($(strip $(FLEX_HOME)),)
  $(error "Please set FLEX_HOME variable in your environment")
endif

ifeq ($(strip $(CYGHOME)),)
  $(error "Please set CYGHOME in your environment")
endif

###################################################################################################
#Константы
###################################################################################################
CYGWIN_BIN        := /$(subst :,,$(call convert_path,$(CYGHOME)))/bin
ALCHEMY_SDK       := /$(subst :,,$(call convert_path,$(ALCHEMY_HOME)))
FLEX_SDK          := /$(subst :,,$(call convert_path,$(FLEX_HOME)))
BUILD_PATHS       := $(ALCHEMY_SDK)/achacks:$(ALCHEMY_SDK)/bin:$(FLEX_SDK)/bin:$(CYGWIN_BIN)
PROFILES          += unistd haswchar no_dll alchemy
EXE_SUFFIX        := .swc
DLL_SUFFIX        := .so #not used, only for compatibility with gcc build scripts
DLL_PREFIX        :=
COMPILER_GCC      := $(BUILD_DIR)platforms/alchemy/gcc
LINKER_GCC        := $(BUILD_DIR)platforms/alchemy/gcc
LIB_GCC           := ar
ADL               := /cygdrive$(FLEX_SDK)/bin/adl
BRIDGE            := swfbridge
COMMON_CPPFLAGS   += -fexceptions -frtti -I"/cygdrive$(ALCHEMY_SDK)/avm2-libc/include/c++/3.4"
COMMON_CFLAGS     += -DALCHEMY
COMMON_LINK_FLAGS += -swc -Wl,-L,$(DIST_BIN_DIR) -Wl,--no-undefined
CYGWIN            := nodosfilewarning

export CYGWIN
export ADL

include $(TOOLSETS_DIR)/g++.mak

###################################################################################################
#Переопределения вызовов утилит
###################################################################################################
define tools.c++compile
export PATH=$(BUILD_PATHS):$$PATH && $(CYGWIN_BIN)/sh -c 'export ALCHEMY_SDK=/$(ALCHEMY_SDK) && $(call tools.g++.c++compile,$1,$2,$3,$4,$5,$6,$7,$8,$9)'
endef

###################################################################################################
#Линковка файлов (имя выходного файла, список файлов, список каталогов со статическими библиотеками,
#список подключаемых символов линковки, флаги линковки, def-файл, файл ошибок)
###################################################################################################
define tools.link
export PATH=$(BUILD_PATHS):$$PATH && $(CYGWIN_BIN)/sh -c 'export ALCHEMY_SDK=/$(ALCHEMY_SDK) && $(call tools.g++.link,$1,$2,$3,$4,$5,$6,$1.stderr,$8,$9); if [ $$? -ne 0 ]; then echo $1.stderr; exit 1; fi'
endef

define tools.lib
export PATH=$(BUILD_PATHS):$$PATH && $(CYGWIN_BIN)/sh -c '$(call tools.g++.lib,$1,$2,$3,$4,$5,$6,$7,$8,$9)'
endef

###################################################################################################
#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений)
###################################################################################################
define tools.run
$(call prepare_to_execute,$2,$3) && export PATH=$(BUILD_PATHS):$$PATH && swfbridge "$(CURDIR)/$(firstword $1)"
endef
