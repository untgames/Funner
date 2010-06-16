local five_time_no_delay_handler_counter = 0

function five_time_no_delay_handler (dt)
  print ("five_time_no_delay_handler called with dt=" .. string.format ("%1.1f", dt) .. " timer=" .. string.format ("%1.1f", timer.Time) 
    .. " paused=" .. tostring (not (timer.Started)))

  five_time_no_delay_handler_counter = five_time_no_delay_handler_counter + 1
  
  if (five_time_no_delay_handler_counter == 2) then
    timer.Started = false
    print ("timer paused with time=" .. string.format ("%1.1f", timer.Time))
  end
  
  if (five_time_no_delay_handler_counter == 4) then    
    timer.Started = true
    print ("timer resumed with time=" .. string.format ("%1.1f", timer.Time))    
  end
  
  if (five_time_no_delay_handler_counter == 5) then  
    five_time_no_delay_handler_connection:Disconnect ()
    timer:Reset ()
    timer.Started = true
  end  
end

function delayed_handler (dt)
  print ("delayed_handler called with dt=" .. string.format ("%1.1f", dt) .. " timer =" .. string.format ("%1.1f", timer.Time))
end

function instant_handler (dt)
  print ("instant_handler called with dt=" ..dt)
end  

function exit_handler (dt)
  print ("exit_handler called with dt=" .. string.format ("%1.0f", dt) .. " timer=" .. string.format ("%1.1f", timer.Time))
  System.Application.Exit (0)
end

function registered_in_pause_handler (dt)
  print ("registered_in_pause_handler called with dt=" .. string.format ("%1.2f", dt) .. " timer =" .. string.format ("%1.2f", timer.Time))
end

function action_queue_pause_handler (dt)
  if action_queue.Paused then 
    print ("action_queue_pause_handler resumed action_queue with dt=" .. string.format ("%1.1f", dt) .. " timer=" .. string.format ("%1.1f", timer.Time))
  
    action_queue:Resume ()
    action_queue_pause_handler_connection:Disconnect ()
  else  
    print ("action_queue_pause_handler paused action_queue with dt=" .. string.format ("%1.1f", dt) .. " timer=" .. string.format ("%1.1f", timer.Time))
  
    action_queue:Pause ()
    action_queue_paused = true
    action_queue:RegisterEventHandler (0, 0.25, Common.ActionQueue.CreateEventHandler ("registered_in_pause_handler"))  
  end  
end

function test ()
  timer = Common.Timer.Create ()
  
  timer:Start ()

  action_queue = Common.ActionQueue.Create ()
  
  five_time_no_delay_handler_connection = action_queue:RegisterEventHandler (0, 0.2, Common.ActionQueue.CreateEventHandler (five_time_no_delay_handler))
  action_queue:RegisterEventHandler (0.5, 0.2, Common.ActionQueue.CreateEventHandler ("delayed_handler"))
--  action_queue:RegisterEventHandler (Common.ActionQueue.CreateEventHandler ("instant_handler"))  
  action_queue:RegisterEventHandler (3, 1, Common.ActionQueue.CreateEventHandler ("exit_handler"))  
  
  print ("action_queue paused = " .. tostring (action_queue.Paused))
  
  local action_queue2 = Common.ActionQueue.Create ()
  
  action_queue_pause_handler_connection = action_queue2:RegisterEventHandler (2.6, 1, Common.ActionQueue.CreateEventHandler ("action_queue_pause_handler"))  
end
