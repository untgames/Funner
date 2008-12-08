#include "shared.h"

void traverse_fn (Entity& entity)
{
  printf ("  entity '%s'\n", entity.Name ());
}

void ctraverse_fn (const Entity& entity)
{
  printf ("  entity '%s'\n", entity.Name ());
}

class MyVisitor: public xtl::visitor<void, TestEntity>
{
  public:
    void visit (TestEntity& object)  { printf ("  test_entity '%s'\n", object.Name ()); }
};

int main ()
{
  printf ("Results of scene_traverse_test:\n");
  
  Scene scene;
  
  TestEntity* entity1 = TestEntity::Create ();
  
  entity1->SetName ("entity1");
  entity1->SetBoundBox (aaboxf (-10.0f, 10.0f));
  entity1->BindToScene (scene, NodeBindMode_Capture);
  
  TestEntity* entity2 = TestEntity::Create ();
  
  entity2->SetName ("entity2");
  entity2->SetInfiniteBounds ();
  entity2->BindToScene (scene, NodeBindMode_Capture);  
  
  TestEntity* entity3 = TestEntity::Create ();
  
  entity3->SetName ("entity3");
  entity3->SetBoundBox (aaboxf (-20.0f, -10.0f));
  entity3->BindToScene (scene, NodeBindMode_Capture);
  
    //обход объектов попадающих в объём
    
  printf ("traverse\n");
  scene.Traverse (aaboxf (-5.0f, 5.0f), &traverse_fn);
  printf ("visit_each\n");  
    
  MyVisitor visitor;
  
  scene.VisitEach (aaboxf (-30.0f, -15.0f), visitor);
  printf ("const traverse\n");
  scene.Traverse (aaboxf (-100.0f, 100.0f), &ctraverse_fn);

  return 0;
}
