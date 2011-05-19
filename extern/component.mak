###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS

#Цель - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := zlib zzip pcre jpeg tiff libpng devil ogg vorbis vorbisfile lua \
                                 freetype libpsd bullet theora mongoose shiny

ifeq (,$(filter iphone,$(PROFILES))$(filter beagleboard,$(PROFILES))$(filter android,$(PROFILES))$(filter webos,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += curl
endif

ifneq (,$(filter win32,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += luajit
endif

ifneq (,$(filter linux,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk
endif

ifneq (,$(filter android,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk
endif

ifneq (,$(filter psp,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := psp_addons $(FUNNER_EXTERN_LIBS.COMPONENTS)
endif

ifneq (,$(filter bada,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENT_DIRS := bada $(FUNNER_EXTERN_LIBS.COMPONENT_DIRS)
endif

ifneq (,$(filter wince,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENT_DIRS := wcecompat $(FUNNER_EXTERN_LIBS.COMPONENT_DIRS)
endif
