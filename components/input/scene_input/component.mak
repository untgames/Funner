###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := INPUT.SCENE_INPUT.SOURCES INPUT.SCENE_INPUT.TESTS INPUT.SCENE_INPUT.INFO

#Цель №1 - INPUT manager sources
INPUT.SCENE_INPUT.SOURCES.TYPE        := static-lib
INPUT.SCENE_INPUT.SOURCES.NAME        := funner.input.scene_input
INPUT.SCENE_INPUT.SOURCES.SOURCE_DIRS := sources
INPUT.SCENE_INPUT.SOURCES.IMPORTS     := 

#Цель №2 - INPUT manager tests
INPUT.SCENE_INPUT.TESTS.TYPE        := test-suite
INPUT.SCENE_INPUT.TESTS.SOURCE_DIRS := tests
INPUT.SCENE_INPUT.TESTS.IMPORTS     := 
                                   
#Цель - сборка документации
INPUT.SCENE_INPUT.INFO.TYPE        := doxygen-info
INPUT.SCENE_INPUT.INFO.CHM_NAME    := funner.input.scene_input
INPUT.SCENE_INPUT.INFO.SOURCE_DIRS := include
INPUT.SCENE_INPUT.INFO.IMPORTS     := compile.input.scene_input
