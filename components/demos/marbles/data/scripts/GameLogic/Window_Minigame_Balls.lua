local GameResult=0

function Minigame_Balls_GameOver(ManaBottles)
  UIControlScript["ShutdownBallsMinigame"] ()
  UIControlScript["RunBallsMinigame"] ()
end

UIControlScript["Show minigame balls tutorial"]=function()
  WindowManager.OpenWindow("Minigame_Balls_Tutorial")
end

UIControlScript["Close minigame balls tutorial"]=function()
  WindowManager.CloseWindow("Minigame_Balls_Tutorial")
end

UIControlScript["Balls - try exit"]=function()
  WindowManager.OpenWindow("Balls_GameOver")
end

UIControlScript["Balls - update mana bottles"]=function()
end

UIControlScript["Balls - close window"] = function ()
  WindowManager.CloseWindow("Balls_GameOver")
  WindowManager.CloseWindow("Balls_GameOver_Replay")
  WindowManager.CloseWindow("Minigame_Balls")
end

UIControlScript["Balls - start new game"] = function ()
  WindowManager.CloseWindow("Balls_GameOver_Replay")
  if not(GameResult==0) then 
    DelayedAction(1, UIControlScript["RunBallsMinigame"])
  end
end

UIControlScript["Balls - lose game"]=function()
  local Player=Game.Player
  Player.ManaBottles=Player.ManaBottles-1
  if Player.ManaBottles<0 then
    Player.ManaBottles=0
  end
  UIControlScript["ShutdownBallsMinigame"]()
  UIControlScript["Balls - close window"] ()
end

UIControlScript["Balls - resume game"]=function()
  WindowManager.CloseWindow("Balls_GameOver")
end
