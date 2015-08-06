###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := EXTERN.WIN32_LOAD_DLL EXTERN.WIN32_LOAD_DLL.SAMPLE_DLL EXTERN.WIN32_LOAD_DLL.TESTS

#Target - Library
EXTERN.WIN32_LOAD_DLL.TYPE                     := static-lib
EXTERN.WIN32_LOAD_DLL.NAME                     := funner.extern.win32_load_dll
EXTERN.WIN32_LOAD_DLL.SOURCE_DIRS              := sources
EXTERN.WIN32_LOAD_DLL.vcx86-64.COMPILER_CFLAGS := -wd4244
EXTERN.WIN32_LOAD_DLL.IMPORTS                  := compile.extern.win32_load_dll

TEST_DLL_DIR  := ../../tmp/$(CURRENT_TOOLSET)/EXTERN.WIN32_LOAD_DLL.SAMPLE_DLL
TEST_DLL_PATH := "$(TEST_DLL_DIR)/sample-dll.dll"

#Target - test library
EXTERN.WIN32_LOAD_DLL.SAMPLE_DLL.TYPE        := dynamic-lib
EXTERN.WIN32_LOAD_DLL.SAMPLE_DLL.OUT_DIR     := $(TEST_DLL_DIR)
EXTERN.WIN32_LOAD_DLL.SAMPLE_DLL.NAME        := sample-dll
EXTERN.WIN32_LOAD_DLL.SAMPLE_DLL.SOURCE_DIRS := utils/sample_dll

#Target - tests
EXTERN.WIN32_LOAD_DLL.TESTS.TYPE             := test-suite
EXTERN.WIN32_LOAD_DLL.TESTS.SOURCE_DIRS      := tests
EXTERN.WIN32_LOAD_DLL.TESTS.EXECUTION_DIR    := .
EXTERN.WIN32_LOAD_DLL.TESTS.IMPORTS          := compile.extern.win32_load_dll link.extern.win32_load_dll
EXTERN.WIN32_LOAD_DLL.TESTS.COMPILER_DEFINES := DLL_PATH='$(subst /,\\,$(TEST_DLL_PATH))'
