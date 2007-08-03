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
  printf ("Results of entity_position test:\n");
  
  EntityPtr entity (Entity::Create (), false);
    
  entity->SetPosition (vec3f (14.4f, 17.1f, -6.66f));
  printf ("set position vec3f result: ");  
  dump   (entity->Position ());
  printf ("\n");

  entity->SetPosition (0.f, 27.14f, 6.12f);
  printf ("set position 3 float result: ");  
  dump   (entity->Position ());
  printf ("\n");
  printf ("world position result: ");  
  dump   (entity->WorldPosition ());
  printf ("\n");

  entity->ResetPosition ();
  printf ("reset position result: ");  
  dump   (entity->Position ());
  printf ("\n");
  printf ("world position result: ");  
  dump   (entity->WorldPosition ());
  printf ("\n");

  return 0;
}
