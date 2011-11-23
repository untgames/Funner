# Compile exports
export.compile.extern.geekinfo.INCLUDE_DIRS               := include
export.compile.extern.geekinfo.x86_win32.COMPILER_DEFINES := WIN32

# Link exports
export.link.extern.geekinfo.LIBS      := funner.extern.geekinfo 
export.link.extern.geekinfo.msvc.LIBS := WbemUuid
