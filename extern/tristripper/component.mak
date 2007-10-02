###################################################################################################
#Список целей
###################################################################################################
TARGETS := TRISTRIPPER TRISTRIPPER-TESTS

#Цель №1 - TRISTRIPPER
TRISTRIPPER.TYPE                    := static-lib     #Тип цели
TRISTRIPPER.NAME                    := libtristripper #Имя выходного файла
TRISTRIPPER.INCLUDE_DIRS            := include        #Относительные пути к каталогам c заголовочными файлами
TRISTRIPPER.SOURCE_DIRS             := src            #Относительные пути к каталогам c исходными файлами
TRISTRIPPER.LIB_DIRS                :=                #Относительные пути к каталогам с библиотеками
TRISTRIPPER.LIBS                    :=                #Список используемых при линковке библиотек
TRISTRIPPER.COMPILER_DEFINES        :=  					    #Определения сборки
TRISTRIPPER.COMPILER_CFLAGS         :=                #Флаги сборки (зависят от конфигурации)

#Цель №2 - TRISTRIPPER-TESTS
TRISTRIPPER-TESTS.TYPE              := application    #Тип цели
TRISTRIPPER-TESTS.NAME              := test           #Имя выходного файла
TRISTRIPPER-TESTS.INCLUDE_DIRS      := include        #Относительные пути к каталогам c заголовочными файлами
TRISTRIPPER-TESTS.SOURCE_DIRS       := test           #Относительные пути к каталогам c исходными файлами
TRISTRIPPER-TESTS.LIB_DIRS          :=                #Относительные пути к каталогам с библиотеками
TRISTRIPPER-TESTS.LIBS              := libtristripper #Список используемых при линковке библиотек
TRISTRIPPER-TESTS.COMPILER_DEFINES  := WIN32 					    #Определения сборки
TRISTRIPPER-TESTS.COMPILER_CFLAGS   :=                #Флаги сборки (зависят от конфигурации)
