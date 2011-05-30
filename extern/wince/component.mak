###################################################################################################
#Список целей
###################################################################################################
TARGETS := EXTERN.WINCE.LAUNCHER

#Цель - WceCompat
EXTERN.WINCE.LAUNCHER.TYPE            := application
EXTERN.WINCE.LAUNCHER.NAME            := wince-launcher
EXTERN.WINCE.LAUNCHER.SOURCE_DIRS     := utils
EXTERN.WINCE.LAUNCHER.LIBS            := ws2
EXTERN.WINCE.LAUNCHER.IMPORTS         := compile.extern.wcecompat
