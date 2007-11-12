###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := OPENALDEVICE_COMPONENTS

#Цель №1 - OPENALDEVICE sources
OPENALDEVICE_COMPONENTS.TYPE             := static-lib
OPENALDEVICE_COMPONENTS.NAME             := openaldevice
OPENALDEVICE_COMPONENTS.INCLUDE_DIRS     := include ../manager/include ../../commonlib/include ../../mathlib/include ../../xtl/include ../../medialib/include ../../../extern/openalsdk/include ../../systemlib/include
OPENALDEVICE_COMPONENTS.SOURCE_DIRS      := sources
OPENALDEVICE_COMPONENTS.LIB_DIRS         :=  
OPENALDEVICE_COMPONENTS.LIBS             := 
OPENALDEVICE_COMPONENTS.COMPILER_CFLAGS  :=
OPENALDEVICE_COMPONENTS.COMPILER_DEFINES := 
