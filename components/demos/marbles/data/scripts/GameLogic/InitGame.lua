local function CreatePlayer()
  local Player={}
    Player.ManaBottles=0
  
  --пополнение запасов маны
  Player.Mana=0 --изменяется от 0 до 1

  Player.SetMana=function(Value)
    Player.Mana=Value
    if Player.Mana>1 then
      Player.Mana=1
    end
    if Player.Mana<0 then
      Player.Mana=0
    end
    WindowManager.HandleEventByAllOpenedWindows("PlayerManaChanged")
  end
  
  Player.ManaReplenishment=StartPeriodicCall(1,function ()
  end)
  
  Player.ManaBottles=0
  
  Player.SetManaBottlesNumber=function(Value)
    if Value<0 then 
      Value=0
    end
    Player.ManaBottles=Value
    WindowManager.HandleEventByAllOpenedWindows("PlayerManaBottlesChanged")
  end
  
  Player.UseManaBottle=function()
    local Player=Game.Player
    if Player.ManaBottles>0 then
      local location=Game.CurrentLocation
      if Player.Mana<0.99 then
        Player.SetManaBottlesNumber(Player.ManaBottles-1)
        Player.SetMana(1.1)
        location.HandleEvent("ManaBottleUsed")
      end
    end
  end
  
  Player.UseHintMana=function()
    local location=Game.CurrentLocation
    local Player=Game.Player
    if Player.Mana>0.99 then
      Player.SetMana(0)
      ShowHintInCurrentLocation()
      location.HandleEvent("HintManaUsed")
    end
  end
  
  return Player
end


--========================================================================================
--========================================================================================

function InitGame()
  -- глобальные переменные
  Game={}
  Game.Player=CreatePlayer()
  Game.Locations={}
  
  
  ---------------------------------------------------------------------------------------
  -- приостановка игры
  Game.Pused=false
  
  function Game.Pause()
    Game.Player.ManaReplenishment.Pause()
    Game.Pused=true
  end
  
  function Game.Resume()
    Game.Player.ManaReplenishment.Resume()
    Game.Pused=false
  end
  
  function Game.IsPaused()
    return Game.Pused
  end
  
  ----------------------------------------------------------------------------------------
  -- загрузка локации
  Game.CloseCurrentLocation=function ()
    assert(false, "ERROR: Game.CloseCurrentLocation()")
  end
  
  Game.LoadLocation=function(Name)
    assert(false, "ERROR: Game.LoadLocation()")
  end
  
  --вначале игра находится на паузе
  Game.Pause()
end

