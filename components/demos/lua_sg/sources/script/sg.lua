root_node = Scene.Node.Create ()
sprite    = Scene.Sprite.Create ()

sprite.Material = "sprite_material"
sprite.Color = vec3 (1, 1, 1)

function set_root_node (new_root_node)
  root_node = new_root_node

  sprite:BindToParent (root_node)
end

function vertical_move (distance)
  sprite:Translate (0, distance, 0)
end

function horisontal_move (distance)
  sprite:Translate (distance, 0, 0)
end
