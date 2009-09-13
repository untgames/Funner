###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := COMMON.SOURCES COMMON.WXF_PARSER COMMON.XML_PARSER COMMON.ZIP_FILE_SYSTEM COMMON.CONFIGURATOR COMMON.AES COMMON.TESTS \
  COMMON.UTILS.FILE_CRYPTER

#Цель - CommonLib sources
COMMON.SOURCES.TYPE                              := static-lib
COMMON.SOURCES.NAME                              := funner.common
COMMON.SOURCES.INCLUDE_DIRS                      := include ../xtl/include ../../extern/pcre/include sources
COMMON.SOURCES.SOURCE_DIRS                       := sources/file_system/core sources/streams sources/hash sources/crypto/core sources/memory \
                                                    sources/parselib/tree sources/parselib/manager sources/strlib sources/utils sources/log \
                                                    sources/var_registry sources/platform/default
COMMON.SOURCES.COMPILER_DEFINES                  := PCRE_STATIC
COMMON.SOURCES.iphone_simulator.COMPILER_DEFINES := __IPHONE__
COMMON.SOURCES.unistd.SOURCE_DIRS                := sources/platform/unistd
COMMON.SOURCES.macosx.SOURCE_DIRS                := sources/platform/macosx
COMMON.SOURCES.win32.SOURCE_DIRS                 := sources/platform/win32

#Цель - WxfParser
COMMON.WXF_PARSER.TYPE             := static-lib
COMMON.WXF_PARSER.NAME             := funner.common.parsers.wxf
COMMON.WXF_PARSER.INCLUDE_DIRS     := include ../xtl/include
COMMON.WXF_PARSER.SOURCE_DIRS      := sources/parselib/parsers/wxf

#Цель - XmlParser
COMMON.XML_PARSER.TYPE             := static-lib
COMMON.XML_PARSER.NAME             := funner.common.parsers.xml
COMMON.XML_PARSER.INCLUDE_DIRS     := include ../xtl/include
COMMON.XML_PARSER.SOURCE_DIRS      := sources/parselib/parsers/xml

#Цель - CommonLib zip file system sources
COMMON.ZIP_FILE_SYSTEM.TYPE         := static-lib
COMMON.ZIP_FILE_SYSTEM.NAME         := funner.common.zip_file_system
COMMON.ZIP_FILE_SYSTEM.INCLUDE_DIRS := include ../xtl/include
COMMON.ZIP_FILE_SYSTEM.SOURCE_DIRS  := sources/file_system/zip
COMMON.ZIP_FILE_SYSTEM.IMPORTS      := ../../extern/zzip/compile.dynamic.mak

#Цель - CommonLib zip file system sources
COMMON.CONFIGURATOR.TYPE             := static-lib
COMMON.CONFIGURATOR.NAME             := funner.common.configurator
COMMON.CONFIGURATOR.SOURCE_DIRS      := sources/var_registry/configurator
COMMON.CONFIGURATOR.IMPORTS          := compile.static.mak

#Цель - CommonLib zip file system sources
COMMON.AES.TYPE             := static-lib
COMMON.AES.NAME             := funner.common.aes
COMMON.AES.SOURCE_DIRS      := sources/crypto/aes
COMMON.AES.IMPORTS          := compile.static.mak

#Цель - CommonLib tests
COMMON.TESTS.TYPE             := test-suite
COMMON.TESTS.SOURCE_DIRS      := tests/file_system tests/streams tests/hash tests/strlib tests/utils \
                                 tests/memory tests/log tests/parselib tests/crypto tests/var_registry
COMMON.TESTS.IMPORTS          := compile.static.mak link.static.mak zip_file_system.link.static.mak \
                                 wxf.link.static.mak xml.link.static.mak configurator.link.static.mak \
                                 default_console_handler.link.static.mak aes.link.static.mak \
                                 ../../extern/zzip/run.dynamic.mak

#Цель - CommonLib crypter
COMMON.UTILS.FILE_CRYPTER.TYPE             := application
COMMON.UTILS.FILE_CRYPTER.NAME             := file-crypter
COMMON.UTILS.FILE_CRYPTER.SOURCE_DIRS      := utils/file_crypter
COMMON.UTILS.FILE_CRYPTER.IMPORTS          := compile.static.mak link.static.mak aes.link.static.mak
