TARGETS := EXTERN.CURL.SOURCES EXTERN.CURL.TESTS

EXTERN.CURL.SOURCES.TYPE                     := static-lib
EXTERN.CURL.SOURCES.NAME                     := funner.extern.curl
EXTERN.CURL.SOURCES.INCLUDE_DIRS             := include sources
EXTERN.CURL.SOURCES.SOURCE_DIRS              := sources sources/vtls
EXTERN.CURL.SOURCES.COMPILER_DEFINES         := CURL_STATICLIB BUILDING_LIBCURL $(if $(filter emscripten,$(PROFILES)),,USE_SSLEAY)
#EXTERN.CURL.SOURCES.iphone.COMPILER_CFLAGS   := -Wno-uninitialized
#EXTERN.CURL.SOURCES.iphone.COMPILER_DEFINES  := __IPHONE__
#EXTERN.CURL.SOURCES.clang.COMPILER_CFLAGS    += -w
EXTERN.CURL.SOURCES.IMPORTS                  := compile.extern.zlib compile.extern.openssl

EXTERN.CURL.TESTS.TYPE                     := test-suite
EXTERN.CURL.TESTS.SOURCE_DIRS              := tests
EXTERN.CURL.TESTS.INCLUDE_DIRS             := sources
EXTERN.CURL.TESTS.INCLUDE_FILES            := first.c testutil.c
EXTERN.CURL.TESTS.IMPORTS                  := compile.extern.curl link.extern.curl
EXTERN.CURL.TESTS.vcx86-64.COMPILER_CFLAGS := -wd4244
#EXTERN.CURL.TESTS.g++.COMPILER_CFLAGS      := --no-warn
