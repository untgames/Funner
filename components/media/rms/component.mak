###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.RMS.SOURCES MEDIA.RMS.TESTS

#Цель №1 - sources
MEDIA.RMS.SOURCES.TYPE             := static-lib
MEDIA.RMS.SOURCES.NAME             := funner.media.rms
MEDIA.RMS.SOURCES.INCLUDE_DIRS     :=
MEDIA.RMS.SOURCES.SOURCE_DIRS      := sources
MEDIA.RMS.SOURCES.LIB_DIRS         :=
MEDIA.RMS.SOURCES.LIBS             :=
MEDIA.RMS.SOURCES.COMPILER_CFLAGS  :=
MEDIA.RMS.SOURCES.COMPILER_DEFINES :=
MEDIA.RMS.SOURCES.IMPORTS          := compile.media.rms compile.common

#Цель №3 - tests
MEDIA.RMS.TESTS.TYPE             := test-suite
MEDIA.RMS.TESTS.INCLUDE_DIRS     :=
MEDIA.RMS.TESTS.SOURCE_DIRS      := tests
MEDIA.RMS.TESTS.LIB_DIRS         :=
MEDIA.RMS.TESTS.LIBS             :=
MEDIA.RMS.TESTS.COMPILER_CFLAGS  :=
MEDIA.RMS.TESTS.COMPILER_DEFINES :=
MEDIA.RMS.TESTS.IMPORTS          := compile.media.rms link.media.rms
