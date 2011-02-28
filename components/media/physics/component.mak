###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.PHYSICS.SOURCES MEDIA.PHYSICS.TESTS MEDIA.PHYSICS.INFO

#Цель - Sound sources
MEDIA.PHYSICS.SOURCES.TYPE        := static-lib
MEDIA.PHYSICS.SOURCES.NAME        := funner.media.physics
MEDIA.PHYSICS.SOURCES.SOURCE_DIRS := sources/core
MEDIA.PHYSICS.SOURCES.IMPORTS     := compile.media.physics

#Цель - Sound tests
MEDIA.PHYSICS.TESTS.TYPE        := test-suite
MEDIA.PHYSICS.TESTS.SOURCE_DIRS := tests
MEDIA.PHYSICS.TESTS.IMPORTS     := compile.media.physics link.media.physics

#Цель - сборка документации
MEDIA.PHYSICS.INFO.TYPE        := doxygen-info
MEDIA.PHYSICS.INFO.CHM_NAME    := funner.media.physics
MEDIA.PHYSICS.INFO.SOURCE_DIRS := include
MEDIA.PHYSICS.INFO.IMPORTS     := compile.media.physics
