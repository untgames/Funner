###################################################################################################
#Список целей
###################################################################################################
TARGETS.linux := EXTERN.OPENALLIB EXTERN.OPENAL_TESTS

#Цель №1 - OPENALLIB
EXTERN.OPENALLIB.TYPE                 := dynamic-lib
EXTERN.OPENALLIB.NAME                 := funner.extern.openal32
EXTERN.OPENALLIB.INCLUDE_DIRS         := include sources sources/openal32
EXTERN.OPENALLIB.SOURCE_DIRS          := sources/openal32 sources/alc
EXTERN.OPENALLIB.LIB_DIRS             :=
EXTERN.OPENALLIB.LIBS                 := 
EXTERN.OPENALLIB.linux.COMPILER_DEFINES := _GNU_SOURCE=1 AL_ALEXT_PROTOTYPES
EXTERN.OPENALLIB.linux.COMPILER_FLAGS := -pthread

#Цель №2 - OPENAL_TESTS
EXTERN.OPENAL_TESTS.TYPE         := test-suite
EXTERN.OPENAL_TESTS.SOURCE_DIRS  := tests
EXTERN.OPENAL_TESTS.INCLUDE_DIRS := include sources sources/openal32
EXTERN.OPENAL_TESTS.IMPORTS      := link.extern.openal