print("loading data/scripts/UI/* files")

  UI={} --интерфейс пользователя
  
  local Folder="data/scripts/UI"
  dofile(Folder.."/CreateWinRectStyle.lua")
  dofile(Folder.."/CreateWinRect.lua")
  dofile(Folder.."/CreateTextStyle.lua")
  dofile(Folder.."/CreateWinStyle.lua")
  dofile(Folder.."/Control_Methods.lua")
  dofile(Folder.."/CreateControl.lua")
  dofile(Folder.."/MakeButton.lua")
  dofile(Folder.."/MakeWindow.lua")
  dofile(Folder.."/WindowEditor.lua")
  dofile(Folder.."/WindowManager.lua")

  --инициализация
    InitWindowEditor()
    InitWindowManager(screen)
    
        