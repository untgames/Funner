###################################################################################################
#Список целей
###################################################################################################
TARGETS := LUALIB LUA_UTILS

#Цель №1 - LUALIB
LUALIB.TYPE             := static-lib        #Тип цели
LUALIB.NAME             := extern.lua        #Имя выходного файла
LUALIB.INCLUDE_DIRS     := include				   #Относительные пути к каталогам c заголовочными файлами
LUALIB.SOURCE_DIRS      := sources           #Относительные пути к каталогам c исходными файлами
LUALIB.LIB_DIRS         :=                   #Относительные пути к каталогам с библиотеками
LUALIB.LIBS             :=                   #Список используемых при линковке библиотек
LUALIB.COMPILER_DEFINES :=  								 #Определения сборки
LUALIB.COMPILER_CFLAGS  := -TP               #Флаги сборки (зависят от конфигурации)

#Цель №2 - LUA_UTILS
LUA_UTILS.TYPE             := test-suite     	 #Тип цели
LUA_UTILS.INCLUDE_DIRS     := include	sources  #Относительные пути к каталогам c заголовочными файлами
LUA_UTILS.SOURCE_DIRS      := utils          	 #Относительные пути к каталогам c исходными файлами
LUA_UTILS.LIB_DIRS         :=                  #Относительные пути к каталогам с библиотеками
LUA_UTILS.LIBS             := extern.lua       #Список используемых при линковке библиотек
LUA_UTILS.COMPILER_DEFINES :=  							   #Определения сборки
LUA_UTILS.COMPILER_CFLAGS  := -TP              #Флаги сборки (зависят от конфигурации)
