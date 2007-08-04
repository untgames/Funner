#include "shared.h"

typedef com_ptr<Entity> EntityPtr;

int main ()
{
  printf ("Results of entity_transform_tm_test:\n");
  
  EntityPtr entity (Entity::Create (), false), parent (Entity::Create (), false);
  
  entity->BindToParent (*parent);
    
  entity->SetPosition    (1, 1, 1);
  parent->SetPosition    (2, 2, 2);
  parent->SetOrientation (90, 1, 0, 0);
  parent->SetScale       (2, 2, 2);
  
  vec3f test_point;
  vec4f test_vec (1, 0, 0, 0);

  printf ("test-point: local=");
  dump   (entity->LocalTM () * test_point);
  printf (" world=");
  dump   (entity->WorldTM () * test_point);
  printf ("\n");
  printf ("test-vec: local=");
  dump   (entity->LocalTM () * test_vec);
  printf (" world=");
  dump   (entity->WorldTM () * test_vec);
  printf ("\n");    

  return 0;
}
