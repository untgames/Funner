# Auto license generator link exports
export.link.common.auto_license_generator.LIBS          := funner.common.auto_license_generator funner.common.parsers.xml
export.link.common.auto_license_generator.LINK_INCLUDES := AutoLicenseGenerator XmlParser

# Compile exports for
export.compile.common.INCLUDE_DIRS     := include ../xtl/include
export.compile.common.COMPILER_DEFINES :=

# Link exports
export.link.common.LIBS                        := funner.common funner.extern.pcre
export.link.common.IMPORTS                     := link.common.auto_license_generator
export.link.common.LINK_INCLUDES               := StandardFilePathsMount
export.link.common.win32.LIBS                  := shell32
export.link.common.cocoa.LINK_FLAGS            := -framework CoreFoundation -framework Foundation
export.link.common.cocoa_desktop.LINK_FLAGS    := -framework AppKit
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
