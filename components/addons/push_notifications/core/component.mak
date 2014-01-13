###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := PUSH_NOTIFICATIONS.CORE.SOURCES PUSH_NOTIFICATIONS.CORE.TESTS PUSH_NOTIFICATIONS.CORE.INFO

#Цель - sources
PUSH_NOTIFICATIONS.CORE.SOURCES.TYPE        := static-lib
PUSH_NOTIFICATIONS.CORE.SOURCES.NAME        := funner.push_notifications.core
PUSH_NOTIFICATIONS.CORE.SOURCES.SOURCE_DIRS := sources
PUSH_NOTIFICATIONS.CORE.SOURCES.IMPORTS     := compile.push_notifications.core compile.common

#Цель - tests
PUSH_NOTIFICATIONS.CORE.TESTS.TYPE        := test-suite
PUSH_NOTIFICATIONS.CORE.TESTS.SOURCE_DIRS := tests
PUSH_NOTIFICATIONS.CORE.TESTS.IMPORTS     := compile.push_notifications.core compile.common link.push_notifications.core

#Цель - сборка документации
PUSH_NOTIFICATIONS.CORE.INFO.TYPE        := doxygen-info
PUSH_NOTIFICATIONS.CORE.INFO.CHM_NAME    := funner.push_notifications.core
PUSH_NOTIFICATIONS.CORE.INFO.SOURCE_DIRS := include
PUSH_NOTIFICATIONS.CORE.INFO.IMPORTS     := compile.push_notifications.core
