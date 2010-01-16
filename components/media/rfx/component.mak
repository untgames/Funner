###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.RFX.SOURCES #MEDIA.RFX.XMTL.SOURCES MEDIA.RFX.TESTS MEDIA.RFX.XMTL.TESTS

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

#Цель №2 - XMTL serializer sources
MEDIA.RFX.XMTL.SOURCES.TYPE             := static-lib
MEDIA.RFX.XMTL.SOURCES.NAME             := funner.media.rfx.xmtl
MEDIA.RFX.XMTL.SOURCES.INCLUDE_DIRS     :=
MEDIA.RFX.XMTL.SOURCES.SOURCE_DIRS      := sources/xmtl
MEDIA.RFX.XMTL.SOURCES.LIB_DIRS         :=  
MEDIA.RFX.XMTL.SOURCES.LIBS             := 
MEDIA.RFX.XMTL.SOURCES.COMPILER_CFLAGS  :=
MEDIA.RFX.XMTL.SOURCES.COMPILER_DEFINES :=
MEDIA.RFX.XMTL.SOURCES.IMPORTS          := compile.media.rfx

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
MEDIA.RFX.XMTL.TESTS.TYPE             := test-suite
MEDIA.RFX.XMTL.TESTS.INCLUDE_DIRS     :=
MEDIA.RFX.XMTL.TESTS.SOURCE_DIRS      := tests/xmtl
MEDIA.RFX.XMTL.TESTS.LIB_DIRS         :=  
MEDIA.RFX.XMTL.TESTS.LIBS             :=
MEDIA.RFX.XMTL.TESTS.COMPILER_CFLAGS  :=
MEDIA.RFX.XMTL.TESTS.COMPILER_DEFINES :=
MEDIA.RFX.XMTL.TESTS.IMPORTS          := link.media.rfx.xmtl compile.media.rfx
