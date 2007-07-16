###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := XTL_TESTS

#Цель №1 - XTL tests
XTL_TESTS.TYPE             := test-suite
XTL_TESTS.INCLUDE_DIRS     := include
XTL_TESTS.SOURCE_DIRS      := tests/stl tests/tr1/type_traits tests/tr1/smart_ptr tests/tr1/functional \
                              tests/tr1/tuple tests/xtl/signals
XTL_TESTS.LIB_DIRS         :=  
XTL_TESTS.EXECTUTION_DIR   :=
XTL_TESTS.LIBS             := commonlib zlib zzip pcre
XTL_TESTS.COMPILER_CFLAGS  :=
XTL_TESTS.COMPILER_DEFINES := __MYSTL_STANDALONE__
