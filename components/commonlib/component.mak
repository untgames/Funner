###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := COMMONLIB_EXTERN_LIBS COMMONLIB_COMPONENTS

#Цель №1 - CommonLib external libraries
COMMONLIB_EXTERN_LIBS.TYPE       := package
COMMONLIB_EXTERN_LIBS.COMPONENTS := extern/zlib extern/zzip extern/pcre

#Цель №1 - CommonLib sources
COMMONLIB_COMPONENTS.TYPE             := static-lib
COMMONLIB_COMPONENTS.NAME             := commnolib
COMMONLIB_COMPONENTS.INCLUDE_DIRS     := include ../xtl/include ../mathlib/include extern/zzip/include extern/pcre/include sources
COMMONLIB_COMPONENTS.SOURCE_DIRS      := sources/filesys sources/hash sources/memory sources/parser sources/strlib sources/utils
COMMONLIB_COMPONENTS.LIB_DIRS         :=  
COMMONLIB_COMPONENTS.LIBS             := 
COMMONLIB_COMPONENTS.COMPILER_CFLAGS  :=
COMMONLIB_COMPONENTS.COMPILER_DEFINES :=
