function ReadSpriteFromXML(xml)
  assert(xml, "ERROR: ReadSpriteFromXML(xml) - xml is nil")
  local name=xml.ReadStringByName("Name")
        if not(name) then name="unnamed" end
  local material=xml.ReadStringByName("Material")
  local scale=xml.TryReadVec3ByName("Scale")
  
  local pivot=xml.TryReadVec3ByName("Pivot")
    if not(scale) then
      scale=vec3(1,1,1) 
    end

  local scalemult=xml.ReadNumberByName("ScaleMultiplier")
    if scalemult then
      scale=vec3(scale.x*scalemult, scale.y*scalemult, 1)
    end
 
  local sprite=CreateSprite(material, scale, pivot)
  if name then
    sprite.Name=name
  end
  
  local position=xml.ReadVec3ByName("Position")
  if position then
    sprite.Position=position
  end
  
  local red,green,blue,alpha = xml.ReadNumbersByName("Color")
  if alpha then
    sprite.Color=vec4(red, green, blue, alpha)
  end
  
  local FPS=xml.ReadNumberByName("AnimationFPS")
  local Animation=false
  if FPS then
    Animation=StartPeriodicCall((1/FPS),function()
      sprite.Frame=sprite.Frame+1
    end)
  end
  
  if Animation then
    return sprite, Animation
  else
    return sprite
  end
end
