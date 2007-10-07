###################################################################################################
#Список целей
###################################################################################################
TARGETS := TRISTRIPPER

#Цель №1 - TRISTRIPPER
TRISTRIPPER.TYPE                    := static-lib     #Тип цели
TRISTRIPPER.NAME                    := libtristripper #Имя выходного файла
TRISTRIPPER.INCLUDE_DIRS            := include        #Относительные пути к каталогам c заголовочными файлами
TRISTRIPPER.SOURCE_DIRS             := src            #Относительные пути к каталогам c исходными файлами
TRISTRIPPER.LIB_DIRS                :=                #Относительные пути к каталогам с библиотеками
TRISTRIPPER.LIBS                    :=                #Список используемых при линковке библиотек
TRISTRIPPER.COMPILER_DEFINES        :=  					    #Определения сборки
TRISTRIPPER.COMPILER_CFLAGS         :=                #Флаги сборки (зависят от конфигурации)
