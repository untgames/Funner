COMPONENT_DIRS := core

ifneq (,$(filter android,$(PROFILES)))
  COMPONENT_DIRS += google_push_notifications# amazon_push_notifications
endif

ifneq (,$(filter iphone,$(PROFILES)))
  COMPONENT_DIRS += ios_push_notifications
endif
