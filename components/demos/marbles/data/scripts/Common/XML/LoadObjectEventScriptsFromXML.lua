--[[
  используется для загрузки скриптов обработки событий из XML файла
  и регистрации этих обработчиков в обьекте
]]


function LoadObjectEventScriptsFromXML (obj, Scripts, xml, Events, Params)
  --проверка входных параметров
  AssertionPrefix("ERROR: LoadObjectEventScriptsFromXML(obj, Scripts, xml, Events, Params)")
    assert(obj, " - obj is nil")
    assert(obj.RegisterEventHandler, " - obj has no event manager")
    assert(type(Scripts)=="table", " - Scripts is not a table")
    assert(xml, " - xml is nil")
    assert(Events," - Events is nil")-- таблица с названиями событий, для которых нужно загрузить скрипты
  AssertionPrefix()
  
  -- регистрация скрипта события
  local function RegisterEventScript(EventScriptName, Event)
    --print("Registering event script: ", EventScriptName)
    obj.RegisterEventHandler(Event, GetScriptByName(Scripts, EventScriptName), Params)
  end
   
  -- загрузка скриптов обработки события
  local function TryLoadEventScript(Event)
    assert(Event,"ERROR: TryLoadEventScript(Event) - name is nil")
    local EventScriptNames=xml.ReadStringByName("OnEvent_"..tostring(Event))
    if EventScriptNames then
      EventScriptNames=UnpackString(EventScriptNames,", ", true)
      ForEachInTable (EventScriptNames, RegisterEventScript, Event)
    end
  end
   
  --загрузка скриптов для всех указанных событий
  ForEachInTable(Events, TryLoadEventScript)
end
