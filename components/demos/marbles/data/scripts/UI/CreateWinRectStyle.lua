function CreateWinRectStyle (Color, Material)
  -- используется для создания прямоугольников кнопок или окошек

  local style={}

  style.Color=Color  
  style.Material=Material
  
  style.GetAsXML=function ()
    assert(CreateXML~=nil, "ERROR: CreateWinRectStyle() - .GetAsXML() - CreateXML() is nil")
    local xml=CreateXML()
    xml.WriteStringWithName("Material", style.Material)
    local c=style.Color
    xml.WriteNumbersWithName("Color", c.x, c.y, c.z, c.w)
    return xml
  end
  
  style.SetByXML=function (xml)
    assert(CreateXML~=nil, "ERROR: CreateWinRectStyle() - .SetAsXML(xml) - CreateXML() is nil")
    assert(xml~=nil, "ERROR: CreateWinRectStyle() - .SetAsXML(xml) - xml is nil")
    local r,g,b,a=xml.ReadNumbersByName("Color")
    style.Color=vec4(r,g,b,a)
    style.Material=xml.ReadStringByName("Material")
  end
  
  style.Copy=function ()
    return CreateWinRectStyle (style.Color, style.Material)
  end
  return style
end
