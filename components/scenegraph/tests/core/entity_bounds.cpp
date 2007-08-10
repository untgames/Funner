#include "shared.h"

typedef com_ptr<TestEntity> EntityPtr;
typedef com_ptr<Node> NodePtr;

void dump (const aaboxf& box)
{
  printf ("[");
  dump (box.minimum ());
  printf (" - ");
  dump (box.maximum ());  
  printf ("]");
}

void dump_bounds (Entity& entity)
{
  if (entity.IsInfiniteBounds ()) printf ("entity '%s' has infinite bounds:\n  local=", entity.Name ());
  else                            printf ("entity '%s' bound:\n  local=", entity.Name ());

  dump (entity.BoundBox ());
  printf ("\n  world=");
  dump (entity.WorldBoundBox ());
  printf ("\n");
}

int main ()
{
  printf ("Results of entity_bounds_test:\n");
  
  EntityPtr entity (TestEntity::Create (), false), child (TestEntity::Create (), false);
  NodePtr parent (Node::Create (), false);
  
  entity->SetName ("entity");
  parent->SetName ("parent");
  child->SetName  ("child");
  
  entity->BindToParent (*parent);
  child->BindToParent (*entity);
  
  child->SetBoundBox (aaboxf (vec3f (-20), vec3f (5)));
  child->Translate (-2, -2, -2);

  printf ("default bound box\n");  
  
  dump_bounds (*entity);
  
  printf ("set finite bound box\n");
  
  entity->SetBoundBox (aaboxf (vec3f (-10), vec3f (10)));
  
  dump_bounds (*entity);
  
  printf ("translate parent\n");
  
  parent->Translate (1, 2, 3);  
  
  dump_bounds (*entity);
  
  printf ("bound statistics:\n");

  printf ("  children_bound_box: ");
  dump   (entity->ChildrenBoundBox ());
  printf ("\n  world_children_bound_box: ");
  dump   (entity->WorldChildrenBoundBox ());
  printf ("\n  full_bound_box: ");
  dump   (entity->FullBoundBox ());
  printf ("\n  world_full_bound_box: ");
  dump   (entity->WorldFullBoundBox ());
  printf ("\n");
  
  printf ("set infinite bounds\n");
  
  entity->SetInfiniteBounds ();
  
  dump_bounds (*entity);  
  
  return 0;
}
