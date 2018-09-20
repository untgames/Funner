###################################################################################################
#Build for iPhone Device 7.0
###################################################################################################

IPHONEOS_DEPLOYMENT_TARGET ?= 7.0

export IPHONEOS_DEPLOYMENT_TARGET

XCODE_PATH ?= /Applications/Xcode.app

REMOTE_DEBUG_DIR  ?= //private/var/work/funner
PROFILES          += arm clang
COMMON_CFLAGS     += -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET) -fstrict-aliasing -fno-common #-gdwarf-2 #flag gdwarf-2 - needed for profiling
COMMON_CPPFLAGS   += -fexceptions -frtti
COMMON_LINK_FLAGS += -miphoneos-version-min=$(IPHONEOS_DEPLOYMENT_TARGET)
COMPILER_GCC      := $(XCODE_PATH)/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang
LINKER_GCC        := $(XCODE_PATH)/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++
IPHONE_SDK_PATH   := $(XCODE_PATH)/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk

include $(TOOLSETS_DIR)/common/iphone.mak

###################################################################################################
#Debug on device
###################################################################################################

#Copy files to devices (local files name, remote directory name)
define tools.install
export SUBST_STRING=$$(cd $2 && pwd) SUBST_SUBSTRING=$$(cd $(ROOT) && pwd)/ && export SUBST_RESULT=$${SUBST_STRING/#$$SUBST_SUBSTRING/} && \
$(call ssh_run,"mkdir -p $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(IPHONE_USER)@$(IPHONE_HOST),$(IPHONE_PASSWORD)) && \
$(call ssh_copy,$1,$(IPHONE_USER)@$(IPHONE_HOST):$(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT),$(IPHONE_PASSWORD)) && \
$(call ssh_run,"chmod -R +x $(REMOTE_DEBUG_DIR)/$$(echo $$SUBST_RESULT)",$(IPHONE_USER)@$(IPHONE_HOST),$(IPHONE_PASSWORD))
endef

#Execute command (command, launch directory, additional search paths for libraries and executables)
define tools.run
export ROOT_SUBSTRING=$$(cd $(ROOT) && pwd)/ && \
export SUBST_DIR_STRING=$$(cd $2 && pwd) && export SUBST_DIR_RESULT=$(REMOTE_DEBUG_DIR)/$${SUBST_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export PATH_SEARCH="$(foreach path,$3,$$(export SUBST_PATH_STRING=$$(cd $(path) && pwd) && echo $(REMOTE_DEBUG_DIR)/$${SUBST_PATH_STRING/#$$ROOT_SUBSTRING/}))" && \
export PATH_SEARCH=$${PATH_SEARCH/\ /:} && \
export SUBST_DIST_BIN_DIR_STRING=$$(cd $(DIST_BIN_DIR) && pwd) && export BIN_DIR=$(REMOTE_DEBUG_DIR)/$${SUBST_DIST_BIN_DIR_STRING/#$$ROOT_SUBSTRING/} && \
export SUBST_CMD_STRING=$$(cd $(dir $(firstword $1)) && pwd)/$(notdir $(firstword $1)) && export SUBST_COMMAND=$(REMOTE_DEBUG_DIR)/$${SUBST_CMD_STRING/#$$ROOT_SUBSTRING/} && \
$(call ssh_run,"export PATH=\$\$$PATH:\.:$$PATH_SEARCH LD_LIBRARY_PATH=\$\$$LD_LIBRARY_PATH:\.:$$PATH_SEARCH BIN_DIR=$$BIN_DIR && mkdir -p $$(echo $$SUBST_DIR_RESULT) && cd $$(echo $$SUBST_DIR_RESULT) && $$(echo $$SUBST_COMMAND) $(subst $(firstword $1),,$1)",$(IPHONE_USER)@$(IPHONE_HOST),$(IPHONE_PASSWORD))
endef
