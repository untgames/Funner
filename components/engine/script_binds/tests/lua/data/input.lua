function input_handler (event)
end

function test ()
  local cursor = Input.Cursor.Create ()  
  
  print ("Position: " .. tostring (cursor.Position))  

  cursor.Position = vec2 (2.1, 3.2)

  print ("Position: " .. tostring (cursor.Position))
  print ("Visible: " .. tostring (cursor.Visible))

  cursor:Hide ()
  
  print ("Visible: " .. tostring (cursor.Visible))
  
  local handler = Input.CreateEventHandler ("input_handler")
end
