#include "shared.h"

int main ()
{
  printf ("Results of entity_basic_test:\n");
  
  TestEntity::Pointer entity = TestEntity::Create ();
  
  entity->SetName ("entity1");
  entity->SetWireColor (1.0f, 0.5f, 0.7f);
  
  const vec3f& color = entity->WireColor ();

  printf ("color: [%g %g %g]\n", color.x, color.y, color.z);
  
  return 0;
}
