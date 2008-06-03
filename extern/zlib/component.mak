###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.ZLIB

#Цель №1 - ZLIB
EXTERN.ZLIB.TYPE             := static-lib  #Тип цели
EXTERN.ZLIB.NAME             := extern.zlib #Имя выходного файла
EXTERN.ZLIB.INCLUDE_DIRS     := include     #Относительные пути к каталогам c заголовочными файлами
EXTERN.ZLIB.SOURCE_DIRS      := sources     #Относительные пути к каталогам c исходными файлами
EXTERN.ZLIB.LIB_DIRS         :=             #Относительные пути к каталогам с библиотеками
EXTERN.ZLIB.LIBS             :=             #Список используемых при линковке библиотек
EXTERN.ZLIB.COMPILER_CFLAGS  :=             #Флаги сборки (зависят от конфигурации)
EXTERN.ZLIB.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE #Определения сборки
