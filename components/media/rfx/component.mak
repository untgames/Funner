###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.RFX.SOURCES MEDIA.RFX.TESTS

#Цель №1 - sources
MEDIA.RFX.SOURCES.TYPE             := static-lib
MEDIA.RFX.SOURCES.NAME             := media.rfx
MEDIA.RFX.SOURCES.INCLUDE_DIRS     :=
MEDIA.RFX.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.RFX.SOURCES.LIB_DIRS         :=  
MEDIA.RFX.SOURCES.LIBS             := 
MEDIA.RFX.SOURCES.COMPILER_CFLAGS  :=
MEDIA.RFX.SOURCES.COMPILER_DEFINES :=
MEDIA.RFX.SOURCES.IMPORTS          := compile.static.mak ../shared/compile.static.mak

#Цель №3 - tests
MEDIA.RFX.TESTS.TYPE             := test-suite
MEDIA.RFX.TESTS.INCLUDE_DIRS     :=
MEDIA.RFX.TESTS.SOURCE_DIRS      := tests/core
MEDIA.RFX.TESTS.LIB_DIRS         :=  
MEDIA.RFX.TESTS.LIBS             :=
MEDIA.RFX.TESTS.COMPILER_CFLAGS  :=
MEDIA.RFX.TESTS.COMPILER_DEFINES :=
MEDIA.RFX.TESTS.IMPORTS          := link.static.mak compile.static.mak
