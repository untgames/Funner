Application.LoadConfiguration ("data/conf/*.xform")
Application.MainForm:Add (Application.ToolStrips:Item ("ToolStrip1"))
Application.MainForm.Text = "Hello world"
Application.MainForm.MenuStrip = Application.MenuStrips:Item("MenuStrip1")
Application.MenuItems:Item ("MenuItem1").OnClick = "Application.MenuItems:Item ('MenuItem1').Image = 'data/app.ico'"
