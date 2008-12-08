#include "shared.h"

using namespace scene_graph::physics;

class MyVisitor: public visitor<void, Body>
{
  public:
    void visit (Body& object) { printf ("body '%s' model-name='%s'\n", object.Name (), object.ModelName ()); }
};

int main ()
{
  printf ("Results of physics_body_test:\n");

  Body::Pointer body = Body::Create ();

  body->SetModelName ("collision-model1");
  body->SetName ("body1");
  
  MyVisitor visitor;

  body->VisitEach (visitor);

  return 0;
}
