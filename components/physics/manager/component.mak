###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := PHYSICS.MANAGER.SOURCES PHYSICS.MANAGER.TESTS PHYSICS.MANAGER.INFO

#Цель Physics system sources
PHYSICS.MANAGER.SOURCES.TYPE        := static-lib
PHYSICS.MANAGER.SOURCES.NAME        := funner.physics.manager
PHYSICS.MANAGER.SOURCES.SOURCE_DIRS := sources
PHYSICS.MANAGER.SOURCES.IMPORTS     := compile.physics.manager compile.common

#Цель Physics system tests
PHYSICS.MANAGER.TESTS.TYPE        := test-suite
PHYSICS.MANAGER.TESTS.SOURCE_DIRS := tests
PHYSICS.MANAGER.TESTS.IMPORTS     := compile.physics.manager link.physics.manager

#Цель - сборка документации
PHYSICS.MANAGER.INFO.TYPE        := doxygen-info
PHYSICS.MANAGER.INFO.CHM_NAME    := funner.physics.manager
PHYSICS.MANAGER.INFO.SOURCE_DIRS := include
PHYSICS.MANAGER.INFO.IMPORTS     := compile.physics.manager
