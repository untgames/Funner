
function CreateTextStyle (Color, Scale, Font)
  -- используется для оформления кнопок или окошек интерфейса, но можно еще для чего-нибудь

  local style={}
  if Color==nil then Color=vec4(1,1,1,1) end
  if Scale==nil then Scale=1 end
  if Font==nil then Font="data/fonts/stylo.xfont" end
  style.Color=Color 
  style.Scale=Scale
  style.Font=Font
  
  style.ApplyToTextLine=function (TextLine)
    assert(TextLine~=nil, "ERROR: style.ApplyToTextLine=function () - textline is nil")
    assert(style.Color~=nil, "ERROR: CreateTextStyle () - .ApplyToTextLine() - Color is nil")
    assert(style.Scale~=nil, "ERROR: CreateTextStyle () - .ApplyToTextLine() - Scale is nil")
    assert(style.Font~=nil, "ERROR: CreateTextStyle () - .ApplyToTextLine() - Font is nil")
    TextLine.Color=style.Color
    TextLine:SetScale(style.Scale,style.Scale, 1)
    TextLine.Font=style.Font
  end

  style.ApplyToMultiTextLine=function (TextLine)
    assert(TextLine~=nil, "ERROR: style.ApplyToTextLine=function () - textline is nil")
    assert(style.Color~=nil, "ERROR: CreateTextStyle () - .ApplyToTextLine() - Color is nil")
    assert(style.Scale~=nil, "ERROR: CreateTextStyle () - .ApplyToTextLine() - Scale is nil")
    assert(style.Font~=nil, "ERROR: CreateTextStyle () - .ApplyToTextLine() - Font is nil")
    TextLine.Color=style.Color
    TextLine.Scale=vec3(style.Scale,style.Scale, 1)
    TextLine.Font=style.Font
    TextLine.Redraw()
  end  
  
  
  style.ApplyToSprite=function(sprite)
    assert(sprite~=nil, "ERROR: style.ApplyToSprite=function () - textline is nil")
    assert(style.Color~=nil, "ERROR: CreateTextStyle () - .ApplyToSprite() - Color is nil")
    assert(style.Scale~=nil, "ERROR: CreateTextStyle () - .ApplyToSprite() - Scale is nil")
    sprite.Color=style.Color
    sprite:SetScale(style.Scale,style.Scale, 1)
  end
  
  style.GetAsXML=function ()
    assert(CreateXML~=nil, "ERROR: CreateTextStyle () - .GetAsXML() - CreateXML() is nil")
    local xml=CreateXML()
    xml.WriteStringWithName("Font", style.Font)
    xml.WriteStringWithName("Scale", style.Scale)
    local c=style.Color
    xml.WriteNumbersWithName("Color", c.x, c.y, c.z, c.w)
    return xml
  end
  
  style.SetByXML=function (xml)
    assert(CreateXML~=nil, "ERROR: CreateBorderedRectStyle() - .SetAsXML(xml) - CreateXML() is nil")
    assert(xml~=nil, "ERROR: CreateBorderedRectStyle() - .SetAsXML(xml) - xml is nil")
    local r,g,b,a=xml.ReadNumbersByName("Color")
    style.Color=vec4(r,g,b,a)
    style.Font=xml.ReadStringByName("Font")
    style.Scale=xml.ReadNumberByName("Scale")
  end
  
  style.Copy=function ()
    return CreateTextStyle (style.Color, style.Scale, style.Font)
  end
  
  return style
end

