###################################################################################################
#Список целей
###################################################################################################
TARGETS := VORBIS_STATIC

#Цель №1 - VORBIS_STATIC
VORBIS_STATIC.TYPE             := static-lib              #Тип цели
VORBIS_STATIC.NAME             := vorbis_static           #Имя выходного файла
VORBIS_STATIC.INCLUDE_DIRS     := include ../ogg/include  #Относительные пути к каталогам c заголовочными файлами
VORBIS_STATIC.SOURCE_DIRS      := sources    						  #Относительные пути к каталогам c исходными файлами
VORBIS_STATIC.LIB_DIRS         :=                         #Относительные пути к каталогам с библиотеками
VORBIS_STATIC.LIBS             :=                         #Список используемых при линковке библиотек
VORBIS_STATIC.COMPILER_DEFINES :=  					              #Определения сборки
VORBIS_STATIC.COMPILER_CFLAGS  :=                         #Флаги сборки (зависят от конфигурации)
