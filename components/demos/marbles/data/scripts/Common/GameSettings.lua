local GameSettings={}
local GameSettingsByIndex={}

function LoadGameSettings()
  print("LoadGameSettings()")
  local xml=CreateXML()
  -- попыта загрузить файл с настройками
  if not(xml.Load("data/GameSettings.xml")) then
    -- загружаем файл настроек по умолчанию
    xml.Load("data/Default_GameSettings.xml")
  end
  
  local index=1
  local function LoadSetting(xml)
    --загрузка отдельной настройки
    local setting={}
    setting.Name=xml.ReadStringByName("Name")
    setting.Type=xml.ReadStringByName("Type")
    if setting.Type=="Number" then
      setting.Value=xml.ReadNumberByName("Value")
    else
      setting.Value=xml.ReadStringByName("Value")
    end
    GameSettings[setting.Name]=setting
    GameSettingsByIndex[index]=setting
    index=index+1
  end
  
  xml.ForEachChildByName("Setting", LoadSetting)
end

function SaveGameSettings()
  print("SaveGameSettings() - saving..")
  local xml=CreateXML()
  xml.SetName("GameSettings")
  local function SaveSetting(setting)
    local sxml=CreateXML()
    sxml.SetName("Setting")
    sxml.WriteStringWithName("Name", setting.Name)
    sxml.WriteStringWithName("Value", setting.Value)
    sxml.WriteStringWithName("Type", setting.Type)
    xml.AddChild(sxml)
  end
  ForEachInTable(GameSettingsByIndex, SaveSetting) 
  xml.Save("data/GameSettings.xml")
end

function GetGameSetting(name)
  assert(name,"ERROR: GetGameSetting(name) - name is nil")
  assert(GameSettings[name], "ERROR: GetGameSetting("..name..") - no such setting")
  return GameSettings[name].Value
end

function SetGameSetting(name, value)
  assert(name,"ERROR: SetGameSetting(name, value) - name is nil")
  assert(GameSettings[name], "ERROR: SetGameSetting("..name..") - no such setting")
  assert(value,"ERROR: SetGameSetting(name, value) - value is nil")
  GameSettings[name].Value=value
end


