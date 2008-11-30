###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := EXTERN.BOX2D

#Цель - sources
EXTERN.BOX2D.TYPE             := static-lib
EXTERN.BOX2D.NAME             := funner.extern.box2d
EXTERN.BOX2D.INCLUDE_DIRS     :=
EXTERN.BOX2D.SOURCE_DIRS      := sources/Common sources/Collision sources/Collision/Shapes sources/Dynamics \
                                 sources/Dynamics/Contacts sources/Dynamics/Joints
EXTERN.BOX2D.COMPILER_CFLAGS  :=
EXTERN.BOX2D.IMPORTS          := compile.static.mak
EXTERN.BOX2D.g++.COMPILER_CFLAGS := -Wno-unused -Wno-switch