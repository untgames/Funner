-- Таблица атрибутов

local meta_attributes_internal =
{
  __mode = "kv"
}

local attributes_table = {}

setmetatable (attributes_table, attributes_meta_table)

-- Создание атрибута
local function CreateAttribute (...)
  local args     = { ... }
  local is_empty = next (args) == nil
  
  local attribute = {}

  local attribute_meta_table =
  {
    -- Установка атрибута для указанного значения
    __call = function (attribute, name, value)
      if value == nil then value = true end
    
      attribute [name] = value
      
      return name
    end,
    
    __newindex = function (attribute, name, value)
      local entry = attributes_table [name]
      
      if not entry then
        entry = {}
        
        attributes_table [name] = entry
      end
      
      if is_empty then
        entry [attribute] = value
      else
        for _, attribute in pairs (args) do
          attribute [name] = value
        end
      end
    end,
    
    __index = function (attribute, name)
      local entry = attributes_table [name]
      
      if not entry then return nil end
      
      if is_empty then
        return entry [attribute]
      else
        local result = {}

        for _, attribute in pairs (args) do
          result [attribute] = attribute [name]
        end

        return result
      end
    end,    
  }  

  -- Проверка наличия атрибута для указанного значения  
  attribute.Check = function (value)
    local entry = attributes_table [value]

    if not entry then return false end

    if is_empty then
      return entry [attribute] ~= nil
    else
      for _, attribute in pairs (args) do
        if not attribute.Check (value) then return false end          
      end

      return true
    end
  end

  setmetatable (attribute, attribute_meta_table)  
  
  return attribute
end

-- Получение атрибутов
local function GetAttributes (value)
  local entry = attributes_table [value]
  
  if not entry then
    return nil
  end      

  return pairs (entry)
end

-- Переопределение имён
attribute     = CreateAttribute
attributesof  = GetAttributes
