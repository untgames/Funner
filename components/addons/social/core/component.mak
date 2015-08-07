###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := SOCIAL.CORE.SOURCES SOCIAL.CORE.TESTS SOCIAL.CORE.INFO

#Target - sources
SOCIAL.CORE.SOURCES.TYPE        := static-lib
SOCIAL.CORE.SOURCES.NAME        := funner.social.core
SOCIAL.CORE.SOURCES.SOURCE_DIRS := sources
SOCIAL.CORE.SOURCES.IMPORTS			:= compile.social.core compile.common

#Target - tests
SOCIAL.CORE.TESTS.TYPE        := test-suite
SOCIAL.CORE.TESTS.SOURCE_DIRS := tests
SOCIAL.CORE.TESTS.IMPORTS			:= compile.social.core compile.common link.social.core

#Target - build documentation
SOCIAL.CORE.INFO.TYPE        := doxygen-info
SOCIAL.CORE.INFO.CHM_NAME    := funner.social.core
SOCIAL.CORE.INFO.SOURCE_DIRS := include
SOCIAL.CORE.INFO.IMPORTS     := compile.social.core
