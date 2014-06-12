function input_handler (event)
  if event == "exit" then System.Application.Exit (0) end

  print ("New input event: '" .. event .. "'")
end

function main ()
  Engine.InputEventHandlers.Register ("game1", Input.CreateEventHandler ("input_handler"))
end
