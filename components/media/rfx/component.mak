###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.RFX.OBSOLETE.SOURCES MEDIA.RFX.OBSOLETE.XMTL.SOURCES MEDIA.RFX.OBSOLETE.TESTS MEDIA.RFX.OBSOLETE.XMTL.TESTS

#Цель №1 - sources
MEDIA.RFX.OBSOLETE.SOURCES.TYPE             := static-lib
MEDIA.RFX.OBSOLETE.SOURCES.NAME             := funner.media.rfx.obsolete
MEDIA.RFX.OBSOLETE.SOURCES.INCLUDE_DIRS     :=
MEDIA.RFX.OBSOLETE.SOURCES.SOURCE_DIRS      := sources/obsolete/core
MEDIA.RFX.OBSOLETE.SOURCES.LIB_DIRS         :=  
MEDIA.RFX.OBSOLETE.SOURCES.LIBS             := 
MEDIA.RFX.OBSOLETE.SOURCES.COMPILER_CFLAGS  :=
MEDIA.RFX.OBSOLETE.SOURCES.COMPILER_DEFINES :=
MEDIA.RFX.OBSOLETE.SOURCES.IMPORTS          := compile.media.rfx.obsolete compile.media.shared

#Цель №2 - XMTL serializer sources
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.TYPE             := static-lib
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.NAME             := funner.media.rfx.xmtl.obsolete
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.INCLUDE_DIRS     :=
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.SOURCE_DIRS      := sources/obsolete/xmtl
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.LIB_DIRS         :=  
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.LIBS             := 
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.COMPILER_CFLAGS  :=
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.COMPILER_DEFINES :=
MEDIA.RFX.OBSOLETE.XMTL.SOURCES.IMPORTS          := compile.media.rfx.obsolete

#Цель №3 - tests
MEDIA.RFX.OBSOLETE.TESTS.TYPE             := test-suite
MEDIA.RFX.OBSOLETE.TESTS.INCLUDE_DIRS     :=
MEDIA.RFX.OBSOLETE.TESTS.SOURCE_DIRS      := tests/obsolete/core
MEDIA.RFX.OBSOLETE.TESTS.LIB_DIRS         :=  
MEDIA.RFX.OBSOLETE.TESTS.LIBS             :=
MEDIA.RFX.OBSOLETE.TESTS.COMPILER_CFLAGS  :=
MEDIA.RFX.OBSOLETE.TESTS.COMPILER_DEFINES :=
MEDIA.RFX.OBSOLETE.TESTS.IMPORTS          := link.media.rfx.obsolete compile.media.rfx.obsolete

#Цель №4 - XMTL tests
MEDIA.RFX.OBSOLETE.XMTL.TESTS.TYPE             := test-suite
MEDIA.RFX.OBSOLETE.XMTL.TESTS.INCLUDE_DIRS     :=
MEDIA.RFX.OBSOLETE.XMTL.TESTS.SOURCE_DIRS      := tests/obsolete/xmtl
MEDIA.RFX.OBSOLETE.XMTL.TESTS.LIB_DIRS         :=  
MEDIA.RFX.OBSOLETE.XMTL.TESTS.LIBS             :=
MEDIA.RFX.OBSOLETE.XMTL.TESTS.COMPILER_CFLAGS  :=
MEDIA.RFX.OBSOLETE.XMTL.TESTS.COMPILER_DEFINES :=
MEDIA.RFX.OBSOLETE.XMTL.TESTS.IMPORTS          := link.media.rfx.xmtl.obsolete compile.media.rfx.obsolete
