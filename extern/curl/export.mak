# Compile exports
export.compile.extern.curl.INCLUDE_DIRS     := include
export.compile.extern.curl.COMPILER_DEFINES := CURL_STATICLIB

# Link exports
export.link.extern.curl.LIBS           := funner.extern.curl
export.link.extern.curl.win32.LIBS     := wsock32 ws2_32 wldap32
export.link.extern.curl.macosx.LIBS    := ldap ssl crypto
export.link.extern.curl.macosx.IMPORTS := link.extern.zlib
export.link.extern.curl.iphone.IMPORTS := link.extern.zlib
export.link.extern.curl.linux.LIBS     := rt
export.link.extern.curl.linux.IMPORTS  := link.extern.zlib
