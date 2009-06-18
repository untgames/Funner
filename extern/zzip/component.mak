###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.ZZIP

#Цель №1 - ZZIP
EXTERN.ZZIP.TYPE                       := dynamic-lib                     #Тип цели
EXTERN.ZZIP.NAME                       := funner.extern.zzip              #Имя выходного файла
EXTERN.ZZIP.INCLUDE_DIRS               := include                         #Относительные пути к каталогам c заголовочными файлами
EXTERN.ZZIP.SOURCE_DIRS                := sources                         #Относительные пути к каталогам c исходными файлами
EXTERN.ZZIP.IMPORTS                    := compile.dynamic.mak ../zlib/compile.static.mak ../zlib/link.static.mak
EXTERN.ZZIP.msvc.COMPILER_CFLAGS       :=  -wd4244                        #Флаги сборки (зависят от конфигурации)
EXTERN.ZZIP.msvc.COMPILER_DEFINES      := _CRT_SECURE_NO_DEPRECATE        #Определения сборки
EXTERN.ZZIP.g++.COMPILER_CFLAGS        := --no-warn                       #Флаги сборки (зависят от конфигурации)
EXTERN.ZZIP.mingw.g++.COMPILER_DEFINES := ZZIP_2_H
