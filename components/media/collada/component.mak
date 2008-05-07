###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.COLLADA.SOURCES MEDIA.COLLADA.DAE.SOURCES MEDIA.COLLADA.CONVERT.SOURCES MEDIA.COLLADA.DAE.TESTS MEDIA.COLLADA.CONVERT.TESTS

#Цель №1 - sources
MEDIA.COLLADA.SOURCES.TYPE             := static-lib
MEDIA.COLLADA.SOURCES.NAME             := media.collada
MEDIA.COLLADA.SOURCES.INCLUDE_DIRS     :=
MEDIA.COLLADA.SOURCES.SOURCE_DIRS      := sources/model
MEDIA.COLLADA.SOURCES.LIB_DIRS         :=  
MEDIA.COLLADA.SOURCES.LIBS             := 
MEDIA.COLLADA.SOURCES.COMPILER_CFLAGS  :=
MEDIA.COLLADA.SOURCES.COMPILER_DEFINES :=
MEDIA.COLLADA.SOURCES.IMPORTS          := compile.static.mak ../shared/compile.static.mak

#Цель №2 - DAE loader sources
MEDIA.COLLADA.DAE.SOURCES.TYPE             := static-lib
MEDIA.COLLADA.DAE.SOURCES.NAME             := media.collada.dae
MEDIA.COLLADA.DAE.SOURCES.INCLUDE_DIRS     :=
MEDIA.COLLADA.DAE.SOURCES.SOURCE_DIRS      := sources/dae_loader
MEDIA.COLLADA.DAE.SOURCES.LIB_DIRS         :=  
MEDIA.COLLADA.DAE.SOURCES.LIBS             := 
MEDIA.COLLADA.DAE.SOURCES.COMPILER_CFLAGS  :=
MEDIA.COLLADA.DAE.SOURCES.COMPILER_DEFINES :=
MEDIA.COLLADA.DAE.SOURCES.IMPORTS          := compile.static.mak link.static.mak

#Цель №3 - collada convert sources
MEDIA.COLLADA.CONVERT.SOURCES.TYPE             := static-lib
MEDIA.COLLADA.CONVERT.SOURCES.NAME             := media.collada.convert
MEDIA.COLLADA.CONVERT.SOURCES.INCLUDE_DIRS     :=
MEDIA.COLLADA.CONVERT.SOURCES.SOURCE_DIRS      := sources/convert
MEDIA.COLLADA.CONVERT.SOURCES.LIB_DIRS         :=
MEDIA.COLLADA.CONVERT.SOURCES.LIBS             := 
MEDIA.COLLADA.CONVERT.SOURCES.COMPILER_CFLAGS  :=
MEDIA.COLLADA.CONVERT.SOURCES.COMPILER_DEFINES :=
MEDIA.COLLADA.CONVERT.SOURCES.IMPORTS          := compile.static.mak ../geometry/compile.static.mak

#Цель №4 - DAE loader tests
MEDIA.COLLADA.DAE.TESTS.TYPE             := test-suite
MEDIA.COLLADA.DAE.TESTS.INCLUDE_DIRS     :=
MEDIA.COLLADA.DAE.TESTS.SOURCE_DIRS      := tests/dae_loader
MEDIA.COLLADA.DAE.TESTS.LIB_DIRS         :=  
MEDIA.COLLADA.DAE.TESTS.LIBS             := 
MEDIA.COLLADA.DAE.TESTS.COMPILER_CFLAGS  :=
MEDIA.COLLADA.DAE.TESTS.COMPILER_DEFINES :=
MEDIA.COLLADA.DAE.TESTS.IMPORTS          := compile.static.mak dae.link.static.mak

#Цель №5 - collada convert tests
MEDIA.COLLADA.CONVERT.TESTS.TYPE             := test-suite
MEDIA.COLLADA.CONVERT.TESTS.INCLUDE_DIRS     :=
MEDIA.COLLADA.CONVERT.TESTS.SOURCE_DIRS      := tests/convert
MEDIA.COLLADA.CONVERT.TESTS.LIB_DIRS         :=
MEDIA.COLLADA.CONVERT.TESTS.LIBS             :=
MEDIA.COLLADA.CONVERT.TESTS.COMPILER_CFLAGS  :=
MEDIA.COLLADA.CONVERT.TESTS.COMPILER_DEFINES :=
MEDIA.COLLADA.CONVERT.TESTS.IMPORTS          := compile.static.mak ../geometry/compile.static.mak \
                                                dae.link.static.mak convert.link.static.mak ../geometry/link.static.mak \
                                                ../geometry/xmesh.link.static.mak
