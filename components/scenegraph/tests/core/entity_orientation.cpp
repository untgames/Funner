#include <mathlib.h>
#include <xtl/intrusive_ptr.h>
#include "shared.h"

using namespace math;

typedef xtl::com_ptr<Entity> EntityPtr;

void dump (const quatf& q)
{
  printf ("[%g %g %g %g]", q.x, q.y, q.z, q.w);
}

int main ()
{
  printf ("Results of entity_orientation test:\n");
  
  EntityPtr entity (Entity::Create (), false);
    
  entity->SetOrientation (quatf (14.4f, 17.1f, -6.66f, 34.7f));
  printf ("set orientation quatf result: ");  
  dump   (entity->Orientation ());
  printf ("\n");

  entity->SetOrientation (48.7f, 0.f, 27.14f, 6.12f);
  printf ("set orientation 4 float result: ");  
  dump   (entity->Orientation ());
  printf ("\n");

  entity->SetOrientation (61.7f, 0.18f, 22.4f);
  printf ("set orientation 3 float result: ");  
  dump   (entity->Orientation ());
  printf ("\n");
  printf ("world orientation result: ");  
  dump   (entity->WorldOrientation ());
  printf ("\n");

  entity->ResetOrientation ();
  printf ("reset orientation result: ");  
  dump   (entity->Orientation ());
  printf ("\n");
  printf ("world orientation result: ");  
  dump   (entity->Orientation ());
  printf ("\n");

  return 0;
}
