###################################################################################################
#¬ыбор конфигурации MSVC
###################################################################################################
COMMON_CFLAGS      += -arch:IA32
COMMON_LINK_FLAGS  += -machine:x86
PROFILES           += x86 x86_winrt
CPU_ARCH           := x86
TARGET_PLATFORM    := Win32

APPX_LAUNCHER_PACKAGE_NAME := 551efd59-0678-4f51-9f33-ebbbcfe14e4f_qqggmkazgf5dr
APPX_REMOTE_DIR            := $(call convert_path,${LOCALAPPDATA}/Packages/$(APPX_LAUNCHER_PACKAGE_NAME)/LocalState)
APPX_APP_NAME              := $(APPX_LAUNCHER_PACKAGE_NAME)!App

include $(TOOLSETS_DIR)/common/winrt.mak

###################################################################################################
#¬ыполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений)
###################################################################################################
define tools.run
$(call prepare_to_execute,$2,$3) && chmod u+x "$(CURDIR)/$(firstword $1)" && "$(CURDIR)/$(firstword $1)"
endef

# опирование файла на устройство (им€ локальных файлов, им€ удалЄнного каталога)
define tools.install
export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
mkdir -p $(APPX_REMOTE_DIR)/$$(echo $$SUBST_RESULT) && \
cp -f $(strip $1) $(APPX_REMOTE_DIR)/$$(echo $$SUBST_RESULT)
endef

#¬ыполнение команды из пакета (команда, каталог запуска, дополнительные пути поиска библиотек и приложений, список динамических библиотек)
define tools.run.winrt_x86_package
CheckNetIsolation.exe LoopbackExempt \-a \-n=$(APPX_LAUNCHER_PACKAGE_NAME) > nul && \
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
$(DIST_BIN_DIR)/win8-app-launcher $(APPX_APP_NAME) $$(echo $$SUBST_COMMAND) $$(echo $$SUBST_DIR_RESULT) $(args)
endef
