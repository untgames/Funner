###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MEDIA.GEOMETRY.SOURCES MEDIA.GEOMETRY.XMESH MEDIA.GEOMETRY.TESTS MEDIA.GEOMETRY.XMESH.TESTS

#Цель №1 - sources
MEDIA.GEOMETRY.SOURCES.TYPE             := static-lib
MEDIA.GEOMETRY.SOURCES.NAME             := media.geometry
MEDIA.GEOMETRY.SOURCES.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.SOURCES.SOURCE_DIRS      := sources/core
MEDIA.GEOMETRY.SOURCES.LIB_DIRS         :=
MEDIA.GEOMETRY.SOURCES.LIBS             :=
MEDIA.GEOMETRY.SOURCES.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.SOURCES.COMPILER_DEFINES :=
MEDIA.GEOMETRY.SOURCES.IMPORTS          := compile.static.mak ../shared/compile.static.mak

#Цель №2 - xmesh serializer sources
MEDIA.GEOMETRY.XMESH.TYPE             := static-lib
MEDIA.GEOMETRY.XMESH.NAME             := media.geometry.xmesh
MEDIA.GEOMETRY.XMESH.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.XMESH.SOURCE_DIRS      := sources/xmesh
MEDIA.GEOMETRY.XMESH.LIB_DIRS         :=
MEDIA.GEOMETRY.XMESH.LIBS             :=
MEDIA.GEOMETRY.XMESH.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.XMESH.COMPILER_DEFINES :=
MEDIA.GEOMETRY.XMESH.IMPORTS          := compile.static.mak

#Цель №3 - tests
MEDIA.GEOMETRY.TESTS.TYPE             := test-suite
MEDIA.GEOMETRY.TESTS.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.TESTS.SOURCE_DIRS      := tests/core
MEDIA.GEOMETRY.TESTS.LIB_DIRS         :=
MEDIA.GEOMETRY.TESTS.LIBS             :=
MEDIA.GEOMETRY.TESTS.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.TESTS.COMPILER_DEFINES :=
MEDIA.GEOMETRY.TESTS.IMPORTS          := compile.static.mak link.static.mak

#Цель №4 - xmesh serializer tests
MEDIA.GEOMETRY.XMESH.TESTS.TYPE             := test-suite
MEDIA.GEOMETRY.XMESH.TESTS.INCLUDE_DIRS     :=
MEDIA.GEOMETRY.XMESH.TESTS.SOURCE_DIRS      := tests/xmesh
MEDIA.GEOMETRY.XMESH.TESTS.LIB_DIRS         :=
MEDIA.GEOMETRY.XMESH.TESTS.LIBS             :=
MEDIA.GEOMETRY.XMESH.TESTS.COMPILER_CFLAGS  :=
MEDIA.GEOMETRY.XMESH.TESTS.COMPILER_DEFINES :=
MEDIA.GEOMETRY.XMESH.TESTS.IMPORTS          := compile.static.mak xmesh.link.static.mak
