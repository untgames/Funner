#include "shared.h"

typedef com_ptr<Entity> EntityPtr;

int main ()
{
  printf ("Results of entity_basic_test:\n");
  
  EntityPtr entity (TestEntity::Create (), false);
  
  entity->SetName ("entity1");
  entity->SetColor (1.0f, 0.5f, 0.7f);
  
  const vec3f& color = entity->Color ();

  printf ("color: [%g %g %g]\n", color.x, color.y, color.z);
  
  return 0;
}
