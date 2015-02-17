# Compile exports
export.compile.extern.geekinfo.INCLUDE_DIRS                  := include
export.compile.extern.geekinfo.x86_win32.COMPILER_DEFINES    := WIN32 HAS_GEEK_INFO
export.compile.extern.geekinfo.x86-64_win32.COMPILER_DEFINES := WIN32 HAS_GEEK_INFO
export.compile.extern.geekinfo.x86.COMPILER_DEFINES          := HAS_GEEK_INFO
export.compile.extern.geekinfo.x86_winrt.COMPILER_DEFINES    := HAS_GEEK_INFO WIN32

# Link exports
export.link.extern.geekinfo.LIBS              := funner.extern.geekinfo 
export.link.extern.geekinfo.msvc.LIBS         := WbemUuid
export.link.extern.geekinfo.macosx.LINK_FLAGS := -framework IOKit -framework Foundation -framework Carbon
