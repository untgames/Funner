local function print_log (message)
  print (message)
end  

local function print_state (node, indent)
  if not indent then indent = "" else indent = indent .. "  " end
  
  print (string.format ("%snode '%s'", indent, node.Name))
  print (string.format ("%s{", indent))
  print (string.format ("%s  position: %.2f %.2f %.2f", indent, node.Position.x, node.Position.y, node.Position.z))
  print (string.format ("%s  ortx: %.2f %.2f %.2f", indent, node.LocalOrtX.x, node.LocalOrtX.y, node.LocalOrtX.z))
  print (string.format ("%s  scale: %.2f %.2f %.2f", indent, node.Scale.x, node.Scale.y, node.Scale.z))    
  
  local child = node.FirstChild
  
  while child do
    print_state (child, indent)

    child = child.NextChild
  end
  
  print (string.format ("%s}", indent))    
end

function main ()
  local scene_manager     = Engine.SceneManagers.Get ("SceneManager1")
  local animation_manager = Engine.AnimationManagers.Get ("AnimationManager1")

  scene_manager:LoadScene ("data/animation.xscene", "", Scene.SceneManager.CreateLogHandler (print_log))

  local root = scene_manager:CreateScene ("vase_pieces")
  
  print ("Before animation")
  print_state (root)
  
  local animation = animation_manager:CreateAnimation ("vase_pieces", root)    
  
  animation:Play ()
  
  root:Update (0)
  root:Update (1.04)    
  
  print ("After animation")  
  
  print_state (root)  
end
