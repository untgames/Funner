function viewport_test ()
  print ("Viewport test")

  local viewport1 = Render.Viewport.Create ()

  print ("Name = " .. viewport1.Name)
  print ("RenderPath = " .. viewport1.RenderPath)
  print ("ZOrder = " .. viewport1.ZOrder)
  print ("Camera = " .. tostring (viewport1.Camera))
  print ("IsActive = " .. tostring (viewport1.Active))
  print ("BackgroundColor = " .. tostring (viewport1.BackgroundColor))
  print ("HasBackground = " .. tostring (viewport1.HasBackground))
--  print ("Id = " .. tostring (viewport1.Id))
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))

  local camera1 = Scene.OrthoCamera.Create ()

  camera1.Name = "camera1"

  viewport1.Name = "viewport1"
  viewport1.RenderPath = "Render2D"
  viewport1.ZOrder = 1
  viewport1.Camera = camera1
  viewport1.Active = 1
  viewport1.BackgroundColor = vec4 (0.1, 0.2, 0.3, 0.4)

  viewport1:SetProperty ("property1", "value_of_property1")

  print ("Name = " .. viewport1.Name)
  print ("RenderPath = " .. viewport1.RenderPath)
  print ("ZOrder = " .. viewport1.ZOrder)
  print ("Camera = " .. tostring (viewport1.Camera.Name))
  print ("IsActive = " .. tostring (viewport1.Active))
  print ("BackgroundColor = " .. tostring (viewport1.BackgroundColor))
  print ("HasBackground = " .. tostring (viewport1.HasBackground))
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))
  print ("Property 'property1' = " .. tostring (viewport1:GetProperty ("property1")))

  viewport1:SetArea (0, 0, 100, 100)
  viewport1:SetOrigin (0, 0)
  viewport1:SetSize (100, 100)

  viewport1:Deactivate ()
  print ("IsActive = " .. tostring (viewport1.Active))
  viewport1:Activate ()
  print ("IsActive = " .. tostring (viewport1.Active))

  viewport1:SetBackgroundColor (0.4, 0.3, 0.2, 0.1)
  print ("BackgroundColor = " .. tostring (viewport1.BackgroundColor))

  viewport1:SetBackgroundColor (0.4, 0.3, 0.2)
  print ("BackgroundColor = " .. tostring (viewport1.BackgroundColor))

  viewport1:EnableBackground ()
  print ("HasBackground = " .. tostring (viewport1.HasBackground))
  viewport1:DisableBackground ()
  print ("HasBackground = " .. tostring (viewport1.HasBackground))

  viewport1:RemoveProperty ("property1")
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))
  viewport1:SetProperty ("property1", "value_of_property1")
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))
  viewport1:RemoveAllProperties ()
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))
end

function test ()
  viewport_test ()
end
