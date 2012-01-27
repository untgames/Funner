# Compile exports
export.compile.extern.curl.INCLUDE_DIRS     := include
export.compile.extern.curl.COMPILER_DEFINES := CURL_STATICLIB

# Link exports
export.link.extern.curl.LIBS             := funner.extern.curl
export.link.extern.curl.x86_win32.LIBS   := wsock32 ws2_32 wldap32
export.link.extern.curl.wince.LIBS       := winsock ws2 wldap32
export.link.extern.curl.macosx.LIBS      := ldap
export.link.extern.curl.macosx.IMPORTS   := link.extern.zlib
export.link.extern.curl.iphone.IMPORTS   := link.extern.zlib
export.link.extern.curl.linux.LIBS       := rt
export.link.extern.curl.linux.IMPORTS    := link.extern.zlib
export.link.extern.curl.tabletos.LIBS    := socket
export.link.extern.curl.tabletos.IMPORTS := link.extern.zlib
export.link.extern.curl.wince.IMPORTS    := link.extern.wcecompat
export.link.extern.curl.IMPORTS          := link.extern.openssl