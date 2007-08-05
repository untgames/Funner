#include "shared.h"
#include <xtl/visitor.h>

typedef com_ptr<Entity> EntityPtr;

class MyVisitor: public visitor<void, Entity, Light, Camera>
{
  public:
    void visit (Entity& object) { printf ("entity '%s'\n", object.Name ()); }
    void visit (Light& object)  { printf ("light '%s'\n", object.Name ()); }
    void visit (Camera& object) { printf ("camera '%s'\n", object.Name ()); }
};

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
  printf ("Results of entity_traverse_visitor_test:\n");
  
  EntityPtr entity (Entity::Create (), false), child1 (Light::Create (), false), child2 (PerspectiveCamera::Create (), false);
  
  entity->SetName ("entity");
  child1->SetName ("child1");
  child2->SetName ("child2");    

  child2->BindToParent (*child1);
  child1->BindToParent (*entity);
  
  printf ("entities tree:\n");
  
  dump (*entity);  
  
  printf ("visitor traverse top-to-bottom\n");
  entity->TraverseAccept (MyVisitor (), EntityTraverseMode_TopToBottom);
  
  printf ("visitor traverse bottom-to-top\n");
  entity->TraverseAccept (MyVisitor (), EntityTraverseMode_BottomToTop);

  return 0;
}
