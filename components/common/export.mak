# Compile exports for
export.compile.common.INCLUDE_DIRS     := include ../xtl/include
export.compile.common.COMPILER_DEFINES :=

# Link exports
export.link.common.LIBS := funner.common funner.extern.pcre
export.link.common.carbon.LINK_FLAGS := -framework Carbon
export.link.common.iphone.LINK_FLAGS := -framework CoreFoundation 

# AES link exports
export.link.common.aes.LIBS          := funner.common.aes
export.link.common.aes.LINK_INCLUDES := AesCrypto
export.link.common.aes.IMPORTS       := link.common

# Configurator link exports
export.link.common.configurator.LIBS          := funner.common.configurator
export.link.common.configurator.LINK_INCLUDES := Configurator
export.link.common.configurator.IMPORTS       := link.common link.common.xml

# DefaultConsoleHandler link exports
export.link.common.default_console_handler.LINK_INCLUDES := DefaultConsoleHandler
export.link.common.default_console_handler.IMPORTS       := link.common

# WXF link exports
export.link.common.wxf.LIBS          := funner.common.parsers.wxf 
export.link.common.wxf.LINK_INCLUDES := WxfParser
export.link.common.wxf.IMPORTS       := link.common

# XML link exports
export.link.common.xml.LIBS          := funner.common.parsers.xml
export.link.common.xml.LINK_INCLUDES := XmlParser
export.link.common.xml.IMPORTS       := link.common

# ZIP file system link exports
export.link.zip_file_system.LIBS          := funner.common.zip_file_system funner.extern.zzip funner.extern.zlib
export.link.zip_file_system.LINK_INCLUDES := ZipFileSystem
export.link.zip_file_system.IMPORTS       := link.common
