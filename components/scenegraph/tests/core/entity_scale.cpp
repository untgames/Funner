#include <xtl/intrusive_ptr.h>
#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

int main ()
{
  printf ("Results of entity_scale_test:\n");
  
  EntityPtr entity (Entity::Create (), false);
    
  printf ("set scale vec3f result: ");
  entity->SetScale (vec3f (14.4f, 17.1f, -6.66f));
  dump_scale (*entity);
  printf ("\n");

  printf ("set scale (sx, sy, sz) result: ");
  entity->SetScale (0.f, 27.14f, 6.12f);
  dump_scale (*entity);
  printf ("\n");

  printf ("reset scale result: ");
  entity->ResetScale ();
  dump_scale (*entity);
  printf ("\n");  

  return 0;
}
