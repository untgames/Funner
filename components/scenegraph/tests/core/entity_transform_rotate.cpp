#include <xtl/intrusive_ptr.h>
#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

void dump_state (Entity& entity, Entity& parent)
{
  printf ("  parent orientation: ");
  dump_orientation (parent);
  printf ("\n");  
  printf ("  entity orientation: ");
  dump_orientation (entity);
  printf ("\n");
}

int main ()
{
  printf ("Results of entity_transform_rotate_test:\n");
  
  EntityPtr entity (Entity::Create (), false), parent (Entity::Create (), false);
  
  entity->BindToParent (*parent);
  entity->SetOrientation (0, 60, 0);
  
  parent->SetScale (1, 2, 3);
  parent->SetPosition (3, 4, 5);
  parent->SetOrientation (90, 0, 0);
  
  printf ("initial state\n");
  
  dump_state (*entity, *parent);
 
  printf ("rotate entity in parent space\n");
  
  entity->Rotate (-90, 0, -60, EntityTransformSpace_Parent); 

  dump_state (*entity, *parent);
  
  printf ("rotate entity in local space\n");
  
  entity->Rotate (0, 30, 0, EntityTransformSpace_Local);
  
  dump_state (*entity, *parent);  
 
  printf ("rotate entity in world space\n");
  
  entity->Rotate (0, 0, 90, EntityTransformSpace_World);
  
  dump_state (*entity, *parent);

  return 0;
}
