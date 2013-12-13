###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.ZLIB

#Цель №1 - ZLIB
EXTERN.ZLIB.TYPE                  := static-lib  #Тип цели
EXTERN.ZLIB.NAME                  := funner.extern.zlib #Имя выходного файла
EXTERN.ZLIB.SOURCE_DIRS           := sources     #Относительные пути к каталогам c исходными файлами
EXTERN.ZLIB.IMPORTS               := compile.extern.zlib
