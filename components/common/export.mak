# Compile exports for
export.compile.common.INCLUDE_DIRS     := include ../xtl/include
export.compile.common.COMPILER_DEFINES :=

# Link exports
export.link.common.LIBS := funner.common funner.extern.pcre
export.link.common.carbon.LINK_FLAGS := -framework Carbon
export.link.common.iphone.LINK_FLAGS := -framework CoreFoundation 
export.link.common.pthread_static_library.LIBS := pthread

# AES link exports
export.link.common.aes.LIBS          := funner.common.aes
export.link.common.aes.LINK_INCLUDES := AesCrypto
export.link.common.aes.IMPORTS       := link.common

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
export.link.common.zip_file_system.LIBS          := funner.common.zip_file_system
export.link.common.zip_file_system.LINK_INCLUDES := ZipFileSystem
export.link.common.zip_file_system.IMPORTS       := link.common link.extern.zlib link.extern.zzip run.extern.zzip
