velocity = {['+x']=0; ['-x']=0; ['+y']=0; ['-y']=0}

x_axis_value = 0
y_axis_value = 0

velocity_x = 3
velocity_y = 3
mouse_sens = 5  

scene = Scene.Scene.Create ()

sprite = Scene.Sprite.Create ()

sprite.Material = "gun_material"
sprite.Color = vec4 (1, 1, 1, 0.5)
--sprite.AlphaReference = 0.5

sprite:BindToScene (scene)

camera = Scene.OrthoCamera.Create ()

camera.Name = "Camera1"
camera.Position = vec3 (0, 0, 0)
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

screen.BackgroundColor = vec4 (0.3, 0.3, 0.3, 1)
screen:Attach (viewport)

Application.SetScreen (screen)

listener1 = Scene.Listener.Create ()

listener1:BindToParent (camera)

Application.SetListener (listener1)

sound_emitter1 = Scene.SoundEmitter.Create ("gorilka")

sound_emitter1:BindToParent (sprite)
--sound_emitter1:Play ()

text_line = Scene.TextLine.Create ()

text_string = "\"UNTGS - the best!\" AV"

text_line.Text = text_string
--text_line.Font = "data/fonts/times_new_roman.xfont"
text_line.Font = "data/fonts/a_damn_mess.xfont"
text_line.Name = "TextLine"
text_line.Color = vec4 (0, 0, 1, 1)
text_line:SetAlignment (Scene.TextLineAlignment.Center, Scene.TextLineAlignment.Center)
--text_line:SetScale (2, 2, 1)
text_line:SetPosition (0, 0, -1)

text_line:BindToScene (scene)

function onBeginMove(ort)
  velocity [ort] = velocity [ort] + 1
end

function onXAxis(axis_value)
  velocity ['+x'] = velocity ['+x'] - x_axis_value + axis_value

  x_axis_value = axis_value
end

function onYAxis(axis_value)
  velocity ['-y'] = velocity ['-y'] - y_axis_value + axis_value

  y_axis_value = axis_value
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

current_text_length = 1

function idle (dt)
  sprite:Translate ((velocity ['+x'] - velocity ['-x']) * dt * velocity_x,
                    (velocity ['+y'] - velocity ['-y']) * dt * velocity_y, 0)

  if (current_text_length > string.len (text_string)) then current_text_length = 1 end

--  text_line.Text = string.char (string.byte (text_string, 1, current_text_length))

  current_text_length = current_text_length + 1
end
