#include "shared.h"

class MyVisitor: public xtl::visitor<void, TestEntity>
{
  public:
    void visit (TestEntity& object)  { printf ("  test_entity '%s'\n", object.Name ()); }
};

int main ()
{
  printf ("Results of scene_traverse2_test:\n");

  Scene scene;

  TestEntity::Pointer entity1 = TestEntity::Create (), entity2 = TestEntity::Create ();

  entity1->SetName ("entity1");
  entity1->SetBoundBox (aaboxf (-10.0f, 10.0f));
  entity1->BindToScene (scene);

  entity2->SetName ("entity2");
  entity2->SetBoundBox (aaboxf (-10.0f, 10.0f));
  entity2->BindToScene (scene);

  MyVisitor visitor;

  printf ("Visit two entities:\n");
  scene.VisitEach (visitor);

  entity1->Unbind ();

  printf ("Visit one entity:\n");
  scene.VisitEach (visitor);

  return 0;
}
