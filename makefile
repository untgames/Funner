###################################################################################################
#Eval relative path from launch path to folder containing this makefile
###################################################################################################
ROOT := $(patsubst %/,%,$(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))))

ifeq (,$(BUILD_SCRIPT))
  BUILD_SCRIPT ?= $(ROOT)/build/make-main.mak
endif

###################################################################################################
#Check that there is makefile on parent folder
###################################################################################################
ifneq (,$(wildcard ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))))
  include ../$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
else

###################################################################################################
#Include build script
###################################################################################################
  include $(BUILD_SCRIPT)
endif
