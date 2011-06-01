###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += MEDIA.PHYSICS.CORE.SOURCES 
TARGETS += MEDIA.PHYSICS.XPHYS.SOURCES 
TARGETS += MEDIA.PHYSICS.CORE.TESTS 
TARGETS += MEDIA.PHYSICS.XPHYS.TESTS
TARGETS += MEDIA.PHYSICS.INFO

#Цель - core sources
MEDIA.PHYSICS.CORE.SOURCES.TYPE        := static-lib
MEDIA.PHYSICS.CORE.SOURCES.NAME        := funner.media.physics
MEDIA.PHYSICS.CORE.SOURCES.SOURCE_DIRS := sources/core
MEDIA.PHYSICS.CORE.SOURCES.IMPORTS     := compile.media.physics compile.media.shared

#Цель - xphys sources
MEDIA.PHYSICS.XPHYS.SOURCES.TYPE        := static-lib
MEDIA.PHYSICS.XPHYS.SOURCES.NAME        := funner.media.physics.xphys
MEDIA.PHYSICS.XPHYS.SOURCES.SOURCE_DIRS := sources/xphys
MEDIA.PHYSICS.XPHYS.SOURCES.IMPORTS     := compile.media.physics

#Цель - core tests
MEDIA.PHYSICS.CORE.TESTS.TYPE        := test-suite
MEDIA.PHYSICS.CORE.TESTS.SOURCE_DIRS := tests/core
MEDIA.PHYSICS.CORE.TESTS.IMPORTS     := compile.media.physics link.media.physics

#Цель - xphys tests
MEDIA.PHYSICS.XPHYS.TESTS.TYPE        := test-suite
MEDIA.PHYSICS.XPHYS.TESTS.SOURCE_DIRS := tests/xphys
MEDIA.PHYSICS.XPHYS.TESTS.IMPORTS     := compile.media.physics link.media.physics.xphys

#Цель - сборка документации
MEDIA.PHYSICS.INFO.TYPE        := doxygen-info
MEDIA.PHYSICS.INFO.CHM_NAME    := funner.media.physics
MEDIA.PHYSICS.INFO.SOURCE_DIRS := include
MEDIA.PHYSICS.INFO.IMPORTS     := compile.media.physics
