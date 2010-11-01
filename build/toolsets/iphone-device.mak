###################################################################################################
#Сборка под iPhone Device 3.0 g++ armv6 (iPhone, iPhone3G, iPod Touch, iPod Touch 2G)
###################################################################################################

IPHONEOS_DEPLOYMENT_TARGET := 3.0

export IPHONEOS_DEPLOYMENT_TARGET

COMMON_CFLAGS     += -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET) #-gdwarf-2 -fobjc-call-cxx-cdtors  #флаг fobjc-call-cxx-cdtors необходим для использования с++ классов в objective-c классах в gcc версий до 4,2; gdwarf-2 - для профилирования
COMMON_LINK_FLAGS += -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET)

COMPILER_GCC := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc-4.2
LINKER_GCC   := /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++-4.2

IPHONE_SDK_PATH  := /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.1.sdk

include $(TOOLSETS_DIR)/iphone.mak

###################################################################################################
#Отладка на устройстве
###################################################################################################

#Копирование файла на устройство (имя локальных файлов, имя удалённого каталога)
define tools.install
export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
plink $(IPHONE_USER)@$(IPHONE_HOST) -pw $(IPHONE_PASSWORD) "mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)" && \
pscp -scp -r -batch -pw $(IPHONE_PASSWORD) $1 $(IPHONE_USER)@$(IPHONE_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT) && \
plink $(IPHONE_USER)@$(IPHONE_HOST) -pw $(IPHONE_PASSWORD) "chmod -R +x $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)"
endef

#Выполнение команды (команда, каталог запуска, дополнительные пути поиска библиотек и приложений)
define tools.run
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
plink $(IPHONE_USER)@$(IPHONE_HOST) -pw $(IPHONE_PASSWORD) "export PATH=\$\$$PATH:\.:$$PATH_SEARCH LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)"
endef
