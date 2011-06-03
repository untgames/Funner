###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.MONOLIB

ifeq (,$(filter wince,$(PROFILES)))
TARGETS += EXTERN.MONO_TESTS
endif

#Цель №1 - MONOLIB
EXTERN.MONOLIB.TYPE                 := static-lib        				#Тип цели
EXTERN.MONOLIB.NAME                 := funner.extern.mono       #Имя выходного файла
EXTERN.MONOLIB.INCLUDE_DIRS         := include                  #Относительные пути к каталогам c заголовочными файлами
EXTERN.MONOLIB.SOURCE_DIRS          := sources           			  #Относительные пути к каталогам c исходными файлами
EXTERN.MONOLIB.LIB_DIRS             :=                          #Относительные пути к каталогам с библиотеками
EXTERN.MONOLIB.LIBS                 :=                   				#Список используемых при линковке библиотек
EXTERN.MONOLIB.msvc.COMPILER_CFLAGS := -TP               				#Флаги сборки (зависят от конфигурации)
EXTERN.MONOLIB.g++.COMPILER_CFLAGS  := -x c++
EXTERN.MONOLIB.android.COMPILER_CFLAGS := -include android_lconv_wrapper.h
EXTERN.MONOLIB.wince.COMPILER_CFLAGS   := -FIwince_wrapper.h -DLUA_ANSI

#Цель №2 - MONO_TESTS
EXTERN.MONO_TESTS.TYPE              := test-suite
EXTERN.MONO_TESTS.SOURCE_DIRS       := tests
EXTERN.MONO_TESTS.USED_APPLICATIONS := mono