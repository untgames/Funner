velocity = {['+x']=0; ['-x']=0; ['+y']=0; ['-y']=0}

root_node = Scene.Node.Create ()
sprite    = Scene.Sprite.Create ()

sprite.Material = "sprite_material"
sprite.Color = vec3 (1, 1, 1)

function set_root_node (new_root_node)
  root_node = new_root_node

  sprite:BindToParent (root_node)
end

function onBeginMove(ort)
  velocity [ort] = velocity [ort] + 3
end

function onEndMove(ort)
  velocity [ort] = velocity [ort] - 3
end

function idle(dt)
  sprite:Translate ((velocity ['+x'] - velocity ['-x'])*dt, (velocity ['+y'] - velocity ['-y'])*dt, 0)
end
