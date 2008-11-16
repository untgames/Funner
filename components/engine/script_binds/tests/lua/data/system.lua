count = 0

function idle1 ()
  print ("idle1")

  count = count + 1  

  if count == 3 then
    print ("connection blocked")  
    connection:Block ()
  end
  
  if count == 4 then
    print ("exit")
    System.Application.Exit (0)
  end
end

function idle2 ()
  print ("idle2: blocked=" .. tostring (connection.Blocked))

  if connection.Blocked then
    print ("connection unblocked")
    connection:Unblock ()
  end
end

function test ()
  connection = System.Application.RegisterEventHandler (System.ApplicationEvent.OnIdle, System.Application.CreateEventHandler ("idle1"))
  System.Application.RegisterEventHandler (System.ApplicationEvent.OnIdle, System.Application.CreateEventHandler ("idle2"))  
end
