

function CreateXML(filename)
  --имя файла указывается если нужно загрузить xml из файла
  local xml={}
  
  xml.Node=Common.StringNode.Create()
  
  --------------------------------------------------------------------------------------------------
  -- сохранение и загрузка
  
  xml.Save=CreateMethodForObject(XML_Save, xml)
  
  xml.Load=CreateMethodForObject(XML_Load, xml)
    
  --------------------------------------------------------------------------------------------------
  --Установка имени
  xml.SetName=CreateMethodForObject(XML_SetName, xml)
   
  xml.GetName=CreateMethodForObject(XML_GetName, xml)
  
  --------------------------------------------------------------------------------------------------
  -- Запись данных
  xml.WriteStringWithName=CreateMethodForObject(XML_WriteStringWithName, xml)
  
  xml.TryWriteStringWithName=CreateMethodForObject(XML_WriteStringWithName, xml)
    
  xml.WriteStringsWithNames=CreateMethodForObject(XML_WriteStringsWithNames, xml)
  
  xml.WriteBooleanWithName=CreateMethodForObject(XML_WriteBooleanWithName, xml)
  
  xml.TryWriteBooleanWithName=CreateMethodForObject(XML_WriteBooleanWithName, xml)
  
  xml.WriteNumbersWithName=CreateMethodForObject(XML_WriteNumbersWithName, xml)
  
  xml.WriteVec3WithName=CreateMethodForObject(XML_WriteVec3WithName, xml)
  
  --------------------------------------------------------------------------------------------------
  -- Чтение данных
  xml.ReadStringByName=CreateMethodForObject(XML_ReadStringByName, xml)
    
  xml.ReadStringsByNames=CreateMethodForObject(XML_ReadStringsByNames, xml)
  
  xml.ReadBooleanByName=CreateMethodForObject(XML_ReadBooleanByName, xml)

  xml.ReadNumberByName=CreateMethodForObject(XML_ReadNumberByName, xml)
  
  xml.ReadNumbersByNames=CreateMethodForObject(XML_ReadNumbersByNames, xml)
  
  xml.ReadNumbersByName=CreateMethodForObject(XML_ReadNumbersByName, xml)
    
  xml.ReadVec3ByName=CreateMethodForObject(XML_ReadVec3ByName, xml)
  
  xml.TryReadVec3ByName=CreateMethodForObject(XML_ReadVec3ByName, xml)
  
  --------------------------------------------------------------------------------------------------
  -- работа с потомками
  
  xml.AddChild=CreateMethodForObject(XML_AddChild, xml)
  
  xml.GetChild=CreateMethodForObject(XML_GetChild, xml)
  
  xml.ForEachChild=CreateMethodForObject(XML_ForEachChild, xml)
  
  xml.ForEachChildByName=CreateMethodForObject(XML_ForEachChildByName, xml)

  --------------------------------------------------------------------------------------------------
  -- Инициализация
  --------------------------------------------------------------------------------------------------
  xml.SetName("noname")
  
  --если было указано имя файла - загрузить xml
  if filename then
    xml.Load(filename)
  end
  
  return xml
end

