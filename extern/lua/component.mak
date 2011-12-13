###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.LUALIB

ifeq (,$(filter wince,$(PROFILES)))
TARGETS += EXTERN.LUA_UTILS EXTERN.LUA_TESTS
endif

#Цель №1 - LUALIB
EXTERN.LUALIB.TYPE                 := static-lib        #Тип цели
EXTERN.LUALIB.NAME                 := funner.extern.lua        #Имя выходного файла
EXTERN.LUALIB.INCLUDE_DIRS         := include				   #Относительные пути к каталогам c заголовочными файлами
EXTERN.LUALIB.SOURCE_DIRS          := sources           #Относительные пути к каталогам c исходными файлами
EXTERN.LUALIB.LIB_DIRS             :=                   #Относительные пути к каталогам с библиотеками
EXTERN.LUALIB.LIBS                 :=                   #Список используемых при линковке библиотек
EXTERN.LUALIB.msvc.COMPILER_CFLAGS := -TP               #Флаги сборки (зависят от конфигурации)
EXTERN.LUALIB.g++.COMPILER_CFLAGS  := -x c++
EXTERN.LUALIB.android.COMPILER_CFLAGS := -include android_lconv_wrapper.h
EXTERN.LUALIB.wince.COMPILER_CFLAGS   := -FIwince_wrapper.h -DLUA_ANSI

#Цель №2 - LUA_UTILS
EXTERN.LUA_UTILS.TYPE                 := test-suite     	 #Тип цели
EXTERN.LUA_UTILS.INCLUDE_DIRS         := include	sources  #Относительные пути к каталогам c заголовочными файлами
EXTERN.LUA_UTILS.SOURCE_DIRS          := utils          	 #Относительные пути к каталогам c исходными файлами
EXTERN.LUA_UTILS.LIB_DIRS             :=                  #Относительные пути к каталогам с библиотеками
EXTERN.LUA_UTILS.LIBS                 := funner.extern.lua       #Список используемых при линковке библиотек
EXTERN.LUA_UTILS.TARGET_DIR            = $(DIST_BIN_DIR)
EXTERN.LUA_UTILS.msvc.COMPILER_CFLAGS := -TP              #Флаги сборки (зависят от конфигурации)
EXTERN.LUA_UTILS.g++.COMPILER_CFLAGS  := -x c++
EXTERN.LUA_UTILS.bada_simulator.IMPORTS := link.extern.bada_addons

#Цель №3 - LUA_TESTS
EXTERN.LUA_TESTS.TYPE              := test-suite
EXTERN.LUA_TESTS.SOURCE_DIRS       := tests
EXTERN.LUA_TESTS.USED_APPLICATIONS := lua