###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := COMMON.SOURCES COMMON.WXF_PARSER COMMON.XML_PARSER COMMON.ZIP_FILE_SYSTEM COMMON.AES \
  COMMON.AUTO_LICENSE_GENERATOR COMMON.TESTS COMMON.UTILS.FILE_CRYPTER COMMON.UTILS.LICENSE_GENERATOR COMMON.INFO

#Цель - CommonLib sources
COMMON.SOURCES.TYPE                       := static-lib
COMMON.SOURCES.NAME                       := funner.common
COMMON.SOURCES.INCLUDE_DIRS               := include ../xtl/include ../../extern/pcre/include sources
COMMON.SOURCES.SOURCE_DIRS                := sources/file_system/core sources/streams sources/hash sources/crypto/core sources/memory \
                                             sources/parselib/tree sources/parselib/manager sources/parselib/utils sources/strlib \
                                             sources/utils sources/log sources/licensing/core sources/platform/default
COMMON.SOURCES.DOCUMENTATION_DIRS         := include
COMMON.SOURCES.COMPILER_DEFINES           := PCRE_STATIC
COMMON.SOURCES.IMPORTS                    := compile.math.vecmath
COMMON.SOURCES.unistd.SOURCE_DIRS         := sources/platform/unistd
COMMON.SOURCES.cocoa.SOURCE_DIRS          := sources/platform/cocoa
COMMON.SOURCES.cocoa_desktop.SOURCE_DIRS  := sources/platform/cocoa_desktop
COMMON.SOURCES.win32.SOURCE_DIRS          := sources/platform/win32
COMMON.SOURCES.bada_simulator.SOURCE_DIRS := sources/platform/win32

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
COMMON.ZIP_FILE_SYSTEM.IMPORTS      := compile.extern.zzip

#Цель - CommonLib aes sources
COMMON.AES.TYPE             := static-lib
COMMON.AES.NAME             := funner.common.aes
COMMON.AES.SOURCE_DIRS      := sources/crypto/aes
COMMON.AES.IMPORTS          := compile.common

#Цель - CommonLib auto license generator
COMMON.AUTO_LICENSE_GENERATOR.TYPE        := static-lib
COMMON.AUTO_LICENSE_GENERATOR.NAME        := funner.common.auto_license_generator
COMMON.AUTO_LICENSE_GENERATOR.INCLUDE_DIRS := sources/licensing/core
COMMON.AUTO_LICENSE_GENERATOR.SOURCE_DIRS := sources/licensing/auto_license_generator
COMMON.AUTO_LICENSE_GENERATOR.IMPORTS     := compile.common

#Цель - CommonLib tests
COMMON.TESTS.TYPE                 := test-suite
COMMON.TESTS.SOURCE_DIRS          := tests/file_system tests/streams tests/hash tests/strlib tests/utils \
                                     tests/memory tests/log tests/parselib tests/crypto
COMMON.TESTS.haswchar.SOURCE_DIRS := tests/strlib/wchar
COMMON.TESTS.IMPORTS              := compile.math.vecmath compile.common link.common.aes link.common.xml link.common.wxf \
                                     link.common.zip_file_system link.common.default_console_handler

#Цель - CommonLib crypter
COMMON.UTILS.FILE_CRYPTER.TYPE        := application
COMMON.UTILS.FILE_CRYPTER.NAME        := file-crypter
COMMON.UTILS.FILE_CRYPTER.SOURCE_DIRS := utils/file_crypter
COMMON.UTILS.FILE_CRYPTER.IMPORTS     := compile.common link.common link.common.aes

#Цель - License generator
COMMON.UTILS.LICENSE_GENERATOR.TYPE         := application
COMMON.UTILS.LICENSE_GENERATOR.NAME         := license-generator
COMMON.UTILS.LICENSE_GENERATOR.INCLUDE_DIRS := sources/licensing/core
COMMON.UTILS.LICENSE_GENERATOR.SOURCE_DIRS  := utils/license_generator
COMMON.UTILS.LICENSE_GENERATOR.IMPORTS      := compile.common link.common

#Цель - сборка документации
COMMON.INFO.TYPE        := doxygen-info
COMMON.INFO.CHM_NAME    := funner.common
COMMON.INFO.SOURCE_DIRS := include
COMMON.INFO.IMPORTS     := compile.common
