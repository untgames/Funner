###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := EXTERN.WINCE.LAUNCHER

#Target - WceCompat
EXTERN.WINCE.LAUNCHER.TYPE            := application
EXTERN.WINCE.LAUNCHER.NAME            := wince-launcher
EXTERN.WINCE.LAUNCHER.SOURCE_DIRS     := utils/launcher
EXTERN.WINCE.LAUNCHER.LIBS            := ws2 
EXTERN.WINCE.LAUNCHER.LINK_FLAGS      := -entry:mainWCRTStartup
