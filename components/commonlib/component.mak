###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := COMMONLIB_COMPONENTS COMMONLIB_TESTS

#Цель №1 - CommonLib sources
COMMONLIB_COMPONENTS.TYPE             := static-lib
COMMONLIB_COMPONENTS.NAME             := common
COMMONLIB_COMPONENTS.INCLUDE_DIRS     := include ../xtl/include ../../extern/zzip/include ../../extern/pcre/include sources
COMMONLIB_COMPONENTS.SOURCE_DIRS      := sources/file_system sources/streams sources/hash sources/memory sources/parser sources/strlib sources/utils sources/platform/default
COMMONLIB_COMPONENTS.LIB_DIRS         :=  
COMMONLIB_COMPONENTS.LIBS             := 
COMMONLIB_COMPONENTS.COMPILER_CFLAGS  :=
COMMONLIB_COMPONENTS.COMPILER_DEFINES := PCRE_STATIC

#Цель №2 - CommonLib tests
COMMONLIB_TESTS.TYPE             := test-suite
COMMONLIB_TESTS.INCLUDE_DIRS     := include ../xtl/include
COMMONLIB_TESTS.SOURCE_DIRS      := tests/file_system tests/streams tests/hash tests/strlib tests/utils tests/parser tests/memory
COMMONLIB_TESTS.EXECUTION_DIR    :=
COMMONLIB_TESTS.LIB_DIRS         :=  
COMMONLIB_TESTS.LIBS             := common zlib zzip pcre
COMMONLIB_TESTS.COMPILER_CFLAGS  :=
COMMONLIB_TESTS.COMPILER_DEFINES :=
