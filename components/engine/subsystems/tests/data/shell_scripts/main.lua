require "include"

function main ()
  included_fn ()
  test_node ()

  local manager = Engine.PhysicsManagers.Get ("PhysicsManager")
  
  print ("Manager description is '" .. manager.Description .. "'")
end
