Application.LoadPlugins ("<BIN_DIR>/ui-plugins/*.dll")
Application.LoadConfiguration ("data/conf/*.xform")
Application.MainForm:Add (Application.ToolStrips:Item ("ToolStrip1"))
Application.MainForm.Text = "Hello world"
Application.MainForm.MenuStrip = Application.MenuStrips:Item("MenuStrip1")
Application.MenuItems:Item ("MenuItem1").OnClick = "Application.MenuItems:Item ('MenuItem1').Image = 'data/app.ico'"

Application.Execute ("dofile ('data/setup_var_registry.lua')")

Application.CreateForm ("PropertyGrid", "Map2 registry1", DockState.Right)
form = Application.CreateForm ("Console", "", DockState.Bottom)

Application.MainForm.OnClosed = "Application.Execute ('close')"
