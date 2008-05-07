###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.GEOMETRY.SOURCES MEDIA.GEOMETRY.TESTS

#Цель №1 - sources
MEDIA.GEOMETRY.SOURCES.TYPE             := static-lib
MEDIA.GEOMETRY.SOURCES.NAME             := media.geometry
MEDIA.GEOMETRY.SOURCES.INCLUDE_DIRS     := include
MEDIA.GEOMETRY.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.GEOMETRY.SOURCES.LIB_DIRS         :=
MEDIA.GEOMETRY.SOURCES.LIBS             :=
MEDIA.GEOMETRY.SOURCES.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.SOURCES.COMPILER_DEFINES :=
MEDIA.GEOMETRY.SOURCES.IMPORTS          := ../../common/compile.static.mak

#Цель №2 - tests
MEDIA.GEOMETRY.TESTS.TYPE             := test-suite
MEDIA.GEOMETRY.TESTS.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.TESTS.SOURCE_DIRS      := tests
MEDIA.GEOMETRY.TESTS.LIB_DIRS         :=
MEDIA.GEOMETRY.TESTS.LIBS             :=
MEDIA.GEOMETRY.TESTS.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.TESTS.COMPILER_DEFINES :=
MEDIA.GEOMETRY.TESTS.IMPORTS          := compile.static.mak link.static.mak
