
function CreateSprite(material, scale, pivot)
  assert(material, "ERROR: LoadSprite(material, scale, pivot) - material is nil")
  local sprite=Scene.Sprite.Create()
  sprite.Material=material
  if scale then
    sprite.Scale=scale
  end
  if pivot then
    sprite:SetPivotPosition(pivot.x, pivot.y, pivot.z)
  end
  sprite.ScaleInherit = true
  return sprite
end

