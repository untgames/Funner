###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := DEVILU_COMPONENTS

#Цель №1 - DevILU sources
DEVILU_COMPONENTS.TYPE             := static-lib
DEVILU_COMPONENTS.NAME             := devilu
DEVILU_COMPONENTS.INCLUDE_DIRS     := include include/IL ../devil/include
DEVILU_COMPONENTS.SOURCE_DIRS      := sources
DEVILU_COMPONENTS.LIB_DIRS         :=  
DEVILU_COMPONENTS.LIBS             := 
DEVILU_COMPONENTS.COMPILER_CFLAGS  :=
DEVILU_COMPONENTS.COMPILER_DEFINES :=
