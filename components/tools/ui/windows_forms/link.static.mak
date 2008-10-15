###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.LIBS          := funner.tools.ui.windows_forms ole32
export.LINK_INCLUDES := WindowsFormsSystem
export.IMPORTS       := ../core/link.static.mak ../../../common/xml.link.static.mak
export.DLLS          := WeifenLuo.WinFormsUI.Docking
export.DLL_DIRS      := ../../../../extern/windows_controls/bin
