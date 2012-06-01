--[[ 
  в этом файле собраны все стандартные функции,
  которые используются для работы с XML
]]

local BooleanByString={}
      BooleanByString["0"]=false
      BooleanByString["false"]=false
      BooleanByString["False"]=false
      BooleanByString["1"]=true
      BooleanByString["true"]=true
      BooleanByString["True"]=true

--------------------------------------------------------------------------------------------------
-- сохранение и загрузка

function XML_Save(xml, FileName)
  assert(xml, "ERROR: XML_Save(xml, FileName) - xml is nil")
  assert(FileName~=nil, "ERROR: XML_Save(xml, FileName) - FileName is nil")
  xml.Node:SaveXml(FileName)
end

function XML_Load(xml, FileName)
  assert(xml, "ERROR: XML_Load(xml, FileName) - xml is nil")
  assert(FileName, "ERROR: XML_Load(xml, FileName) - FileName is nil")
  assert(type(FileName)=="string", "ERROR: XML_Load(xml, FileName) - FileName is not a string")
  if not(Common.File.IsFileExist(FileName)) then
    return false
  end
  xml.Node=Common.StringNode.LoadXml(tostring(FileName))
  return true
end

--------------------------------------------------------------------------------------------------
--Установка имени

function XML_SetName(xml, name)
  assert(name~=nil, "ERROR: XML_SetName(name) - name is nil")
  xml.Node.Name=name
end

function  XML_GetName(xml)
  return xml.Node.Name
end

--------------------------------------------------------------------------------------------------
-- Запись данных

function XML_WriteStringWithName(xml, name, data)
  assert(xml, "ERROR: XML_WriteStringWithName(xml, name, data) - xml is nil")
  assert(name, "ERROR: XML_WriteStringWithName(xml, name, data) - name is nil")
  if data then
    local child=Common.StringNode.Create()
    child.Name=name
    if type(data)~="string" then
      data=tostring(data)
    end
    child:AddAttribute(data)
    xml.Node:AddChild(child)
  end
end


function XML_WriteStringsWithNames(xml,...)
  assert(xml,"ERROR: XML_WriteStringsWithNames(xml,...) - xml is nil")
  local params={...}
  local child=nil
  local data=nil
  local index, name = next(params)
  while index do
    index, data = next(params, index)
    XML_WriteStringWithName (xml, name, data)
    index, name = next(params, index)
  end
end


function XML_WriteBooleanWithName(xml, name, data)
  assert(xml,"ERROR: XML_WriteBooleanWithName(xml, name, data) - xml is nil")
  assert(name,"ERROR: XML_WriteBooleanWithName(xml, name, data) - name is nil")
  if data then
    data="1"
  else
    data="0"
  end
  XML_WriteStringWithName(xml, name, data)
end


function XML_WriteNumbersWithName(xml, name, ...)
  assert(xml, "ERROR: XML_WriteNumbersWithName(xml, name, ...) - xml is nil")
  assert(name, "ERROR: XML_WriteNumbersWithName(xml, name, ...) - name is nil")
  XML_WriteStringWithName (xml, name, PackString(" ", ...))
end


function XML_WriteVec3WithName(xml, name, data)
  assert(xml, "ERROR: XML_WriteVec3WithName(xml, name, data) - xml is nil")
  assert(name, "ERROR: XML_WriteVec3WithName(xml, name, data) - name is nil")
  assert(data, "ERROR: XML_WriteVec3WithName(xml, "..name..", data) - data is nil")
  XML_WriteNumbersWithName(xml, name, data.x, data.y, data.z)
end

--------------------------------------------------------------------------------------------------
-- Чтение данных

function XML_ReadStringByName (xml, name)
  assert(xml,"ERROR: XML_ReadStringByName (xml, name) - xml is nil")
  assert(name,"ERROR: XML_ReadStringByName (xml, name) - name is nil")
  local data=xml.Node:Find(name)
  if data==nil then 
    return nil
  end
  local str=data:Attribute(0)
  if LocaleXML then
    --если обьявлена XML с локализацией - то пробуем
    if string.sub(str,1,1)=="#" then
      str=LocaleXML.ReadStringByName(string.sub(str,2))
    end
  end
  return str
end


function  XML_ReadStringsByNames (xml, ...)
  assert(xml,"ERROR: XML_ReadStringsByNames (xml, ...) - xml is nil")
  local params={...}
  local results={}
  local ResultNumber=0
  local data=nil
  local index, name = next(params)
  while index do
    data=XML_ReadStringByName(xml, name)
    assert(data~=nil, "ERROR: XML_ReadStringsByNames (xml, ...) - no such string:"..name)
    ResultNumber=ResultNumber+1
    results[ResultNumber]=data
    index, name = next(params, index)
  end
  return unpack(results)
end


function  XML_ReadBooleanByName (xml, name, default)
  assert(xml, "ERROR: XML_ReadBooleanByName (xml, name, default) - xml is nil")
  assert(name,"ERROR: XML_ReadBooleanByName (xml, name, default) - name is nil")
  data=XML_ReadStringByName(xml, name)
  if data then 
    return BooleanByString[data]
  end
  if default==nil then
    return false
  else
    return default
  end
end


function XML_ReadNumberByName (xml, name)
  assert(xml, "ERROR: XML_ReadNumberByName (xml, name) - xml is nil")
  assert(name,"ERROR: XML_ReadNumberByName (xml, name) - name is nil")
  data=XML_ReadStringByName(xml, name)
  if data then 
    data=tonumber(data) 
  end
  return data
end


function XML_ReadNumbersByNames (xml, ...)
  assert(xml, "ERROR: XML_ReadNumbersByNames (xml, ...) - xml is nil")
  local params={...}
  local results={}
  local ResultNumber=0
  local data=nil
  local index, name = next(params)
  while index do
    data=XML_ReadNumberByName (xml, name)
    ResultNumber=ResultNumber+1
    results[ResultNumber]=data
    index, name = next(params, index)
  end
  return unpack(results)
end


function XML_ReadNumbersByName(xml, name)
  assert(xml, "ERROR: XML_ReadNumbersByName(xml, name) - xml is nil")
  assert(name, "ERROR: XML_ReadNumbersByName(xml, name) - name is nil")
  return ToNumbers( UnpackString( XML_ReadStringByName(xml, name), " "))
end


function XML_ReadVec3ByName(xml, name)
  assert(xml, "ERROR: XML_ReadVec3ByName(xml, name) - xml is nil")
  assert(name, "ERROR: XML_ReadVec3ByName(xml, name) - name is nil")
  local str=XML_ReadStringByName(xml, name)
  if str then
    return ConvertStringToVec3(str)
  end
  return nil
end

--------------------------------------------------------------------------------------------------
-- работа с потомками

local function XML_CreateFromNode (node)
  assert(node,"ERROR: XML_CreateFromNode (node) - node is nil")
  assert(CreateXML, "ERROR: XML_CreateFromNode (node) - CreateXML is nil")
  local NewXml=CreateXML()
  NewXml.Node=node
  return NewXml
end

function XML_AddChild (xml, child)
  assert(xml, "ERROR: XML_AddChild(xml, child) - xml is nil")
  if child then
    xml.Node:AddChild(child.Node)
  end
end

local function XML_ForEachChildNode (xml, func, ...)
  assert(xml, "ERROR: XML_ForEachChildNode (xml, func, ...) - xml is nil")
  assert(func, "ERROR: XML_ForEachChildNode (xml, func, ...) - func is nil")
  local parent=xml.Node
  local i=0
  local i_max=parent.ChildrenCount
  while (i<i_max) do
    local child=parent:Child(i)
    if func(child, ...)==false then
      return
    end
    i=i+1
  end
end

function XML_GetChild (xml, name, index)
  --внимание!! индекс начинается с 1, а не с 0 !!!!!
  assert(xml, "ERROR: XML_GetChild( xml, name, index) - xml is nil")
  assert(name, "ERROR: XML_GetChild( xml, name, index) - name is nil")
  if index then
    --выбор одного из нескольких одноименных потомков с помощью индекса
    local found=nil
    XML_ForEachChildNode(xml, function(child)
      if child.Name==name then
        index=index-1
        if index<1 then
          found=XML_CreateFromNode(child)
          return false
        end
      end
    end)
    return found
  else
    local child=xml.Node:Find(name)
    if child then
      return XML_CreateFromNode(child)
    end
  end
  return nil
end

function XML_ForEachChild (xml, func, ...)
  --func(child, ...) срабатывает для каждого child
  assert(xml, "ERROR: XML_ForEachChild (xml, func, ...) - xml is nil")
  assert(func, "ERROR: XML_ForEachChild (xml, func, ...) - func is nil")
  assert(type(func)=="function", "ERROR: XML_ForEachChild (xml, func, ...) - func is not a function")
  --кеширование порождающей функции
  local NodeToXML=XML_CreateFromNode
  --обработка всех потомков
  local params={...}
  XML_ForEachChildNode(xml, function(child)
    func (NodeToXML(child), unpack(params))
  end)
end

function XML_ForEachChildByName (xml, name, func, ...)
  assert(xml, "ERROR: XML_ForEachChildByName (xml, name, func, ...) - xml is nil")
  assert(name,"ERROR: XML_ForEachChildByName (xml, name, func, ...) - name is nil")
  assert(func,"ERROR: XML_ForEachChildByName (xml, name, func, ...) - func is nil")
  --кеширование порождающей функции
  local NodeToXML=XML_CreateFromNode
  --обработка всех потомков
  local params={...}
  XML_ForEachChildNode(xml, function(child)
    if child.Name==name then
      func(NodeToXML(child), unpack(params))
    end
  end)
end




