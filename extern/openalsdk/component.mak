###################################################################################################
#Список целей
###################################################################################################
TARGETS.linux  := EXTERN.OPENALLIB EXTERN.OPENAL_TESTS
TARGETS.android := EXTERN.OPENALLIB EXTERN.OPENAL_TESTS

#Цель №1 - OPENALLIB
ifneq (,$(filter no_dll,$(PROFILES)))
EXTERN.OPENALLIB.TYPE                       := static-lib
else
EXTERN.OPENALLIB.TYPE                       := dynamic-lib
endif
EXTERN.OPENALLIB.NAME                       := funner.extern.openal32
EXTERN.OPENALLIB.INCLUDE_DIRS               := include sources sources/openal32
EXTERN.OPENALLIB.SOURCE_DIRS                := sources/openal32 sources/alc
EXTERN.OPENALLIB.LIB_DIRS                   :=
EXTERN.OPENALLIB.LIBS                       :=
EXTERN.OPENALLIB.COMPILER_DEFINES           := AL_ALEXT_PROTOTYPES _GNU_SOURCE=1 
EXTERN.OPENALLIB.linux.COMPILER_DEFINES     := 
EXTERN.OPENALLIB.linux.COMPILER_FLAGS       := -pthread
EXTERN.OPENALLIB.linux.LIBS                 := pthread dl rt
EXTERN.OPENALLIB.linux.LINK_FLAGS           := -pthread
EXTERN.OPENALLIB.beagleboard.LINK_FLAGS := -rdynamic

#Цель №2 - OPENAL_TESTS
EXTERN.OPENAL_TESTS.TYPE         := test-suite
EXTERN.OPENAL_TESTS.SOURCE_DIRS  := tests
EXTERN.OPENAL_TESTS.INCLUDE_DIRS := include sources sources/openal32
EXTERN.OPENAL_TESTS.IMPORTS      := link.extern.openal
