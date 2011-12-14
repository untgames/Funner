-- Атрибут функторов, вызываемых при конструировании класса
local class_initializer = attribute ()

-- Получение таблицы реализации класса
local function GetClassImpl (class)
  local impl = rawget (class, "impl")
  
  if not impl then
    error (string.format ("No implementation handler found in class %s"), tostring (class))
  end
  
  return impl
end

-- Установка свойства
local function SetProperty (object, name, value)
  local set_handler = object [string.format ("set_%s", name)]
  
  if not set_handler then return false end
  
  set_handler (name, value)

  return true
end

-- Чтение свойства
local function GetProperty (object, name, result)
  local get_handler = object [string.format ("get_%s", name)]

  if not get_handler then return false end

  result.value = get_handler (name)

  return true
end

-- Установка члена класса
local function SetClassField (class, name, value)
  local impl   = GetClassImpl (class)
  local member = impl [name]  

  if member then
    impl [name] = value
    return true
  end
  
  if SetProperty (impl, name, value) then
    return true
  end
  
  local parents = rawget (class, "parents") or {}  
  
  for _, parent in pairs (parents) do  
    if SetClassField (parent, name, value) then
      return true
    end
  end

  return false
end

-- Получение члена класса
local function GetClassField (class, name, result)
  local impl   = GetClassImpl (class)
  local member = impl [name]

  if member then
    result.value = member
    return true
  end  

  if GetProperty (impl, name, result) then
    return true
  end

  local parents = rawget (class, "parents") or {}
  
  for _, parent in pairs (parents) do
    if GetClassField (parent, name, result) then
      return true
    end
  end

  return false
end

-- Мета-таблица класса
local class_meta_table =
{
  -- Установка члена класса 
  __newindex = function (class, name, value)
    if not SetClassField (class, name, value) then
      error (string.format ("Can't set value: member '%s.%s' not found", tostring (class), name))    
    end
  end,
  
  -- Получение члена класса
  __index = function (class, name)
    local result = {}
  
    if not GetClassField (class, name, result) then
      error (string.format ("Can't get value: member '%s.%s' not found", tostring (class), name))
    end

    return result.value
  end,
  
  -- Преобразование в строку
  __tostring = function (class)
    return rawget (class, "name")
  end,
}

-- Создание класса или объекта
local function CreateClassInstance (class_name, impl, parent)
  -- Создание класса

  local class = {}  
  
  class.impl = impl  
  class.name = class_name

  -- Установка мета-таблицы
  
  setmetatable (class, class_meta_table)

  -- Инициализация класса    

  for name, value in pairs (impl) do
    local initializer = class_initializer [value]
   
    if initializer then
      if name == value then
        value = initializer (class, value)
        
        impl [value] = value
      else
        impl [name] = initializer (class, value)
      end
    end
  end  
  
  Runtime.Attribute.Class (class)
  
  return class
end

-- Счётчик анонимных классов
local anonymous_classes_number = 0

-- Регистрация класса
local function KeywordClass (param)
  if type (param) == "string" then
    local class_name = param

    local function Create (decl)
      return CreateClassInstance (class_name, decl)
    end
    
    return Create
  elseif type (param) == "table" then
    anonymous_classes_number = anonymous_classes_number + 1
  
    return CreateClassInstance (string.format ("Class%u", anonymous_classes_number), param)
  else
   error (string.format ("Bad class parameter '%s'", tostring (param)))
  end
end

-- Реализация наследования
local function KeywordSuperClass (...)
  local parents = { ... }

  if not next (parents) then
    error ("Empty super classes list")
  end    
  
  local function RegisterSuperClass (class)
    local registered_parents = rawget (class, "parents")

    if not registered_parents then
      registered_parents = {}

      rawset (class, "parents", registered_parents)
    end

    for _, parent in pairs (parents) do
      table.insert (registered_parents, parent)
    end
  end  
  
  class_initializer [RegisterSuperClass] = RegisterSuperClass

  return RegisterSuperClass
end

-- Реализация конструктора
local function KeywordConstructor (fn)
  local function CreateConstructor (class, fn)
    local function Constructor (...)
      local instance = fn (...)

      if not instance then return nil end

      local registered_parents = rawget (instance, "parents")

      if not registered_parents then
        registered_parents = {}

        rawset (instance, "parents", registered_parents)
      end

      table.insert (registered_parents, class)
      
      rawset (instance, "name", tostring (class))

      return instance
    end      

    return Constructor
  end

  class_initializer [fn] = CreateConstructor
  
  return fn
end

-- Реализация перебора полей класса
local function KeywordMembersOf (class)
  local result = {}
  
  local function GetMembers (class)
    for member, value in pairs (rawget (class, "impl")) do
      if not result [member] then
        result [member] = value
      end
    end
    
    local parents = rawget (class, "parents") or {}
    
    for _, parent in pairs (parents) do
      GetMembers (parent)
    end
  end
  
  GetMembers (class)
  
  return pairs (result)
end

-- Переопределения имён
class       = KeywordClass
extend      = KeywordSuperClass
constructor = KeywordConstructor
membersof   = KeywordMembersOf
