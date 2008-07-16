###################################################################################################
#Определения и константы
###################################################################################################
#TARGETS := XTL.STL.TESTS XTL.TR1.TESTS XTL.XTL.TESTS XTL.XTL.EXPORT XTL.MATH.TESTS
TARGETS := XTL.STL.TESTS XTL.TR1.TESTS XTL.XTL.TESTS XTL.XTL.EXPORT

#Цель №1 - STL tests
XTL.STL.TESTS.TYPE             := test-suite
XTL.STL.TESTS.INCLUDE_DIRS     := include
XTL.STL.TESTS.SOURCE_DIRS      := tests/stl
XTL.STL.TESTS.LIB_DIRS         :=  
XTL.STL.TESTS.EXECTUTION_DIR   :=
XTL.STL.TESTS.LIBS             :=
XTL.STL.TESTS.COMPILER_CFLAGS  :=
XTL.STL.TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__

#Цель №2 - TR1 tests
XTL.TR1.TESTS.TYPE                := test-suite
XTL.TR1.TESTS.INCLUDE_DIRS        := include
XTL.TR1.TESTS.SOURCE_DIRS         := tests/tr1/type_traits tests/tr1/smart_ptr tests/tr1/functional tests/tr1/tuple tests/tr1/array
XTL.TR1.TESTS.LIB_DIRS            :=  
XTL.TR1.TESTS.EXECTUTION_DIR      :=
XTL.TR1.TESTS.LIBS                :=
XTL.TR1.TESTS.COMPILER_CFLAGS     :=
XTL.TR1.TESTS.COMPILER_DEFINES    := __MYSTL_STANDALONE__
XTL.TR1.TESTS.msvc.SOURCE_DIRS    := tests/tr1/functional/msvc
XTL.TR1.TYPE_TRAITS_TESTS.cygwin.g++.COMPILER_CFLAGS := --no-warn

#Цель №3 - XTL tests
XTL.XTL.TESTS.TYPE             := test-suite
XTL.XTL.TESTS.INCLUDE_DIRS     := include
XTL.XTL.TESTS.SOURCE_DIRS      := tests/xtl/signals tests/xtl/utils tests/xtl/visitor tests/xtl/range tests/xtl/smart_ptr tests/xtl/any tests/xtl/exception
XTL.XTL.TESTS.LIB_DIRS         :=  
XTL.XTL.TESTS.EXECTUTION_DIR   :=
XTL.XTL.TESTS.LIBS             :=
XTL.XTL.TESTS.COMPILER_CFLAGS  :=
XTL.XTL.TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__

#Цель №4 - Math tests
XTL.MATH.TESTS.TYPE             := test-suite
XTL.MATH.TESTS.INCLUDE_DIRS     := include
XTL.MATH.TESTS.SOURCE_DIRS      := tests/math
XTL.MATH.TESTS.LIB_DIRS         :=  
XTL.MATH.TESTS.EXECTUTION_DIR   :=
XTL.MATH.TESTS.LIBS             :=
XTL.MATH.TESTS.COMPILER_CFLAGS  :=
XTL.MATH.TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__

#Цель №5 - XTL export
XTL.XTL.EXPORT.TYPE         := test-suite
XTL.XTL.EXPORT.EXPORT_DIRS  := include

