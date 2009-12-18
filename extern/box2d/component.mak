###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.BOX2D

#Цель - sources
EXTERN.BOX2D.TYPE                    := static-lib
EXTERN.BOX2D.NAME                    := funner.extern.box2d
EXTERN.BOX2D.SOURCE_DIRS             := Source/Common Source/Collision Source/Collision/Shapes Source/Dynamics \
                                        Source/Dynamics/Contacts Source/Dynamics/Joints
EXTERN.BOX2D.IMPORTS                 := compile.extern.box2d
EXTERN.BOX2D.g++.COMPILER_CFLAGS     := -Wno-unused -Wno-switch
EXTERN.BOX2D.mingw.COMPILER_CFLAGS   := -include memory.h
EXTERN.BOX2D.linux.COMPILER_CFLAGS   := -include cstring
EXTERN.BOX2D.iphone.INCLUDE_FILES    := iphone-build.h
