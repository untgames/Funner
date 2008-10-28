###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := ENGINE.SOURCES ENGINE.TESTS

#Цель - CLIENTLib sources
ENGINE.SOURCES.TYPE             := static-lib
ENGINE.SOURCES.NAME             := funner.engine.core
ENGINE.SOURCES.INCLUDE_DIRS     := 
ENGINE.SOURCES.SOURCE_DIRS      := sources
ENGINE.SOURCES.LIB_DIRS         :=  
ENGINE.SOURCES.LIBS             := 
ENGINE.SOURCES.IMPORTS          := compile.static.mak

#Цель - CLIENTLib tests
ENGINE.TESTS.TYPE             := test-suite
ENGINE.TESTS.INCLUDE_DIRS     :=
ENGINE.TESTS.SOURCE_DIRS      := tests
ENGINE.TESTS.EXECUTION_DIR    :=
ENGINE.TESTS.LIB_DIRS         :=  
ENGINE.TESTS.LIBS             :=
ENGINE.TESTS.LINK_INCLUDES    :=
ENGINE.TESTS.COMPILER_CFLAGS  :=
ENGINE.TESTS.COMPILER_DEFINES :=
ENGINE.TESTS.IMPORTS          := compile.static.mak link.static.mak ../../common/xml.link.static.mak
