-- Предварительные объявления
local ReadFromStringNode

-- Преобразователи
local DefaultParsers =
{
  ["number"] = function (node, context)
    return tonumber (node:Attribute (0))
  end,
  
  ["string"] = function (node, context)
    return node:Attribute (0)
  end,
  
  ["function"] = function (node, context)
    error ("Function parser not yet implemented")
  end,
  
  ["class"] = function (node, context)
    return ReadFromStringNode (node, context)
  end
}

-- Преобразователь класса по умолчанию
local DefaultClassConverter =
{
--Загрузка из дерева строк
  FromStringTree = function (node, type_name, context)
    local class    = context [type_name]
    local instance = class.Create ()
    local members  = {}
    
    for name, member in membersof (class) do
      members [name] = member
    end
    
    local count = node.ChildrenCount

    for i=0,count-1 do 
      local subnode = node:Child (i)
      local name    = subnode.Name
      local member  = members [name]
      
      if not member then
        member = members [string.format ("set_%s", name)]
        
        if not member then
          error (string.format ("Error at parse '%s.%s'. Class has not this member", type_name, name))
        end
      end
      
      local type = Runtime.Attribute.Type [member]
      local parser
      
      if type then
        parser = DefaultParsers [type]
      end

      if not parser then
        error (string.format ("Error at parse '%s.%s'. No parser found for type '%s'", type_name, name, type))
      end
      
      instance [name] = parser (subnode, context)
    end
    
    return instance
  end
}

-- Чтение XML дерева
local function ReadFromStringNode (node, context)
  if not context then context = _G end

  -- Определение типа элемента

  local tag_name  = node.Name
  local type_name = tag_name
  
  -- Определение конвертера
  
  local converter = Runtime.Attribute.StringTreeConverter [type_name]
  
  if not converter then
    if Runtime.Attribute.Type [context [type_name]] == "class" then
      converter = DefaultClassConverter
    else
      error (string.format ("Type '%s' has no converter", type_name))
    end
  end
  
  local creator = converter.FromStringTree
  
  if not creator then
    error (string.format ("Type '%s' converter has not FromStringTree method", type_name))
  end

  local instance = creator (node, type_name, context)
  
  return instance
end

-- Загрузка из XML
local function ReadFromXml (name, context)
  local node = Common.StringNode.LoadXml (name)
  
  return ReadFromStringNode (node, context)
end

-- Работа с сериализацией
local function CreateSerializeClass ()
  return class "Serialize"
  {
    FromStringNode = ReadFromStringNode,
    FromXml        = ReadFromXml
  }
end

Runtime = Runtime or {}

Runtime.Serialize = CreateSerializeClass ()
