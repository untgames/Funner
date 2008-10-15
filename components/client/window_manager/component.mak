###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := CLIENT.WINDOW_MANAGER.SOURCES CLIENT.WINDOW_MANAGER.TESTS

#Цель - Lib sources
CLIENT.WINDOW_MANAGER.SOURCES.TYPE             := static-lib
CLIENT.WINDOW_MANAGER.SOURCES.NAME             := funner.client.window_manager
CLIENT.WINDOW_MANAGER.SOURCES.INCLUDE_DIRS     := 
CLIENT.WINDOW_MANAGER.SOURCES.SOURCE_DIRS      := sources
CLIENT.WINDOW_MANAGER.SOURCES.LIB_DIRS         :=  
CLIENT.WINDOW_MANAGER.SOURCES.LIBS             := 
CLIENT.WINDOW_MANAGER.SOURCES.IMPORTS          := compile.static.mak

#Цель - Lib tests
CLIENT.WINDOW_MANAGER.TESTS.TYPE             := test-suite
CLIENT.WINDOW_MANAGER.TESTS.INCLUDE_DIRS     :=
CLIENT.WINDOW_MANAGER.TESTS.SOURCE_DIRS      := tests
CLIENT.WINDOW_MANAGER.TESTS.EXECUTION_DIR    :=
CLIENT.WINDOW_MANAGER.TESTS.LIB_DIRS         :=  
CLIENT.WINDOW_MANAGER.TESTS.LIBS             :=
CLIENT.WINDOW_MANAGER.TESTS.LINK_INCLUDES    :=
CLIENT.WINDOW_MANAGER.TESTS.COMPILER_CFLAGS  :=
CLIENT.WINDOW_MANAGER.TESTS.COMPILER_DEFINES :=
CLIENT.WINDOW_MANAGER.TESTS.IMPORTS          := compile.static.mak link.static.mak ../../common/configurator.link.static.mak
