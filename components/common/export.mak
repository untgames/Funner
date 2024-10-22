# Auto license generator link exports
export.link.common.auto_license_generator.LIBS          := funner.common.auto_license_generator funner.common.parsers.xml funner.common
export.link.common.auto_license_generator.LINK_INCLUDES := AutoLicenseGenerator XmlParser

# Memory manager override
export.link.common.memory_manager_override.LIBS := funner.common.memory_manager_override

# Compile exports for
export.compile.common.INCLUDE_DIRS     := include ../xtl/include
export.compile.common.COMPILER_DEFINES :=

# Link exports
export.link.common.LIBS                        := funner.common
export.link.common.IMPORTS                     := link.common.auto_license_generator link.extern.openssl link.extern.lib64 link.extern.pcre # link.common.memory_manager_override 
export.link.common.LINK_INCLUDES               := StandardFilePathsMount
export.link.common.mingw.LIBS                  := shell32
export.link.common.vcx86.LIBS                  := shell32
export.link.common.vcx86-64.LIBS               := shell32
export.link.common.winrt.LIBS                  := shell32
export.link.common.wince.LIBS                  := coredll
export.link.common.linux.LIBS                  := rt
export.link.common.cocoa.LINK_FLAGS            := -framework CoreFoundation -framework Foundation
export.link.common.cocoa_desktop.LINK_FLAGS    := -framework AppKit
export.link.common.pthread_static_library.LIBS := pthread
export.link.common.wince.IMPORTS               := link.extern.wcecompat

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

# JSON link exports
export.link.common.json.LIBS          := funner.common.parsers.json
export.link.common.json.LINK_INCLUDES := JsonParser
export.link.common.json.IMPORTS       := link.common

# ZIP file system link exports
export.link.common.zip_file_system.LIBS          := funner.common.zip_file_system
export.link.common.zip_file_system.LINK_INCLUDES := ZipFileSystem
export.link.common.zip_file_system.IMPORTS       := link.common link.extern.zlib link.extern.zzip run.extern.zzip

# ICONV converters
export.link.common.iconv.LIBS          := funner.common.iconv
export.link.common.iconv.LINK_INCLUDES := IconvConverter
export.link.common.iconv.IMPORTS       := link.common link.extern.iconv

export.info.common.CHMS := funner.common
