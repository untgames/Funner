###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := FUNNER_EXTERN_LIBS

#Цель - external libraries
FUNNER_EXTERN_LIBS.TYPE       := package
FUNNER_EXTERN_LIBS.COMPONENTS := 

ifeq (,$(filter tabletos,$(PROFILES)))
FUNNER_EXTERN_LIBS.COMPONENTS += zlib
endif

FUNNER_EXTERN_LIBS.COMPONENTS += zzip lib64 pcre tiff

ifeq (,$(filter tabletos,$(PROFILES)))
FUNNER_EXTERN_LIBS.COMPONENTS += jpeg libpng freetype
endif

FUNNER_EXTERN_LIBS.COMPONENTS += devil ogg vorbis vorbisfile lua libpsd theora shiny flac

ifeq (,$(filter tabletos,$(PROFILES)))
FUNNER_EXTERN_LIBS.COMPONENTS += openssl
endif

ifeq (,$(filter iphone,$(PROFILES))$(filter android,$(PROFILES)))
  FUNNER_EXTERN_LIBS.x86.COMPONENTS := geekinfo
endif

ifeq (,$(filter beagleboard,$(PROFILES))$(filter webos,$(PROFILES))$(filter tabletos,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += curl
endif

#ifneq (,$(filter win32,$(PROFILES)))
#  FUNNER_EXTERN_LIBS.COMPONENTS += luajit
#endif

ifneq (,$(filter win32,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += win32_load_dll
endif

ifneq (,$(filter linux,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk
endif

ifneq (,$(filter android,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += openalsdk google_play_billing
endif

ifneq (,$(filter android-arm,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += android_ndk_profiler
endif

ifneq (,$(filter psp,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := psp_addons $(FUNNER_EXTERN_LIBS.COMPONENTS)
endif

ifneq (,$(filter bada,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := bada $(FUNNER_EXTERN_LIBS.COMPONENTS)
endif

ifneq (,$(filter wince,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := wcecompat wince $(FUNNER_EXTERN_LIBS.COMPONENTS) pthreads_wince
else
  FUNNER_EXTERN_LIBS.COMPONENTS += bullet mongoose
endif

ifeq (,$(filter has_iconv,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS += libiconv
endif

ifneq (,$(filter win8,$(PROFILES)))
  FUNNER_EXTERN_LIBS.COMPONENTS := win8_compat $(FUNNER_EXTERN_LIBS.COMPONENTS)
endif
