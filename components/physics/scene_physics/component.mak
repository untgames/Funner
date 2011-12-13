###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := PHYSICS.SCENE_PHYSICS.SOURCES PHYSICS.SCENE_PHYSICS.TESTS PHYSICS.SCENE_PHYSICS.INFO

#Цель Physics system sources
PHYSICS.SCENE_PHYSICS.SOURCES.TYPE        := static-lib
PHYSICS.SCENE_PHYSICS.SOURCES.NAME        := funner.physics.scene_physics
PHYSICS.SCENE_PHYSICS.SOURCES.SOURCE_DIRS := sources
PHYSICS.SCENE_PHYSICS.SOURCES.IMPORTS     := compile.physics.scene_physics compile.physics.manager

#Цель Physics system tests
PHYSICS.SCENE_PHYSICS.TESTS.TYPE        := test-suite
PHYSICS.SCENE_PHYSICS.TESTS.SOURCE_DIRS := tests
PHYSICS.SCENE_PHYSICS.TESTS.IMPORTS     := compile.physics.scene_physics link.physics.scene_physics link.physics.low_level.bullet_driver

#Цель - сборка документации
PHYSICS.SCENE_PHYSICS.INFO.TYPE        := doxygen-info
PHYSICS.SCENE_PHYSICS.INFO.CHM_NAME    := funner.physics.scene_physics
PHYSICS.SCENE_PHYSICS.INFO.SOURCE_DIRS := include
PHYSICS.SCENE_PHYSICS.INFO.IMPORTS     := compile.physics.scene_physics
