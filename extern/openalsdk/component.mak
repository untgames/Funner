###################################################################################################
#Список целей
###################################################################################################
#TARGETS.msvc    := EXTERN.OPENALLIB EXTERN.OPENAL_TESTS
TARGETS.linux   := EXTERN.OPENALLIB EXTERN.OPENAL_TESTS
TARGETS.android := EXTERN.OPENALLIB EXTERN.OPENAL_TESTS
TARGETS.wp8     := EXTERN.OPENALLIB EXTERN.OPENAL_TESTS

#Цель №1 - OPENALLIB
ifneq (,$(filter no_dll,$(PROFILES)))
ifneq (,$(filter android,$(PROFILES))$(filter wp8,$(PROFILES)))
EXTERN.OPENALLIB.TYPE                       := dynamic-lib
else
EXTERN.OPENALLIB.TYPE                       := static-lib
endif
else
EXTERN.OPENALLIB.TYPE                       := dynamic-lib
endif
EXTERN.OPENALLIB.NAME                       := funner.extern.openal32
EXTERN.OPENALLIB.INCLUDE_DIRS               := include sources sources/openal32
EXTERN.OPENALLIB.SOURCE_DIRS                := sources/openal32 sources/alc
EXTERN.OPENALLIB.g++.COMPILER_DEFINES       := AL_ALEXT_PROTOTYPES _GNU_SOURCE=1 AL_BUILD_LIBRARY HAVE_GCC_VISIBILITY
EXTERN.OPENALLIB.msvc.COMPILER_DEFINES      := AL_ALEXT_PROTOTYPES AL_BUILD_LIBRARY snprintf=_snprintf isfinite=_finite strcasecmp=_stricmp strncasecmp=_strnicmp
EXTERN.OPENALLIB.linux.COMPILER_CFLAGS      := -pthread --no-warn
EXTERN.OPENALLIB.linux.LIBS                 := pthread dl rt
EXTERN.OPENALLIB.linux.LINK_FLAGS           := -pthread
EXTERN.OPENALLIB.beagleboard.LINK_FLAGS     := -rdynamic
EXTERN.OPENALLIB.meego.LINK_FLAGS           := -rdynamic
EXTERN.OPENALLIB.android.LIBS               := log
EXTERN.OPENALLIB.win32.LIBS                 := winmm user32 shell32 ole32 ksguid

#Цель №2 - OPENAL_TESTS
EXTERN.OPENAL_TESTS.TYPE         := test-suite
EXTERN.OPENAL_TESTS.SOURCE_DIRS  := tests
EXTERN.OPENAL_TESTS.INCLUDE_DIRS := include sources sources/openal32
EXTERN.OPENAL_TESTS.IMPORTS      := link.extern.openal run.extern.openal
