#include "shared.h"

typedef com_ptr<Entity> EntityPtr;

void print (Entity& entity)
{
  printf ("entity '%s'\n", entity.Name ());
}

void cprint (const Entity& entity)
{
  printf ("entity '%s'\n", entity.Name ());
}

void dump (const Entity& entity, size_t level=1)
{
  for (size_t i=0; i<level; i++)
    printf (" ");
    
  cprint (entity);
  
  for (const Entity* child=entity.FirstChild (); child; child=child->NextChild ())
    dump (*child, level+1);    
}

int main ()
{
  printf ("Results of entity_traverse_test:\n");
  
  EntityPtr entity (Entity::Create (), false), child1 (Entity::Create (), false), child2 (Entity::Create (), false);
  
  entity->SetName ("entity");
  child1->SetName ("child1");
  child2->SetName ("child2");    

  child2->BindToParent (*child1);
  child1->BindToParent (*entity);
  
  printf ("entities tree:\n");
  
  dump (*entity);
  
  printf ("traverse top-to-bottom\n");  
  entity->Traverse (&print, EntityTraverseMode_TopToBottom);
  
  printf ("traverse bottom-to-top\n");
  entity->Traverse (&print, EntityTraverseMode_BottomToTop);
  
  printf ("traverse top-to-bottom (const)\n");  
  const_cast<const Entity&> (*entity).Traverse (&cprint, EntityTraverseMode_TopToBottom);
  
  printf ("traverse bottom-to-top (const)\n");
  const_cast<const Entity&> (*entity).Traverse (&cprint, EntityTraverseMode_BottomToTop);

  return 0;
}
