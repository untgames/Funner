velocity = {['+x']=0; ['-x']=0; ['+y']=0; ['-y']=0}

velocity_x = 3
velocity_y = 3
mouse_sens = 5	

root_node = Scene.Node.Create ()
sprite    = Scene.Sprite.Create ()

sprite.Material = "sprite_material"
sprite.Color = vec3 (1, 1, 1)

function set_root_node (new_root_node)
  root_node = new_root_node

  sprite:BindToParent (root_node)
end

function onBeginMove(ort)
  velocity [ort] = velocity [ort] + 1
end

function onEndMove(ort)
  velocity [ort] = velocity [ort] - 1
end

function onMove(dx, dy)
  sprite:Translate (dx * velocity_x * mouse_sens, dy * velocity_y * mouse_sens, 0)
end

function sign (x)
  if x < 0 then
    return -1
  elseif x > 0 then
    return 1
  else
    return 0
  end
end

function idle(dt)
  sprite:Translate (sign (velocity ['+x'] - velocity ['-x']) * dt * velocity_x,
                    sign (velocity ['+y'] - velocity ['-y']) * dt * velocity_y, 0)
end
