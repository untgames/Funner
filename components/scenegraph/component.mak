###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := SG_COMPONENTS

#Цель №1 - SG_COMPONENTS sources
SG_COMPONENTS.TYPE             := static-lib
SG_COMPONENTS.NAME             := sg
SG_COMPONENTS.INCLUDE_DIRS     := include ../commonlib/include ../xtl/include ../mathlib/include
SG_COMPONENTS.SOURCE_DIRS      := sources/core
SG_COMPONENTS.LIB_DIRS         :=  
SG_COMPONENTS.LIBS             :=
SG_COMPONENTS.COMPILER_CFLAGS  :=
SG_COMPONENTS.COMPILER_DEFINES := 
