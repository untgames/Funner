###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.MONO_LIB
TARGETS += EXTERN.MONO_CS_INVOKE EXTERN.MONO_TESTS

#Цель №1 - MONOLIB
EXTERN.MONO_LIB.TYPE                 := static-lib               #Тип цели
EXTERN.MONO_LIB.NAME                 := funner.extern.mono       #Имя выходного файла
EXTERN.MONO_LIB.INCLUDE_DIRS         := sources                  #Относительные пути к каталогам c заголовочными файлами
EXTERN.MONO_LIB.SOURCE_DIRS          := sources                  #Относительные пути к каталогам c исходными файлами
EXTERN.MONO_LIB.LIB_DIRS             :=                          #Относительные пути к каталогам с библиотеками
EXTERN.MONO_LIB.LIBS                 :=                          #Список используемых при линковке библиотек
#EXTERN.MONOLIB.msvc.COMPILER_CFLAGS := -TP                      #Флаги сборки (зависят от конфигурации)
#EXTERN.MONOLIB.g++.COMPILER_CFLAGS  := -x c++
#EXTERN.MONOLIB.android.COMPILER_CFLAGS := -include android_lconv_wrapper.h
#EXTERN.MONOLIB.wince.COMPILER_CFLAGS   := -FIwince_wrapper.h -DLUA_ANSI

#Цель №2 - MONO_CS_INVOKE
EXTERN.MONO_CS_INVOKE.TYPE              := cs-dynamic-lib
EXTERN.MONO_CS_INVOKE.NAME              := funner.extern.mono-test-invoke
EXTERN.MONO_CS_INVOKE.SOURCE_DIRS       := tests/invoke_assembly

#Цель №3 - MONO_TESTS
EXTERN.MONO_TESTS.TYPE              := test-suite
EXTERN.MONO_TESTS.SOURCE_DIRS       := tests
EXTERN.MONO_TESTS.USED_APPLICATIONS := mono
EXTERN.MONO_TESTS.INCLUDE_DIRS      := sources
EXTERN.MONO_TESTS.LIBS              := funner.extern.mono