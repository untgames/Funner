###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.ZZIP

#Цель №1 - ZZIP
EXTERN.ZZIP.TYPE             := static-lib               #Тип цели
EXTERN.ZZIP.NAME             := extern.zzip              #Имя выходного файла
EXTERN.ZZIP.INCLUDE_DIRS     := include ../zlib/include  #Относительные пути к каталогам c заголовочными файлами
EXTERN.ZZIP.SOURCE_DIRS      := sources                  #Относительные пути к каталогам c исходными файлами
EXTERN.ZZIP.LIB_DIRS         :=                          #Относительные пути к каталогам с библиотеками
EXTERN.ZZIP.LIBS             :=                          #Список используемых при линковке библиотек
EXTERN.ZZIP.COMPILER_CFLAGS  :=                          #Флаги сборки (зависят от конфигурации)
EXTERN.ZZIP.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE #Определения сборки
EXTERN.ZZIP.g++.COMPILER_CFLAGS := --no-warn                #Флаги сборки (зависят от конфигурации)
EXTERN.ZZIP.nds.g++.COMPILER_DEFINES := ZZIP_1_H
EXTERN.ZZIP.macosx.COMPILER_DEFINES := ZZIP_1_H
