Application.LoadPlugins ("<BIN_DIR>/ui-plugins/*.dll")
Application.LoadConfiguration ("media/conf/*.xform")
Application.MainForm.Text = "Modeler"
Application.MainForm:Add (Application.ToolStrips:Item("ToolStrip1"))
Application.MainForm.MenuStrip = Application.MenuStrips:Item("MenuStrip1")
Application.CreateForm ("Application", "")
Application.CreateForm ("Console", "", DockState.Bottom)
