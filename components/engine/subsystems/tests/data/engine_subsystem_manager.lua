local CONFIG_FILE_NAME = "data/config.xml"

function test ()
  local subsystem_manager = Engine.SubsystemManager
  
  print ("Removing subsystems of group 1")
  subsystem_manager:Remove ("Group1*")
  print ("Subsystems removed")
  
  print ("Restarting subsystems of group2")
  subsystem_manager:Restart (CONFIG_FILE_NAME, "Group2*")
  print ("Subsystems restarted")
  
  print ("Starting subsystems of group1")
  subsystem_manager:Start (CONFIG_FILE_NAME, "Group1*")
  print ("Subsystems restarted")

  print ("Restarting subsystems of group1 and group2")
  subsystem_manager:Restart (CONFIG_FILE_NAME, "Group*")
  print ("Subsystems restarted")  
end
