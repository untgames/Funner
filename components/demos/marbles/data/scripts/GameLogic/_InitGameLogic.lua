function InitGameLogic()
  LocationScript={}
  
  local Folder="data/scripts/GameLogic/"
  dofile(Folder.."InitGame.lua")
  dofile(Folder.."Window_Minigame_Balls.lua")

  local LoadWindow=WindowManager.LoadWindow
  LoadWindow("Minigame_Balls")
  
  InitGame()
  
  WindowManager.OpenWindow("Minigame_Balls") 
end
