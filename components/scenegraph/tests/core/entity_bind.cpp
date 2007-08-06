#include "shared.h"

typedef com_ptr<Entity> EntityPtr;

void on_update (Entity& entity)
{
  printf ("entity '%s' updated\n", entity.Name ());
}

void on_bind (Entity& entity)
{
  printf ("entity '%s' binded to parent '%s'\n", entity.Name (), entity.Parent () ? entity.Parent ()->Name () : "#ROOT");
}

void on_unbind (Entity& entity)
{
  printf ("entity '%s' unbinded from parent '%s'\n", entity.Name (), entity.Parent () ? entity.Parent ()->Name () : "#ROOT");
}

void on_before_destroy (Entity& entity)
{
  printf ("entity '%s' before destroy\n", entity.Name ());
}

void on_after_destroy (Entity& entity)
{
  printf ("entity '%s' destroyed\n", entity.Name ());
}

void register_notifiers (Entity& entity)
{
  entity.Listeners (EntityEvent_AfterUpdate).connect (&on_update);
  entity.Listeners (EntityEvent_AfterBind).connect (&on_bind);
  entity.Listeners (EntityEvent_BeforeUnbind).connect (&on_unbind);
  entity.Listeners (EntityEvent_BeforeDestroy).connect (&on_before_destroy);
  entity.Listeners (EntityEvent_AfterDestroy).connect (&on_after_destroy);
}

int main ()
{
  printf ("Results of entity_bind_test:\n");
  
  EntityPtr entity (Entity::Create (), false);
  
  entity->SetName   ("entity1");
  register_notifiers (*entity);
   
  EntityPtr parent1 (Entity::Create (), false);
  
  parent1->SetName ("parent1");
  register_notifiers (*parent1);
  
  entity->BindToParent (*parent1);
  
  EntityPtr parent2 (Entity::Create (), false);  
  
  parent2->SetName ("parent2");
  register_notifiers (*parent2);
  
  entity->BindToParent (*parent2);    

  return 0;
}
