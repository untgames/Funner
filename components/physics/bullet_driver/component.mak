###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := PHYSICS.BULLET_DRIVER.SOURCES PHYSICS.BULLET_DRIVER.TESTS

#OpenGL render system sources
PHYSICS.BULLET_DRIVER.SOURCES.TYPE         := static-lib
PHYSICS.BULLET_DRIVER.SOURCES.NAME         := funner.physics.low_level.bullet_driver
PHYSICS.BULLET_DRIVER.SOURCES.INCLUDE_DIRS := sources
PHYSICS.BULLET_DRIVER.SOURCES.SOURCE_DIRS  := sources
PHYSICS.BULLET_DRIVER.SOURCES.IMPORTS      := compile.physics.low_level compile.common compile.extern.bullet

#OpenGL render system tests
PHYSICS.BULLET_DRIVER.TESTS.TYPE        := test-suite
PHYSICS.BULLET_DRIVER.TESTS.SOURCE_DIRS := tests
PHYSICS.BULLET_DRIVER.TESTS.IMPORTS     := link.physics.low_level.bullet_driver compile.physics.low_level
