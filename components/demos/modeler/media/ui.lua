Application.LoadPlugins ("<BIN_DIR>/ui-plugins/*.dll")
Application.LoadConfiguration ("media/conf/*.xform")

MainFormTitle = "Modeler"
Application.MainForm.Text = MainFormTitle
Application.MainForm.MenuStrip = Application.MenuStrips:Item("FileMenu")

Application.MainForm.OnClosed = "Application.Execute ('close')"
Application.CreateForm ("Application", "")
Application.CreateForm ("PropertyGrid", "Render Properties", DockState.Right)
--Application.CreateForm ("Ribbon", "MainRibbon", DockState.Top)
Application.CreateForm ("Console", "", DockState.Bottom)

function toogle_visualize_new_calculations()
  return
--[[  local item = Application.MenuItems:Item ('VisualuzeNewCalculations')
  
  if not (item) then return end

  if item.Text == "Визуализировать новые расчёты" then
    print ("!!!")
--    item.Text = "Не визуализировать новые расчёты"  
  else
    print ("???")  
--    item.Text = "Визуализировать новые расчёты"
  end ]]
end