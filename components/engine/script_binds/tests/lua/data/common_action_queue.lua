three_time_no_delay_handler_counter = 0

function three_time_no_delay_handler (dt)
  print ("three_time_no_delay_handler called with dt=" .. dt .. " timer=" .. timer.SecondsEllapsed)

  three_time_no_delay_handler_counter = three_time_no_delay_handler_counter + 1
	
  if (three_time_no_delay_handler_counter == 3) then  
    three_time_no_delay_handler_connection:Disconnect ()
    timer:Reset ()
  end  
end

function delayed_handler (dt)
  print ("delayed_handler called with dt=" .. dt)
end

function instant_handler (dt)
  print ("instant_handler called with dt=" ..dt)
end  

function exit_handler (dt)
  print ("exit_handler called with dt=" .. dt .. " timer=" .. timer.SecondsEllapsed)
  System.Application.Exit (0)
end

function test ()
  timer = Common.Timer.Create ()

  local action_queue = Common.ActionQueue.Create ()
  
  three_time_no_delay_handler_connection = action_queue:RegisterEventHandler (0, 0.2, Common.ActionQueue.CreateEventHandler ("three_time_no_delay_handler"))
  action_queue:RegisterEventHandler (0.5, 0.2, Common.ActionQueue.CreateEventHandler ("delayed_handler"))
--  action_queue:RegisterEventHandler (Common.ActionQueue.CreateEventHandler ("instant_handler"))  
  action_queue:RegisterEventHandler (1, 1, Common.ActionQueue.CreateEventHandler ("exit_handler"))  
end
