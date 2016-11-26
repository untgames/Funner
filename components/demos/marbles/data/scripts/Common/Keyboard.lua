--[[
создает глобальный объект "Keyboard"


Доступные методы: ---------------------------------------------------------------

Keyboard.RegisterEventHandler(event, handler) - регистрация обработчика событий

Keyboard.UnregisterEventHandler(event, handler) - отмена регистрации

]]


function InitKeyboard()
  Keyboard={}
    
  --------------------------------------------------------------------------------------------------
  -- обработка событий
  --------------------------------------------------------------------------------------------------
  
  local handler=Input.CreateEventHandler()
  Engine.InputEventHandlers.Register("Input", handler)
  
  assert(AddEventManager~=nil, "ERROR: InitKeyboard() - AddEventManager() is nil")
  AddEventManager(Keyboard)
  
end

