###################################################################################################
#Определения и константы
###################################################################################################
TARGETS := MAX8EXPORT_PLUGIN

#Цель №1 - Max8 export plugin sources
MAX8EXPORT_PLUGIN.TYPE             := dynamic-lib
MAX8EXPORT_PLUGIN.NAME             := megaexp.dle
MAX8EXPORT_PLUGIN.INCLUDE_DIRS     := ../../../extern/max8sdk/include
MAX8EXPORT_PLUGIN.SOURCE_DIRS      := .
MAX8EXPORT_PLUGIN.LIB_DIRS         := ../../../extern/max8sdk/lib
MAX8EXPORT_PLUGIN.LIBS             := comctl32 kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32 ole32 oleaut32 \
                                      uuid core geom gfx mesh maxutil helpsys
MAX8EXPORT_PLUGIN.COMPILER_CFLAGS  :=
MAX8EXPORT_PLUGIN.COMPILER_DEFINES :=
