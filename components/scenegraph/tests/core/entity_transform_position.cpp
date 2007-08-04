#include <xtl/intrusive_ptr.h>
#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

int main ()
{
  printf ("Results of entity_transform_position_test:\n");
  
  EntityPtr entity (Entity::Create (), false);
    
  printf ("set position vec3f result: ");
  entity->SetPosition (vec3f (14.4f, 17.1f, -6.66f));
  dump_position (*entity);
  printf ("\n");

  printf ("set position (x, y, z) result: ");
  entity->SetPosition (0.f, 27.14f, 6.12f);
  dump_position (*entity);
  printf ("\n");  

  printf ("reset position result: ");
  entity->ResetPosition ();
  dump_position (*entity);  
  printf ("\n");

  return 0;
}
