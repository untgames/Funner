###################################################################################################
#Список целей
###################################################################################################
TARGETS := VORBISFILE_STATIC

#Цель №1 - VORBISFILE_STATIC
VORBISFILE_STATIC.TYPE             := static-lib              #Тип цели
VORBISFILE_STATIC.NAME             := vorbisfile_static           #Имя выходного файла
VORBISFILE_STATIC.INCLUDE_DIRS     := include ../vorbis/include ../ogg/include  #Относительные пути к каталогам c заголовочными файлами
VORBISFILE_STATIC.SOURCE_DIRS      := sources    						  #Относительные пути к каталогам c исходными файлами
VORBISFILE_STATIC.LIB_DIRS         :=                         #Относительные пути к каталогам с библиотеками
VORBISFILE_STATIC.LIBS             :=                         #Список используемых при линковке библиотек
VORBISFILE_STATIC.COMPILER_DEFINES :=  					              #Определения сборки
VORBISFILE_STATIC.COMPILER_CFLAGS  :=                         #Флаги сборки (зависят от конфигурации)
