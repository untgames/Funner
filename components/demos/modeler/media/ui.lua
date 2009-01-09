Application.LoadPlugins ("<BIN_DIR>/ui-plugins/*.dll")
Application.LoadConfiguration ("media/conf/*.xform")

MainFormTitle = "Modeler"
Application.MainForm.Text = MainFormTitle

Application.MainForm:Add (Application.ToolStrips:Item("ToolStrip1"))
Application.MainForm.OnClosed = "Application.Execute ('close')"
Application.CreateForm ("Application", "")
Application.CreateForm ("PropertyGrid", "Render Properties", DockState.Right)
Application.CreateForm ("Console", "", DockState.Bottom)
