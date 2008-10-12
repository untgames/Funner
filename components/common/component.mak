###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := COMMON.SOURCES COMMON.ZIP_FILE_SYSTEM COMMON.CONFIGURATOR COMMON.TESTS

#Цель - CommonLib sources
COMMON.SOURCES.TYPE             := static-lib
COMMON.SOURCES.NAME             := common
COMMON.SOURCES.INCLUDE_DIRS     := include ../xtl/include ../../extern/pcre/include sources
COMMON.SOURCES.SOURCE_DIRS      := sources/file_system/core sources/streams sources/hash sources/memory \
                                   sources/parser sources/strlib sources/utils sources/log sources/var_registry \
                                   sources/platform/default
COMMON.SOURCES.LIB_DIRS         :=  
COMMON.SOURCES.LIBS             := 
COMMON.SOURCES.COMPILER_CFLAGS  :=
COMMON.SOURCES.COMPILER_DEFINES := PCRE_STATIC
COMMON.SOURCES.unistd.SOURCE_DIRS  := sources/platform/unistd

#Цель - CommonLib zip file system sources
COMMON.ZIP_FILE_SYSTEM.TYPE                    := static-lib
COMMON.ZIP_FILE_SYSTEM.NAME                    := common.zip_file_system
COMMON.ZIP_FILE_SYSTEM.INCLUDE_DIRS            := include ../xtl/include ../../extern/zzip/include
COMMON.ZIP_FILE_SYSTEM.SOURCE_DIRS             := sources/file_system/zip
COMMON.ZIP_FILE_SYSTEM.LIB_DIRS                :=
COMMON.ZIP_FILE_SYSTEM.LIBS                    :=
COMMON.ZIP_FILE_SYSTEM.COMPILER_CFLAGS         :=
COMMON.ZIP_FILE_SYSTEM.COMPILER_DEFINES        :=
COMMON.ZIP_FILE_SYSTEM.macosx.COMPILER_DEFINES := ZZIP_1_H

#Цель - CommonLib zip file system sources
COMMON.CONFIGURATOR.TYPE             := static-lib
COMMON.CONFIGURATOR.NAME             := common.configurator
COMMON.CONFIGURATOR.INCLUDE_DIRS     := 
COMMON.CONFIGURATOR.SOURCE_DIRS      := sources/var_registry/configurator
COMMON.CONFIGURATOR.LIB_DIRS         :=
COMMON.CONFIGURATOR.LIBS             :=
COMMON.CONFIGURATOR.COMPILER_CFLAGS  :=
COMMON.CONFIGURATOR.COMPILER_DEFINES :=
COMMON.CONFIGURATOR.IMPORTS          := compile.static.mak

#Цель - CommonLib tests
COMMON.TESTS.TYPE             := test-suite
COMMON.TESTS.INCLUDE_DIRS     :=
COMMON.TESTS.SOURCE_DIRS      := tests/file_system tests/streams tests/hash tests/strlib tests/utils \
                                 tests/parser tests/memory tests/var_registry tests/log
COMMON.TESTS.EXECUTION_DIR    :=
COMMON.TESTS.LIB_DIRS         :=  
COMMON.TESTS.LIBS             :=
COMMON.TESTS.LINK_INCLUDES    :=
COMMON.TESTS.COMPILER_CFLAGS  :=
COMMON.TESTS.COMPILER_DEFINES :=
COMMON.TESTS.IMPORTS          := compile.static.mak link.static.mak zip_file_system.link.static.mak configurator.link.static.mak
