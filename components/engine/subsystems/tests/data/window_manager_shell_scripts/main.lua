function input_handler (event)
  if event == "exit" then System.Application.Exit (0) end

  print ("New input event: '" .. event .. "'")
end

function main ()
  local screen1 = Render.Screen.Create ()
  local screen2 = Render.Screen.Create ()  
  
  screen1.BackgroundColor = vec4 (0.8, 0, 0, 0)
  screen2.BackgroundColor = vec4 (0, 0.8, 0, 0)
  
  Engine.Screens.Register ("Screen1", screen1)
  Engine.Screens.Register ("Screen2", screen2)
  
  Engine.InputEventHandlers.Register ("game1", Input.CreateEventHandler ("input_handler"))
end
