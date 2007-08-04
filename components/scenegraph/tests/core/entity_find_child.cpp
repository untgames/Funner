#include "shared.h"

typedef com_ptr<Entity> EntityPtr;

void test (const Entity& entity, const char* child_name, EntitySearchMode mode)
{
  const Entity* child = entity.FindChild (child_name, mode);
  
  if (!child) printf ("  no child entity with name '%s'\n", child_name);
  else        printf ("  child entity '%s' found\n", child->Name ());
}

void dump (const Entity& entity, size_t level=1)
{
  for (size_t i=0; i<level; i++)
    printf (" ");
    
  printf ("entity '%s'\n", entity.Name ()); 
  
  for (const Entity* child=entity.FirstChild (); child; child=child->NextChild ())
    dump (*child, level+1);    
}

int main ()
{
  printf ("Results of entity_find_child_test:\n");

  EntityPtr entity (Entity::Create (), false), child1 (Entity::Create (), false), child2 (Entity::Create (), false);
  
  entity->SetName ("entity");
  child1->SetName ("child1");
  child2->SetName ("child2");    

  child2->BindToParent (*child1);
  child1->BindToParent (*entity);
  
  printf ("entities tree:\n");
  
  dump (*entity);

  printf ("search on all sublevels:\n");

  test (*entity, "child1", EntitySearchMode_OnAllSublevels);
  test (*entity, "child2", EntitySearchMode_OnAllSublevels);

  printf ("search on next sublevel:\n");

  test (*entity, "child1", EntitySearchMode_OnNextSublevel);
  test (*entity, "child2", EntitySearchMode_OnNextSublevel);

  return 0;
}
