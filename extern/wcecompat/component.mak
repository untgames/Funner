###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := EXTERN.WCECOMPAT

#Target - WceCompat
EXTERN.WCECOMPAT.TYPE            := static-lib
EXTERN.WCECOMPAT.NAME            := funner.extern.wcecompat
EXTERN.WCECOMPAT.SOURCE_DIRS     := sources
EXTERN.WCECOMPAT.IMPORTS         := compile.extern.wcecompat
EXTERN.WCECOMPAT.COMPILER_CFLAGS := -Zc:wchar_t- -DUNICODE -D_UNICODE -Dwcecompat_EXPORTS
