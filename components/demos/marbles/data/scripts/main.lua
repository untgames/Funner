function OnWindowClose ()
  System.Application.Exit (0) 
end

function Init()
  mYeR=true

  screen = Render.Screen.Create ()
  Engine.Screens.Register ("Screen1", screen)
  
  dofile("data/scripts/Common/_Include.lua")
  dofile("data/scripts/UI/_Include.lua")
  
  LoadGameSettings()
  
  dofile("data/scripts/GameLogic/_InitGameLogic.lua")
  InitGameLogic()
  
  CursorSprite:Unbind ()

--  HardwareAudioPlayer.PlaySound ("data/sounds/music/music.aac", true)

  --MusicEmitter=PlaySound ("Music")
end
