###################################################################################################
#Экспортируемые настройки компонента
###################################################################################################
export.LIBS          := funner.tools.ui.windows_forms ole32
export.LINK_INCLUDES := WindowsFormsSystem
export.IMPORTS       := ../core/link.static.mak ../../../common/xml.link.static.mak
export.DLLS          := WeifenLuo.WinFormsUI.Docking funner.tools.ui.windows_forms.PluginApi funner.tools.ui.windows_forms.Helpers System.Windows.Forms.Ribbon funner.tools.ui.windows_forms.TypeConverter
export.DLL_DIRS      := ../../../../extern/windows_controls/bin
