###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := STL_TESTS TR1_TESTS XTL_TESTS

#Цель №1 - STL tests
STL_TESTS.TYPE             := test-suite
STL_TESTS.INCLUDE_DIRS     := include
STL_TESTS.SOURCE_DIRS      := tests/stl
STL_TESTS.LIB_DIRS         :=  
STL_TESTS.EXECTUTION_DIR   :=
STL_TESTS.LIBS             :=
STL_TESTS.COMPILER_CFLAGS  :=
STL_TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__

#Цель №2 - XTL tests
TR1_TESTS.TYPE             := test-suite
TR1_TESTS.INCLUDE_DIRS     := include
TR1_TESTS.SOURCE_DIRS      := tests/tr1/type_traits tests/tr1/smart_ptr tests/tr1/functional tests/tr1/tuple
TR1_TESTS.LIB_DIRS         :=  
TR1_TESTS.EXECTUTION_DIR   :=
TR1_TESTS.LIBS             :=
TR1_TESTS.COMPILER_CFLAGS  :=
TR1_TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__

#Цель №3 - XTL tests
XTL_TESTS.TYPE             := test-suite
XTL_TESTS.INCLUDE_DIRS     := include ../mathlib/include
XTL_TESTS.SOURCE_DIRS      := tests/xtl/signals tests/xtl/io
XTL_TESTS.LIB_DIRS         :=  
XTL_TESTS.EXECTUTION_DIR   :=
XTL_TESTS.LIBS             :=
XTL_TESTS.COMPILER_CFLAGS  :=
XTL_TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__
