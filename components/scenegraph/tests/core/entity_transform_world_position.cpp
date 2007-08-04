#include "shared.h"

typedef com_ptr<Entity> EntityPtr;

int main ()
{
  printf ("Results of entity_transform_world_position_test:\n");
  
  EntityPtr entity (Entity::Create (), false), parent (Entity::Create (), false);
  
  entity->BindToParent (*parent);
    
  printf ("set position vec3f result: ");
  entity->SetPosition (1, 0, 0);
  dump_position (*entity);
  printf ("\n");
  
  printf ("translate parent\n");
  
  parent->Translate (0, 2, 0);
  
  printf ("parent position: ");
  dump_position (*parent);
  printf ("\nentity position: ");
  dump_position (*entity);
  printf ("\n");
  
  printf ("rotate parent\n");
  
  parent->Rotate (180, 0, 1, 0);
  
  printf ("parent orientation: ");
  dump_orientation (*parent);
  printf ("\nentity position: ");
  dump_position (*entity);
  printf ("\n");  
  
  printf ("scale parent\n");
  
  parent->Scale (2, 1, 1);
  
  printf ("parent scale: ");
  dump_scale (*parent);
  printf ("\nentity position: ");
  dump_position (*entity);
  printf ("\n");  

  return 0;
}
