function test ()
  local cursor = Input.Cursor.Create ()

  Engine.Cursors.Register ("Cursor1", cursor)
  
  local result = Engine.Cursors.Get ("Cursor1")
  
  result:Hide ()

  local result = Engine.Cursors.Find ("Cursor1")

  if result ~= nil then result:Show () else print ("Cursor1 is nil") end

  local result = Engine.Cursors.Find ("Cursor2")

  if result == nil then print ("Cursor 2 is nil") end
  
  if Engine.Cursors.IsPresent ("Cursor1") then print ("Cursor1 is present") else print ("Cursor1 isn't present") end
  if Engine.Cursors.IsPresent ("Cursor2") then print ("Cursor2 is present") else print ("Cursor2 isn't present") end  
  
  local listener = Scene.Listener.Create ()
  
  listener.Name = "MyListener"
  
  Engine.Listeners.Register ("Listener1", listener)
  
  local result = Engine.Listeners.Get ("Listener1")
  
  print ("Listener name: '" .. result.Name .. "'")
end
