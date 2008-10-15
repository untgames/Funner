###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.PCRE

#Цель №1 - PCRE
EXTERN.PCRE.TYPE                  := static-lib               #Тип цели
EXTERN.PCRE.NAME                  := funner.extern.pcre              #Имя выходного файла
EXTERN.PCRE.INCLUDE_DIRS          := include                  #Относительные пути к каталогам c заголовочными файлами
EXTERN.PCRE.SOURCE_DIRS           := sources                  #Относительные пути к каталогам c исходными файлами
EXTERN.PCRE.LIB_DIRS              :=                          #Относительные пути к каталогам с библиотеками
EXTERN.PCRE.LIBS                  :=                          #Список используемых при линковке библиотек
EXTERN.PCRE.COMPILER_DEFINES      := PCRE_STATIC              #Определения сборки
EXTERN.PCRE.COMPILER_CFLAGS       :=                          #Флаги сборки (зависят от конфигурации)
EXTERN.PCRE.msvc.COMPILER_DEFINES := _CRT_SECURE_NO_DEPRECATE #Определения сборки
EXTERN.PCRE.g++.COMPILER_CFLAGS   := --no-warn                #Флаги сборки (зависят от конфигурации)
