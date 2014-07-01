#include "shared.h"

void default_update_notifier (Node& node, NodeEvent)
{
  printf ("update node '%s'\n", node.Name ());
}

void update_notifier (SpriteModel& model, SpriteModelEvent event)
{
  switch (event)
  {
    case SpriteModelEvent_AfterSpriteDescsUpdate:
    {
      printf ("update sprite '%s' descs (count=%u):\n", model.Name (), model.SpriteDescsCount ());
      
      for (size_t i=0; i<model.SpriteDescsCount (); i++)
      {
        const SpriteDesc& desc = model.SpriteDescs () [i];
        
        printf ("    position: [%.2f %.2f %.2f]\n", desc.position.x, desc.position.y, desc.position.z);
        printf ("    size:     [%.2f %.2f]\n", desc.size.x, desc.size.y);
        printf ("    color:    [%.2f %.2f %.2f %.2f]\n", desc.color.x, desc.color.y, desc.color.z, desc.color.w);
        printf ("    tex_offs: [%.2f %.2f]\n", desc.tex_offset.x, desc.tex_offset.y);
        printf ("    tex_size: [%.2f %.2f]\n", desc.tex_size.x, desc.tex_size.y);
        printf ("    normal:   [%.2f %.2f %.2f]\n", desc.normal.x, desc.normal.y, desc.normal.z);
        printf ("    angle:    %.2f deg\n", math::degree (desc.rotation));
      }
      
      break;
    }
    default:
      printf ("unknown event %d\n", event);
      break;
  }
}

int main ()
{
  printf ("Results of sprite2_test:\n");
  
  Sprite::Pointer sprite (Sprite::Create ());
  
  xtl::auto_connection c1 = sprite->RegisterEventHandler (SpriteModelEvent_AfterSpriteDescsUpdate, &update_notifier),
                       c2 = sprite->RegisterEventHandler (NodeEvent_AfterUpdate, &default_update_notifier); 

  sprite->SetName      ("Sprite1");
  sprite->SetMaterial  ("material1");
  sprite->SetAlpha     (0.5f);  
  sprite->SetTexOffset (math::vec2f (0.5f, 0.5f));

  return 0;
}
