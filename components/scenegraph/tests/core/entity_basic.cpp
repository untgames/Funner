#include <xtl/intrusive_ptr.h>
#include <xtl/signal.h>
#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

void dump (const vec3f& v)
{
  printf ("[%g %g %g]", v.x, v.y, v.z);
}

void on_destroy (Entity* entity)
{
  printf ("entity '%s' destroyed\n", entity->Name ());
}

int main ()
{
  printf ("Results of entity1_test:\n");
  
  EntityPtr entity (Entity::Create (), false);
  
  entity->Rename   ("entity1");
  entity->SetColor (1, 0, 0);
  entity->Listeners (EntityEvent_Destroyed).connect (&on_destroy);

  printf ("name: '%s'\n", entity->Name ());
  printf ("color: ");  
  dump   (entity->Color ());
  printf ("\n");
  printf ("use count: %u\n", entity->UseCount ());

  return 0;
}
