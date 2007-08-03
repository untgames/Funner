#include <mathlib.h>
#include <xtl/intrusive_ptr.h>
#include "shared.h"

using namespace math;

typedef xtl::com_ptr<Entity> EntityPtr;

void dump (const vec3f& v)
{
  printf ("[%g %g %g]", v.x, v.y, v.z);
}

int main ()
{
  printf ("Results of entity_scale test:\n");
  
  EntityPtr entity (Entity::Create (), false);
    
  entity->SetScale (vec3f (14.4f, 17.1f, -6.66f));
  printf ("set scale vec3f result: ");  
  dump   (entity->Scale ());
  printf ("\n");

  entity->SetScale (0.f, 27.14f, 6.12f);
  printf ("set scale 3 float result: ");  
  dump   (entity->Scale ());
  printf ("\n");
  printf ("world scale result: ");  
  dump   (entity->WorldScale ());
  printf ("\n");

  entity->ResetScale ();
  printf ("reset scale result: ");  
  dump   (entity->Scale ());
  printf ("\n");
  printf ("world scale result: ");  
  dump   (entity->WorldScale ());
  printf ("\n");

  return 0;
}
