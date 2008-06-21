###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := STL_TESTS TR1_TYPE_TRAITS_TESTS TR1_TESTS XTL_TESTS MATH_TESTS

#Цель №1 - STL tests
STL_TESTS.TYPE             := test-suite
STL_TESTS.INCLUDE_DIRS     := include
STL_TESTS.SOURCE_DIRS      := tests/stl
STL_TESTS.LIB_DIRS         :=  
STL_TESTS.EXECTUTION_DIR   :=
STL_TESTS.LIBS             :=
STL_TESTS.COMPILER_CFLAGS  :=
STL_TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__

#Цель №2 - TR1 type traits tests
TR1_TYPE_TRAITS_TESTS.TYPE                := test-suite
TR1_TYPE_TRAITS_TESTS.INCLUDE_DIRS        := include
TR1_TYPE_TRAITS_TESTS.SOURCE_DIRS         := tests/tr1/type_traits
TR1_TYPE_TRAITS_TESTS.LIB_DIRS            :=
TR1_TYPE_TRAITS_TESTS.EXECTUTION_DIR      :=
TR1_TYPE_TRAITS_TESTS.LIBS                :=
TR1_TYPE_TRAITS_TESTS.COMPILER_CFLAGS     :=
TR1_TYPE_TRAITS_TESTS.COMPILER_DEFINES    := __MYSTL_STANDALONE__
TR1_TYPE_TRAITS_TESTS.cygwin.g++.COMPILER_CFLAGS := --no-warn

#Цель №3 - TR1 tests
TR1_TESTS.TYPE                := test-suite
TR1_TESTS.INCLUDE_DIRS        := include
TR1_TESTS.SOURCE_DIRS         := tests/tr1/smart_ptr tests/tr1/functional tests/tr1/tuple tests/tr1/array
TR1_TESTS.LIB_DIRS            :=  
TR1_TESTS.EXECTUTION_DIR      :=
TR1_TESTS.LIBS                :=
TR1_TESTS.COMPILER_CFLAGS     :=
TR1_TESTS.COMPILER_DEFINES    := __MYSTL_STANDALONE__
TR1_TESTS.msvc.SOURCE_DIRS    := tests/tr1/functional/msvc

#Цель №4 - XTL tests
XTL_TESTS.TYPE             := test-suite
XTL_TESTS.INCLUDE_DIRS     := include
XTL_TESTS.SOURCE_DIRS      := tests/xtl/signals tests/xtl/utils tests/xtl/visitor tests/xtl/range tests/xtl/smart_ptr tests/xtl/any tests/xtl/exception
XTL_TESTS.LIB_DIRS         :=  
XTL_TESTS.EXECTUTION_DIR   :=
XTL_TESTS.LIBS             :=
XTL_TESTS.COMPILER_CFLAGS  :=
XTL_TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__

#Цель №5 - Math tests
MATH_TESTS.TYPE             := test-suite
MATH_TESTS.INCLUDE_DIRS     := include
MATH_TESTS.SOURCE_DIRS      := tests/math
MATH_TESTS.LIB_DIRS         :=  
MATH_TESTS.EXECTUTION_DIR   :=
MATH_TESTS.LIBS             :=
MATH_TESTS.COMPILER_CFLAGS  :=
MATH_TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__
