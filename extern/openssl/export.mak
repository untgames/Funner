# Compile exports
export.compile.extern.openssl.INCLUDE_DIRS            := include include/openssl
export.compile.extern.openssl.unistd.COMPILER_DEFINES := HAVE_UNISTD_H
export.compile.extern.openssl.bada.COMPILER_DEFINES   := HAVE_UNISTD_H

# Link exports
export.link.extern.openssl.LIBS       := funner.extern.openssl
export.link.extern.openssl.win32.LIBS := ws2_32 gdi32 advapi32 crypt32 user32
export.link.extern.openssl.win8.LIBS  := gdi32 user32 advapi32
