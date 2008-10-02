###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := CLIENT.SOURCES CLIENT.TESTS

#Цель - CLIENTLib sources
CLIENT.SOURCES.TYPE             := static-lib
CLIENT.SOURCES.NAME             := client.core
CLIENT.SOURCES.INCLUDE_DIRS     := 
CLIENT.SOURCES.SOURCE_DIRS      := sources
CLIENT.SOURCES.LIB_DIRS         :=  
CLIENT.SOURCES.LIBS             := 
CLIENT.SOURCES.IMPORTS          := compile.static.mak ../../input/manager/compile.static.mak

#Цель - CLIENTLib tests
CLIENT.TESTS.TYPE             := test-suite
CLIENT.TESTS.INCLUDE_DIRS     :=
CLIENT.TESTS.SOURCE_DIRS      := tests
CLIENT.TESTS.EXECUTION_DIR    :=
CLIENT.TESTS.LIB_DIRS         :=  
CLIENT.TESTS.LIBS             :=
CLIENT.TESTS.LINK_INCLUDES    :=
CLIENT.TESTS.COMPILER_CFLAGS  :=
CLIENT.TESTS.COMPILER_DEFINES :=
CLIENT.TESTS.IMPORTS          := compile.static.mak link.static.mak ../../input/manager/xkeymap.link.static.mak
