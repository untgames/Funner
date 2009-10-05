#Link exports
export.link.tools.ui.windows_forms.LIBS          := funner.tools.ui.windows_forms ole32
export.link.tools.ui.windows_forms.LINK_INCLUDES := WindowsFormsSystem
export.link.tools.ui.windows_forms.IMPORTS       := link.tools.ui.core link.common.xml
export.link.tools.ui.windows_forms.DLLS          := WeifenLuo.WinFormsUI.Docking funner.tools.ui.windows_forms.PluginApi funner.tools.ui.windows_forms.Helpers System.Windows.Forms.Ribbon
export.link.tools.ui.windows_forms.DLL_DIRS      := ../../../../extern/windows_controls/bin
