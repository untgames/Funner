Application.LoadPlugins ("<BIN_DIR>/ui-plugins/*.dll")
Application.LoadConfiguration ("media/conf/*.xform")

MainFormTitle = "Modeler"
Application.MainForm.Text = MainFormTitle

Application.MainForm.OnClosed = "Application.Execute ('close')"
Application.CreateForm ("Application", "")
Application.CreateForm ("PropertyGrid", "Render Properties", DockState.Right)
Application.CreateForm ("Ribbon", "MainRibbon", DockState.Top)
Application.CreateForm ("Console", "", DockState.Bottom)

