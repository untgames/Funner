###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.LUALIB EXTERN.LUA_UTILS

#Цель №1 - LUALIB
EXTERN.LUALIB.TYPE                 := static-lib        #Тип цели
EXTERN.LUALIB.NAME                 := funner.extern.lua        #Имя выходного файла
EXTERN.LUALIB.INCLUDE_DIRS         := include				   #Относительные пути к каталогам c заголовочными файлами
EXTERN.LUALIB.SOURCE_DIRS          := sources           #Относительные пути к каталогам c исходными файлами
EXTERN.LUALIB.LIB_DIRS             :=                   #Относительные пути к каталогам с библиотеками
EXTERN.LUALIB.LIBS                 :=                   #Список используемых при линковке библиотек
EXTERN.LUALIB.COMPILER_DEFINES     :=  								 #Определения сборки
EXTERN.LUALIB.msvc.COMPILER_CFLAGS := -TP               #Флаги сборки (зависят от конфигурации)
EXTERN.LUALIB.g++.COMPILER_CFLAGS  := -x c++

#Цель №2 - LUA_UTILS
EXTERN.LUA_UTILS.TYPE                 := test-suite     	 #Тип цели
EXTERN.LUA_UTILS.INCLUDE_DIRS         := include	sources  #Относительные пути к каталогам c заголовочными файлами
EXTERN.LUA_UTILS.SOURCE_DIRS          := utils          	 #Относительные пути к каталогам c исходными файлами
EXTERN.LUA_UTILS.LIB_DIRS             :=                  #Относительные пути к каталогам с библиотеками
EXTERN.LUA_UTILS.LIBS                 := funner.extern.lua       #Список используемых при линковке библиотек
EXTERN.LUA_UTILS.COMPILER_DEFINES     :=  							   #Определения сборки
EXTERN.LUA_UTILS.msvc.COMPILER_CFLAGS := -TP              #Флаги сборки (зависят от конфигурации)
EXTERN.LUA_UTILS.g++.COMPILER_CFLAGS  := -x c++
