function AddEventManager(obj)
  assert(obj~=nil, "ERROR: AddEventManager - object is nil")
  assert(type(obj)=="table", "ERROR: AddEventManager - object is not a metatable")
  
  local Handle={} --список списков обработчиков для различных событий
  local UHandle={} --список универсальных обработчиков
  local AdditionalParams={}
  
  obj.RegisterUniversalHandler=function (handler, Param)
    assert(handler~=nil, "ERROR: AddEventManager(obj).RegisterUniversalHandler - handler is nil")
    handler=ConvertToFunction(handler)
    UHandle[handler]=handler
    AdditionalParams[handler]=Param
  end
  
  obj.UnregisterUniversalHandler=function (handler)
    assert(handler~=nil, "ERROR: AddEventManager(obj).UnregisterUniversalHandler - handler is nil")
    UHandle[handler]=nil
    AdditionalParams[handler]=nil
  end
  
  obj.RegisterEventHandler=function (event, handler, Param)
    assert(event~=nil, "ERROR: AddEventManager(obj).RegisterEventHandler - event is nil")
    assert(handler~=nil, "ERROR: AddEventManager(obj).RegisterEventHandler - handler is nil")
    handler=ConvertToFunction(handler)
    if Handle[event]==nil then
      Handle[event]={}
    end
    Handle[event][handler]=handler
    AdditionalParams[handler]=Param
  end
  
  obj.UnregisterEventHandler=function (event, handler)
    assert(event~=nil, "ERROR: AddEventManager(obj).UnregisterEventHandler - event is nil")
    assert(handler~=nil, "ERROR: AddEventManager(obj).UnregisterEventHandler - handler is nil")
    if Handle[event] then
      Handle[event][handler]=nil
      AdditionalParams[handler]=nil
    end
  end

  
  obj.HandleEvent=function (event, ...)
    --print("Event:",event,...)
    assert(event~=nil, "ERROR AddEventManager(obj).HandleEvent - event is nil")
    --сохраняем в обьект последнее поступившее событие
    local LastEvent={}
    LastEvent.Name=event
    LastEvent.Params={...}
    obj.LastEvent=LastEvent
    --основной обработчик событий, вызывающий конкретный обработчик события
    local AP=false
    if Handle[event] then
      --вызов обработчиков для конкретного события
      local handlers=Handle[event]
      local handler_index, handler=next(handlers)
      while handler_index do
        AP=AdditionalParams[handler]
        if AP then
          handler(AP, ...)
        else
          handler(...)
        end
        handler_index, handler=next(handlers, handler_index)
      end
    end
    --вызов универсальных обработчиков
    local handlers=UHandle
    local handler_index, handler=next(handlers)
    while handler_index do
      AP=AdditionalParams[handler]
      if AP then
        handler(AP, event, ...)
      else
        handler(event, ...)
      end
      handler_index, handler=next(handlers, handler_index)
    end
  end
  
  return obj
end
