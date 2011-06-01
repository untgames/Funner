###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := RENDER.DEBUG_RENDER.SOURCES RENDER.DEBUG_RENDER.TESTS RENDER.DEBUG_RENDER.INFO

#Цель - debug render system sources
RENDER.DEBUG_RENDER.SOURCES.TYPE        := static-lib
RENDER.DEBUG_RENDER.SOURCES.NAME        := funner.render.debug_render
RENDER.DEBUG_RENDER.SOURCES.SOURCE_DIRS := sources
RENDER.DEBUG_RENDER.SOURCES.IMPORTS     := compile.render.debug_render compile.xtl

#Цель - debug render system tests
RENDER.DEBUG_RENDER.TESTS.TYPE        := test-suite
RENDER.DEBUG_RENDER.TESTS.SOURCE_DIRS := tests
RENDER.DEBUG_RENDER.TESTS.IMPORTS     := compile.render.debug_render link.render.debug_render

#Цель - сборка документации
RENDER.DEBUG_RENDER.INFO.TYPE        := doxygen-info
RENDER.DEBUG_RENDER.INFO.CHM_NAME    := funner.render.debug_render
RENDER.DEBUG_RENDER.INFO.SOURCE_DIRS := include
RENDER.DEBUG_RENDER.INFO.IMPORTS     := compile.render.debug_render
