###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS

#���� - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := zlib zzip pcre jpeg tiff libpng devil ogg vorbis vorbisfile lua \
                                 freetype libpsd bullet theora mongoose shiny

ifeq (,$(filter iphone,$(PROFILES))$(filter beagleboard,$(PROFILES))$(filter android,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += curl
endif

ifneq (,$(filter win32,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += luajit
endif

ifneq (,$(filter linux,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk
endif

ifeq (,$(filter android,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk
endif

ifeq (,$(filter psp,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := psp_addons $(FUNNER_EXTERN_LIBS.COMPONENTS)
endif
