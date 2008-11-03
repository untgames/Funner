function viewport_test ()
  print ("Viewport test")

  local viewport1 = Render.Viewport.Create ()

  print ("Name = " .. viewport1.Name)
  print ("RenderPath = " .. viewport1.RenderPath)
  print ("ZOrder = " .. viewport1.ZOrder)
  print ("Camera = " .. tostring (viewport1.Camera))
  print ("IsActive = " .. tostring (viewport1.Active))
  print ("BackgroundColor = " .. tostring (viewport1.BackgroundColor))
  print ("BackgroundState = " .. tostring (viewport1.BackgroundState))
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
  print ("BackgroundState = " .. tostring (viewport1.BackgroundState))
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))
  print ("Property 'property1' = " .. tostring (viewport1:GetProperty ("property1")))

  viewport1:SetArea (0, 0, 100, 100)
  viewport1:SetOrigin (0, 0)
  viewport1:SetSize (100, 100)
  
  print ("Area = (" .. tostring (viewport1.Area.Left) .. ", " .. tostring (viewport1.Area.Top) .. ", " .. tostring (viewport1.Area.Width) .. ", " .. tostring (viewport1.Area.Height) .. ")")
  print ("Right = " .. tostring (viewport1.Area.Right) .. ", Bottom = " .. tostring (viewport1.Area.Bottom))

  viewport1:Deactivate ()
  print ("IsActive = " .. tostring (viewport1.Active))
  viewport1:Activate ()
  print ("IsActive = " .. tostring (viewport1.Active))

  viewport1:SetBackgroundColor (0.4, 0.3, 0.2, 0.1)
  print ("BackgroundColor = " .. tostring (viewport1.BackgroundColor))

  viewport1:SetBackgroundColor (0.4, 0.3, 0.2)
  print ("BackgroundColor = " .. tostring (viewport1.BackgroundColor))

  viewport1:EnableBackground ()
  print ("BackgroundState = " .. tostring (viewport1.BackgroundState))
  viewport1:DisableBackground ()
  print ("BackgroundState = " .. tostring (viewport1.BackgroundState))

  viewport1:RemoveProperty ("property1")
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))
  viewport1:SetProperty ("property1", "value_of_property1")
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))
  viewport1:RemoveAllProperties ()
  print ("HasProperty 'property1' = " .. tostring (viewport1:HasProperty ("property1")))
end

function screen_test ()
  print ("Screen test")

  local screen1 = Render.Screen.Create ()

  print ("Name = " .. screen1.Name)
  print ("BackgroundColor = " .. tostring (screen1.BackgroundColor))
  print ("BackgroundState = " .. tostring (screen1.BackgroundState))
  print ("Viewports count = " .. screen1.ViewportsCount)
--  print ("Id = " .. tostring (screen1.Id))

  screen1.Name = "screen1"
  screen1.BackgroundColor = vec4 (0.1, 0.2, 0.3, 0.4)

  print ("Name = " .. screen1.Name)
  print ("BackgroundColor = " .. tostring (screen1.BackgroundColor))
  print ("BackgroundState = " .. tostring (screen1.BackgroundState))

  screen1:SetBackgroundColor (0.4, 0.3, 0.2, 0.1)
  print ("BackgroundColor = " .. tostring (screen1.BackgroundColor))

  screen1:SetBackgroundColor (0.4, 0.3, 0.2)
  print ("BackgroundColor = " .. tostring (screen1.BackgroundColor))

  screen1:DisableBackground ()
  print ("BackgroundState = " .. tostring (screen1.BackgroundState))
  screen1:EnableBackground ()
  print ("BackgroundState = " .. tostring (screen1.BackgroundState))

  local viewport1 = Render.Viewport.Create ()

  viewport1.Name = "viewport1"

  screen1:Attach (viewport1)
  print ("Viewports count = " .. screen1.ViewportsCount)
  print ("Viewports 0 name = " .. screen1:Viewport (0).Name)

  screen1:Detach (viewport1)
  print ("Viewports count = " .. screen1.ViewportsCount)

  screen1:Attach (viewport1)
  print ("Viewports count = " .. screen1.ViewportsCount)

  screen1:DetachAllViewports ()
  print ("Viewports count = " .. screen1.ViewportsCount)
end

function test ()
  viewport_test ()
  screen_test ()
end
