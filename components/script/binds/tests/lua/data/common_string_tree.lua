function clone_test ()
  local node1 = StringNode.Create ()

  node1:AddAttribute ("First attribute value")

  local child = StringNode.Create ()

  child.Name = "Child Node"

  node1:AddChild (child)

  local node2 = node1:Clone ()

  print ("Node 1 attributes áount: " .. tostring (node1.AttributesCount))
  print ("Node 2 attributes áount: " .. tostring (node2.AttributesCount))
  print ("Node 1 children count: " .. tostring (node1.ChildrenCount))
  print ("Node 2 children count: " .. tostring (node2.ChildrenCount))

  node1:RemoveAllAttributes ()
  node1:RemoveAllChildren ()

  print ("Node 1 attributes áount: " .. tostring (node1.AttributesCount))
  print ("Node 2 attributes áount: " .. tostring (node2.AttributesCount))
  print ("Node 1 children count: " .. tostring (node1.ChildrenCount))
  print ("Node 2 children count: " .. tostring (node2.ChildrenCount))
end

function print_node (node, indent)
  Utils.PrintIndent (indent)
  print ("Node '" .. node.Name .. "'")
  Utils.PrintIndent (indent)
  print ("Attributes:")

  for current_attribute = 0, node.AttributesCount - 1, 1 do
    Utils.PrintIndent (indent)
    print (current_attribute .. " = " .. node:Attribute (current_attribute))
  end

  Utils.PrintIndent (indent)
  print ("End of attributes")
  Utils.PrintIndent (indent)
  print ("Children")

  for current_child = 0, node.ChildrenCount - 1, 1 do
    print_node (node:Child (current_child), indent + 4)
  end

  Utils.PrintIndent (indent)
  print ("End of children")
end

function xml_test ()
  local node = StringNode.LoadXml ("data/string_tree.xml")
  print_node (node, 0)
  node:SaveXml ("/io/stdout")
end

function test ()
  local node = StringNode.Create ()

  print ("Name: '" .. node.Name .. "'")
  print ("Attributes áount: " .. node.AttributesCount)
  print ("Children count: " .. node.ChildrenCount)
  print ("Attributes capacity: " .. node.AttributesCapacity)
  print ("Children capacity: " .. node.ChildrenCapacity)

  node.Name = "Node"
  node:ReserveAttributes (4)
  node:ReserveChildren (4)
  
  print ("Added attribute " .. node:AddAttribute ("First attribute value"))
  print ("Added attribute " .. node:AddAttribute ("Second attribute value"))
  print ("Second attribute value is " .. node:Attribute (1))
  print ("Attributes capacity: " .. node.AttributesCapacity)
  print ("Children capacity: " .. node.ChildrenCapacity)

  node:SetAttribute (1, "New second attribute value")

  print ("Second attribute value is " .. node:Attribute (1))

  local child = StringNode.Create ()

  child.Name = "Child Node"

  node:AddChild (child)

  print ("Name: '" .. node.Name .. "'")
  print ("Attributes áount: " .. node.AttributesCount)
  print ("Children count: " .. node.ChildrenCount)

  print ("First child name is " .. node:Child (0).Name)

  node:AddAttribute (0, "New first attribute value")

  print ("Attributes áount: " .. node.AttributesCount)
  print ("First attribute value is " .. node:Attribute (0))
  print ("Second attribute value is " .. node:Attribute (1))

  node:RemoveAttribute (1)

  print ("Attributes áount: " .. node.AttributesCount)

  node:RemoveAllAttributes ()

  print ("Attributes áount: " .. node.AttributesCount)

  local child2 = StringNode.Create ()

  child2.Name = "Child2 Node"

  child2:AddAttribute ("child2 attribute0")

  node:AddChild (0, child2)
  child:AddChild (0, child2)

  print ("First child name is " .. node:Child (0).Name)
  print ("Children count: " .. node.ChildrenCount)

  local find_node = node:Find ("Child2 Node")

  if (not (find_node)) then
    print ("Node 'Child2 Node' not found")
  else
    print ("Node 'Child2 Node' found")
  end

  find_node = node:Find ("Child22 Node")

  if (not (find_node)) then
    print ("Node 'Child22 Node' not found")
  else
    print ("Node 'Child22 Node' found")
  end

  print ("Node.Child2 Node = " .. node:Get ("Child Node.Child2 Node"))
  print ("Node.Child2 Node = " .. node:Get ("Child Node.Child22 Node", "default value"))

  node:Set ("Child Node.Child2 Node", "new child2 attribute0")

  print ("Child2 attribute0 = " .. child2:Attribute (0))

  node:RemoveChild (1)

  print ("Children count: " .. node.ChildrenCount)

  node:RemoveAllChildren ()

  print ("Children count: " .. node.ChildrenCount)

  clone_test ()

  xml_test ()
end
