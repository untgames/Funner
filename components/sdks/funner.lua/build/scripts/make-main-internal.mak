###################################################################################################
#Переопределение FUNNER_SDK
###################################################################################################
TOOLSETS            := $(patsubst $(ROOT)/build/toolsets/%.mak,%,$(wildcard $(ROOT)/build/toolsets/*.mak))
DIST_DIR_SHORT_NAME ?= dist

ifeq (,$(filter $(TOOLSET),$(TOOLSETS)))
  $(error Unknown toolset '$(TOOLSET)'. Use one of available toolsets '$(TOOLSETS)')
endif

FUNNER_SDK := $(ROOT)/$(DIST_DIR_SHORT_NAME)/$(TOOLSET)

include $(dir $(BUILD_SCRIPT))/make-main.mak
