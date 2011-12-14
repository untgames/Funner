local xml         = Common.StringNode.LoadXml ("/anonymous/system/startup_environment_variables.xml")
local nodes_count = xml.ChildrenCount

for i=0, nodes_count-1 do
  local node = xml:Child (i)
  
  if node.Name == "Property" then  
    local name  = node:Get ("Name")
    local value = node:Get ("Value")
    
    print (string.format ("%s=%s", name, value))
  end
end  
