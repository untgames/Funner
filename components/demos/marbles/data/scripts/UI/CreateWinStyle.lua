---------------------------------------------------------------------------------------------
-- методы стиля окна
---------------------------------------------------------------------------------------------

local function GetStyleAsXML (style)
  assert(style, "ERROR: GetStyleAsXML (style) - style is nil")
  assert(CreateXML, "ERROR: CreateWinStyle () - .GetAsXML() - CreateXML() is nil")
  local xml=CreateXML()
  xml.SetName("Style")
  xml.WriteStringWithName("Name", style.Name)
  if style.Sound then xml.WriteStringWithName("Sound", style.Sound) end
  if style.Rect then 
    local rect=style.Rect.GetAsXML()
    rect.SetName("Rect")
    xml.AddChild(rect)
  end
  if style.Text then
    local text=style.Text.GetAsXML()
    text.SetName("Text")
    xml.AddChild(text)
  end
  return xml
end

local function SetStyleByXML (style, xml)
  assert(style, "ERROR: SetStyleByXML (style, xml) - style is nil")
  assert(xml, "ERROR: CreateWinStyle() - .SetAsXML(xml) - xml is nil")
  local rect_xml=xml.GetChild("Rect")
      if rect_xml then 
        if not(style.Rect) then
          style.Rect=CreateWinRectStyle()
        end
        style.Rect.SetByXML(rect_xml)
      end
  local text_xml=xml.GetChild("Text")
      if text_xml then
        if not(style.Text) then
          style.Text=CreateTextStyle()
        end
        style.Text.SetByXML(text_xml)
      end
  style.Name=xml.ReadStringByName("Name")
  style.Sound=xml.ReadStringByName("Sound")
end

---------------------------------------------------------------------------------------------
-- создание стиля окна
---------------------------------------------------------------------------------------------

function CreateWinStyle (MakeDefault)

  local style={}
  style.Name="Default"
  
  if MakeDefault then
    style.Rect=CreateWinRectStyle()
    style.Text=CreateTextStyle()
  end
  
  style.GetAsXML=function ()
    return GetStyleAsXML (style)
  end
  
  style.SetByXML=function (xml)
    SetStyleByXML (style, xml)
  end
  
  return style
end
