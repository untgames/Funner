

function UI.MakeButton (control)
  assert(control, "ERROR: MakeButton(control) - control is nil")
  
  local button=control
  
  button.ControlType="Button"
  
  
  if not(button.CaptionRelativePosition) then
    button.CaptionRelativePosition=vec3( 0.5, 0.5, 0)
  end
  
  if not(button.CaptionAlignment) then 
    button.CaptionAlignment=vec3( 0.5, 0.5, 0)
  end
 
  --------------------------------------------------------------------------------------------------
  -- основные функции
  --------------------------------------------------------------------------------------------------
  
  button.Press=function()
    button.HandleEvent("Pressed")
    if button.Focused then 
      button.HandleEvent("Released") 
    end
  end
  
  button.OnKeyDown=function(key)
    if key=="Enter" then 
      button.Press() 
      return
    end
    if key=="Space" then 
      button.Press() 
      return
    end
  end
  button.RegisterEventHandler("KeyDown", button.OnKeyDown)
  
  
  --------------------------------------------------------------------------------------------------
  -- работа с XML (сохранение и загрузка)
  --------------------------------------------------------------------------------------------------  
  
  button.GetAsXML=function ()
    local xml=button.GetControlAsXML()
    if button.Caption then
      xml.WriteStringWithName("Caption", button.Caption.Text)
    end
    return xml
  end
  
  button.SetByXML=function (xml)
    assert(xml~=nil, "ERROR: button.SetByXML(xml) - xml is nil")
    --[[local caption=xml.ReadStringByName("Caption")
    if caption then button.SetCaption(caption) end]]
  end
  
  return button
end

