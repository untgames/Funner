###################################################################################################
#����������� � ���������
###################################################################################################
TARGETS := HSSERVER.SOURCES

HSSERVER.SOURCES.TYPE         := static-lib
HSSERVER.SOURCES.NAME         := plarium.hsserver
HSSERVER.SOURCES.INCLUDE_DIRS += include
HSSERVER.SOURCES.SOURCE_DIRS  += sources
HSSERVER.SOURCES.IMPORTS      := compile.plarium.system
