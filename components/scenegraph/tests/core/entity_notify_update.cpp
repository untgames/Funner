#include "shared.h"

typedef xtl::com_ptr<Entity> EntityPtr;

void on_update1 (Entity* entity)
{
  printf ("entity '%s' updated\n", entity->Name ());  
}

void on_update2 (Entity* entity)
{
  entity->SetColor (1, 0, 0);
}

int main ()
{
  printf ("Results of entity_notify_update_test:\n");
  
  EntityPtr entity (Entity::Create (), false);
  
  entity->SetName   ("entity1");
  entity->Listeners (EntityEvent_Updated).connect (&on_update1);
  
  printf ("Update color\n");  
  
  entity->SetColor (0, 1, 0);
  
  printf ("Update name\n");
  
  entity->SetName ("entity2");
  
  entity->Listeners (EntityEvent_Updated).connect (&on_update2);
  
  printf ("Begin update\n");
  
  EntityUpdateLock lock (entity.get ());
  
  printf ("Update color\n");
  
  entity->SetColor (0, 0, 1);
  
  printf ("Update name\n");
  
  entity->SetName ("entity3");
  
  printf ("End update\n");
  
  EntityUpdateLock ().Swap (lock);

  return 0;
}
