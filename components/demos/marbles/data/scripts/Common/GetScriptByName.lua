
function GetScriptByName(ScriptStore, name, UseWrapper)
  assert(name, "ERROR: GetScriptByName(ScriptStore, name, UseWrapper) - name is nil")
  assert(type(ScriptStore)=="table", "ERROR: GetScriptByName(ScriptStore, name, UseWrapper) - ScriptStore is not a table")
  local script=ScriptStore[name]
  --оболочка скрипта позвол€ет получать и использовать скрипт в до того как он был объ€влен в массиве скриптов, не вызыва€ ошибок в программе
  local function ScriptWrapper(...)
    if script then
      script(...)
    else
      --если скрипт до сих пор не был получен - нова€ попытка получить его
      script=ScriptStore[name]
      if script then
        script(...)
      else
        --если скрипт не получилось вызвать - выводим сообщение о том, что такой скрипт не существует
        myprint("SCRIPT WARNING: Unknown script: ", name)
      end
    end
  end
  if UseWrapper then
    return ScriptWrapper
  else
    if script then
      --возвращатс€ уже обь€вленный скрипт без оболочки
      return script
    else
      --если скрипт не обь€влен - приходитс€ использовать оболочку
      myprint("SCRIPT WARNING: ", name," - used ScriptWrapper!")
      return ScriptWrapper
    end
  end
end
