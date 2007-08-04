#include <xtl/intrusive_ptr.h>
#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

void dump_transform (Entity& entity)
{
  printf ("  position: ");
  dump_position (entity);
  printf ("\n  orientation: ");
  dump_orientation (entity);
  printf ("\n  scale: ");
  dump_scale (entity);  
  printf ("\n");
}

int main ()
{
  printf ("Results of entity_transform_translate_test:\n");
  
  EntityPtr entity (Entity::Create (), false), parent (Entity::Create (), false);
  
  entity->BindToParent (*parent);
    
  entity->SetPosition (1, 1, 1);
  entity->SetOrientation (180, 0, 0, 1);

  printf ("parent:\n");
  
  parent->SetPosition    (1, 2, 3);
  parent->SetOrientation (90, 0, 0);
  parent->SetScale       (2, 2, 2);
  
  dump_transform (*parent);

  printf ("entity:\n");  

  dump_transform (*entity);  
  
  printf ("translate entity in parent space\n");
  
  entity->Translate (1, 0, 0, EntityTransformSpace_Parent); 

  printf ("entity:\n");  

  dump_transform (*entity);  
  
  printf ("translate entity in local space\n");
  
  entity->Translate (0, 1, 0, EntityTransformSpace_Local);

  printf ("entity:\n");  

  dump_transform (*entity);    
  
  printf ("translate entity in world space\n");
  
  entity->Translate (1, 1, 1, EntityTransformSpace_World);

  printf ("entity:\n");  

  dump_transform (*entity);      

  return 0;
}
