###################################################################################################
#Сборка под iPhone Device 3.0 g++ armv6 (iPhone, iPhone3G, iPod Touch, iPod Touch 2G)
###################################################################################################

IPHONEOS_DEPLOYMENT_TARGET := 3.0

export IPHONEOS_DEPLOYMENT_TARGET

REMOTE_DEBUG_DIR  ?= //private/var/work/funner
COMMON_CFLAGS     += -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET) #-gdwarf-2 -fobjc-call-cxx-cdtors  #флаг fobjc-call-cxx-cdtors необходим для использования с++ классов в objective-c классах в gcc версий до 4,2; gdwarf-2 - для профилирования
COMMON_LINK_FLAGS += -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET)
COMPILER_GCC      := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc-4.2
LINKER_GCC        := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++-4.2
IPHONE_SDK_PATH   := /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.1.sdk

include $(TOOLSETS_DIR)/iphone.mak

###################################################################################################
#Отладка на устройстве
###################################################################################################

#Копирование файла на устройство (имя локальных файлов, имя удалённого каталога)
define tools.install
export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
$(call ssh_run,"mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(IPHONE_USER)@$(IPHONE_HOST),$(IPHONE_PASSWORD)) && \
$(call ssh_copy,$1,$(IPHONE_USER)@$(IPHONE_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT),$(IPHONE_PASSWORD)) && \
$(call ssh_run,"chmod -R +x $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(IPHONE_USER)@$(IPHONE_HOST),$(IPHONE_PASSWORD))
endef

#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений)
define tools.run
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_DIST_BIN_DIR_STRING=$$(cd $(DIST_BIN_DIR) && pwd) && export BIN_DIR=$(REMOTE_DEBUG_DIR)/$${SUBST_DIST_BIN_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
$(call ssh_run,"export PATH=\$\$$PATH:\.:$$PATH_SEARCH LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH BIN_DIR=$$BIN_DIR && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)",$(IPHONE_USER)@$(IPHONE_HOST),$(IPHONE_PASSWORD))
endef
