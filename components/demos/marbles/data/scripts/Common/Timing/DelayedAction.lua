
function DelayedAction(CallDelay, func, ...)
  assert(func, "ERROR: DelayedAction(CallDelay, func, ...) - func is nil")
  assert(CallDelay,"ERROR: DelayedAction(CallDelay, func, ...) - delay is nil")
  
  func=ConvertToFunction(func)
  
  local Action={}
  Action.Params={...}
  Action.DataBank={}
  
  local Params=Action.Params 
  local DataBank=Action.DataBank
  
  Action.Stop=function()
    --эта функция позволит отменить отложенное действие еще до его выполнения
    Action.connection:Disconnect()
    TempGlobals[Action]=nil --это позволит сборщику мусора сделать свое дело :)
  end
  
  local function callback()
    func(unpack(Params))
    Action.Stop()
  end
  
  Action.EventHandler=Common.ActionQueue.CreateEventHandler(callback)
  Action.ActionQueue=Common.ActionQueue.Create()
  Action.connection = Action.ActionQueue:RegisterEventHandler(CallDelay, 1, Action.EventHandler)
  TempGlobals[Action]=Action -- хак сборщика мусора
  return Action
end



