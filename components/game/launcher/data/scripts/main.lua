Application.LoadConfiguration ("data/configurations/configuration.xml")

velocity = {['+x']=0; ['-x']=0; ['+y']=0; ['-y']=0}

velocity_x = 3
velocity_y = 3
mouse_sens = 5	

scene = Scene.Scene.Create ()

sprite = Scene.Sprite.Create ()

sprite.Material = "sprite_material"
sprite.Color = vec4 (1, 1, 1, 0.5)

sprite:BindToScene (scene)

camera = Scene.OrthoCamera.Create ()

camera.Name = "Camera1"
camera.Position = vec3 (0, 0, -3)
camera.Left = -10
camera.Right = 10
camera.Top = 10
camera.Bottom = -10
camera.ZNear = -10
camera.ZFar = 10

camera:BindToScene (scene)

Application.InitRender () --необходимо вызывать до Application.SetScreen (screen)

viewport = Render.Viewport.Create ()

viewport.Name = "Viewport1"
viewport.RenderPath = "Render2d"

viewport:SetArea (0, 0, 100, 100);
viewport.Camera = camera

screen = Render.Screen.Create ()

screen.BackgroundColor = vec4 (1, 1, 1, 1)
screen:Attach (viewport)

Application.SetScreen (screen)

listener1 = Scene.Listener.Create ()

listener1:BindToParent (camera)

Application.SetListener (listener1)

sound_emitter1 = Scene.SoundEmitter.Create ("gorilka")

sound_emitter1:BindToParent (sprite)
sound_emitter1:Play ()

function onBeginMove(ort)
  velocity [ort] = velocity [ort] + 1
end

function onEndMove(ort)
  velocity [ort] = velocity [ort] - 1
end

function onMove(dx, dy)
  sprite:Translate (dx * velocity_x * mouse_sens, dy * velocity_y * mouse_sens, 0)
end

function sign (x)
  if x < 0 then
    return -1
  elseif x > 0 then
    return 1
  else
    return 0
  end
end

function idle(dt)
  sprite:Translate (sign (velocity ['+x'] - velocity ['-x']) * dt * velocity_x,
                    sign (velocity ['+y'] - velocity ['-y']) * dt * velocity_y, 0)
end
