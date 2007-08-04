#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

void on_destroy (Entity& entity)
{
  printf ("entity '%s' destroyed\n", entity.Name ());
}

int main ()
{
  printf ("Results of entity_basic_test:\n");
  
  EntityPtr entity (Entity::Create (), false);
  
  entity->SetName   ("entity1");
  entity->SetColor  (1, 0, 0);
  entity->Listeners (EntityEvent_BeforeDestroy).connect (&on_destroy);
  
  const vec3f& color = entity->Color ();

  printf ("name: '%s'\n", entity->Name ());
  printf ("color: [%g %g %g]\n", color.x, color.y, color.z);
  printf ("use count: %u\n", entity->UseCount ());

  return 0;
}
