###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := COMMONLIB_SOURCES COMMONLIB_ZIP_FILE_SYSTEM COMMONLIB_TESTS

#Цель №1 - CommonLib sources
COMMONLIB_SOURCES.TYPE             := static-lib
COMMONLIB_SOURCES.NAME             := common
COMMONLIB_SOURCES.INCLUDE_DIRS     := include ../xtl/include ../../extern/pcre/include sources
COMMONLIB_SOURCES.SOURCE_DIRS      := sources/file_system/core sources/streams sources/hash sources/memory sources/parser sources/strlib sources/utils sources/platform/default
COMMONLIB_SOURCES.LIB_DIRS         :=  
COMMONLIB_SOURCES.LIBS             := 
COMMONLIB_SOURCES.COMPILER_CFLAGS  :=
COMMONLIB_SOURCES.COMPILER_DEFINES := PCRE_STATIC
COMMONLIB_SOURCES.g++.SOURCE_DIRS  := sources/platform/unistd
COMMONLIB_SOURCES.nds.g++.COMPILER_DEFINES := COMMONLIB_NDS

#Цель №2 - CommonLib zip file system sources
COMMONLIB_ZIP_FILE_SYSTEM.TYPE             := static-lib
COMMONLIB_ZIP_FILE_SYSTEM.NAME             := common.zip_file_system
COMMONLIB_ZIP_FILE_SYSTEM.INCLUDE_DIRS     := include ../xtl/include ../../extern/zzip/include
COMMONLIB_ZIP_FILE_SYSTEM.SOURCE_DIRS      := sources/file_system/zip
COMMONLIB_ZIP_FILE_SYSTEM.LIB_DIRS         :=
COMMONLIB_ZIP_FILE_SYSTEM.LIBS             :=
COMMONLIB_ZIP_FILE_SYSTEM.COMPILER_CFLAGS  :=
COMMONLIB_ZIP_FILE_SYSTEM.COMPILER_DEFINES :=

#Цель №3 - CommonLib tests
COMMONLIB_TESTS.TYPE             := test-suite
COMMONLIB_TESTS.INCLUDE_DIRS     :=
COMMONLIB_TESTS.SOURCE_DIRS      := tests/file_system tests/streams tests/hash tests/strlib tests/utils tests/parser tests/memory
COMMONLIB_TESTS.EXECUTION_DIR    :=
COMMONLIB_TESTS.LIB_DIRS         :=  
COMMONLIB_TESTS.LIBS             :=
COMMONLIB_TESTS.LINK_INCLUDES    :=
COMMONLIB_TESTS.COMPILER_CFLAGS  :=
COMMONLIB_TESTS.COMPILER_DEFINES :=
COMMONLIB_TESTS.IMPORTS          := compile.static.mak link.static.mak zip_file_system.link.static.mak
