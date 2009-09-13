###################################################################################################
#Ёкспортируемые настройки
###################################################################################################
export.LIBS           := funner.extern.curl
export.win32.LIBS     := wsock32 ws2_32 wldap32
export.macosx.LIBS    := ssl crypto ldap
export.macosx.IMPORTS := ../zlib/link.static.mak
export.linux.LIBS     := rt
export.linux.IMPORTS  := ../zlib/link.static.mak
