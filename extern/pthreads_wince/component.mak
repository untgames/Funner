###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := EXTERN.PTHREADS_WINCE.SOURCES EXTERN.PTHREADS_WINCE.TESTS

#Target - PTHREADS_WINCE.SOURCES
EXTERN.PTHREADS_WINCE.SOURCES.TYPE                  := static-lib
EXTERN.PTHREADS_WINCE.SOURCES.NAME                  := funner.extern.pthreads_wince
EXTERN.PTHREADS_WINCE.SOURCES.INCLUDE_DIRS          :=
EXTERN.PTHREADS_WINCE.SOURCES.SOURCE_DIRS           := sources
EXTERN.PTHREADS_WINCE.SOURCES.LIB_DIRS              :=
EXTERN.PTHREADS_WINCE.SOURCES.LIBS                  :=
EXTERN.PTHREADS_WINCE.SOURCES.COMPILER_DEFINES      :=
EXTERN.PTHREADS_WINCE.SOURCES.COMPILER_CFLAGS       := -wd4090 -wd4101
EXTERN.PTHREADS_WINCE.SOURCES.IMPORTS               := compile.extern.pthreads_wince

#Target - PTHREADS_WINCE.TESTS
EXTERN.PTHREADS_WINCE.TESTS.TYPE            := test-suite
EXTERN.PTHREADS_WINCE.TESTS.SOURCE_DIRS     := tests
EXTERN.PTHREADS_WINCE.TESTS.LIBS            :=
EXTERN.PTHREADS_WINCE.TESTS.IMPORTS         := compile.extern.pthreads_wince link.extern.pthreads_wince
EXTERN.PTHREADS_WINCE.TESTS.COMPILER_CFLAGS := -wd4013