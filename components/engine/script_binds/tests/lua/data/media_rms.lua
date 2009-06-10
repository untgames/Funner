function test () 
  local binding1 = Engine.ResourceManager.CreateFileGroupBinding ("*")
  
  binding1:Prefetch ()
  binding1:Load ()
  binding1:Unload ()
  
  local group1 = Engine.ResourceManager.Group.Create ()
  
  print ("Is group empty = " .. tostring (group1.Empty))
  print ("Group size = " .. tostring (group1.Size))
  
  group1:Add ("data/media_rms.lua")
  
  print ("Is group empty = " .. tostring (group1.Empty))
  print ("Group size = " .. tostring (group1.Size))

  local group2 = Engine.ResourceManager.Group.Create ("data/bv.*")

  print ("Is group2 empty = " .. tostring (group2.Empty))
  print ("Group2 size = " .. tostring (group2.Size))

  group1:Add (group2)
  
  print ("Is group empty = " .. tostring (group1.Empty))
  print ("Group size = " .. tostring (group1.Size))
  print ("Item 1 = " .. group1:Item (1))

  group1:Remove (group2)

  print ("Is group empty = " .. tostring (group1.Empty))
  print ("Group size = " .. tostring (group1.Size))

  group1:Remove ("data/media_rms.lua")

  print ("Is group empty = " .. tostring (group1.Empty))
  print ("Group size = " .. tostring (group1.Size))

  group1:AddFileGroup ("data/media_rms.lua")

  print ("Is group empty = " .. tostring (group1.Empty))
  print ("Group size = " .. tostring (group1.Size))

  group1:Clear ()
  
  print ("Is group empty = " .. tostring (group1.Empty))
  print ("Group size = " .. tostring (group1.Size))

  local binding2 = Engine.ResourceManager.CreateBinding (group1)
  
  Engine.ResourceManager.FlushUnusedResources ()
end
