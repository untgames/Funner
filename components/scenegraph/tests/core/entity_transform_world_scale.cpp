#include <xtl/intrusive_ptr.h>
#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

int main ()
{
  printf ("Results of entity_transform_world_scale_test:\n");
  
  EntityPtr entity (Entity::Create (), false), parent (Entity::Create (), false);
  
  entity->BindToParent (*parent);
    
  printf ("set scale result: ");
  entity->SetScale (1, 2, 3);
  dump_scale (*entity);
  printf ("\n");

  printf ("translate parent\n");
  
  parent->Translate (0, 2, 0);
  
  printf ("parent position: ");
  dump_position (*parent);
  printf ("\nentity scale: ");
  dump_scale (*entity);
  printf ("\n");
  
  printf ("rotate parent\n");
  
  parent->Rotate (30, 0, 0);
  
  printf ("parent orientation: ");
  dump_orientation (*parent);
  printf ("\nentity scale: ");
  dump_scale (*entity);
  printf ("\n");
  
  printf ("scale parent\n");
  
  parent->Scale (2, 3, -1);
  
  printf ("parent scale: ");
  dump_scale (*parent);
  printf ("\nentity scale: ");
  dump_scale (*entity);
  printf ("\n");
  
  printf ("no parent-scale inherit\n");
  
  entity->SetScaleInherit (false);
  
  printf ("entity scale: ");
  dump_scale (*entity);
  printf ("\n");

  return 0;
}
