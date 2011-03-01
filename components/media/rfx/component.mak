###################################################################################################
#Определения и константы
###################################################################################################
TARGETS += MEDIA.RFX.SOURCES
#TARGETS += MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES
TARGETS += MEDIA.RFX.TESTS
#TARGETS += MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS
#TARGETS += MEDIA.RFX.INFO

#Цель №1 - sources
MEDIA.RFX.SOURCES.TYPE             := static-lib
MEDIA.RFX.SOURCES.NAME             := funner.media.rfx
MEDIA.RFX.SOURCES.INCLUDE_DIRS     :=
MEDIA.RFX.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.RFX.SOURCES.LIB_DIRS         :=  
MEDIA.RFX.SOURCES.LIBS             := 
MEDIA.RFX.SOURCES.COMPILER_CFLAGS  :=
MEDIA.RFX.SOURCES.COMPILER_DEFINES :=
MEDIA.RFX.SOURCES.IMPORTS          := compile.media.rfx compile.media.shared

#Цель №2 - Default serializers sources
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.TYPE             := static-lib
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.NAME             := funner.media.rfx.default_serializers
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.INCLUDE_DIRS     :=
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.SOURCE_DIRS      := sources/default_serializers
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.LIB_DIRS         :=  
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.LIBS             := 
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.COMPILER_CFLAGS  :=
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.COMPILER_DEFINES :=
MEDIA.RFX.DEFAULT_SERIALIZERS.SOURCES.IMPORTS          := compile.media.rfx

#Цель №3 - tests
MEDIA.RFX.TESTS.TYPE             := test-suite
MEDIA.RFX.TESTS.INCLUDE_DIRS     :=
MEDIA.RFX.TESTS.SOURCE_DIRS      := tests/core
MEDIA.RFX.TESTS.LIB_DIRS         :=  
MEDIA.RFX.TESTS.LIBS             :=
MEDIA.RFX.TESTS.COMPILER_CFLAGS  :=
MEDIA.RFX.TESTS.COMPILER_DEFINES :=
MEDIA.RFX.TESTS.IMPORTS          := link.media.rfx compile.media.rfx

#Цель №4 - XMTL tests
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.TYPE             := test-suite
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.INCLUDE_DIRS     :=
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.SOURCE_DIRS      := tests/default_serializers
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.LIB_DIRS         :=  
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.LIBS             :=
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.COMPILER_CFLAGS  :=
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.COMPILER_DEFINES :=
MEDIA.RFX.DEFAULT_SERIALIZERS.TESTS.IMPORTS          := link.media.rfx.default_serializers compile.media.rfx

#Цель - сборка документации
MEDIA.RFX.INFO.TYPE        := doxygen-info
MEDIA.RFX.INFO.CHM_NAME    := funner.media.rfx
MEDIA.RFX.INFO.SOURCE_DIRS := include
MEDIA.RFX.INFO.IMPORTS     := compile.media.rfx
