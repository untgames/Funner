#include <xtl/intrusive_ptr.h>
#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

void dump (const vec3f& v)
{
  printf ("[%g %g %g]", v.x, v.y, v.z);
}

int main ()
{
  printf ("Results of entity1_test:\n");
  
  EntityPtr entity = Entity::Create ();
  
  entity->Rename   ("entity1");
  entity->SetColor (1, 0, 0);

  printf ("name: '%s'\n", entity->Name ());
  printf ("color: ");
  dump   (entity->Color ());
  printf ("\n");

  return 0;
}
