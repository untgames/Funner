COMPONENT_DIRS := core

ifneq (,$(filter android,$(PROFILES)))
  COMPONENT_DIRS += android_store amazon_store
endif

ifneq (,$(filter iphone,$(PROFILES)))
  COMPONENT_DIRS += ios_store
endif
