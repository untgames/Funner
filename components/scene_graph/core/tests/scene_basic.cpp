#include "shared.h"

void on_destroy (Node& node, NodeEvent)
{
  printf ("destroy node '%s'\n", node.Name ());
}

void on_scene_attach (Node& node, NodeEvent)
{
  printf ("node '%s' attached to scene '%s'\n", node.Name (), node.Scene  ()->Name ());
}

void on_scene_detach (Node& node, NodeEvent)
{
  printf ("node '%s' detached from scene '%s'\n", node.Name (), node.Scene  ()->Name ());
}

void on_scene_change (Node& node, NodeEvent)
{
  printf ("node '%s' change scene '%s'\n", node.Name (), node.Scene () ? node.Scene ()->Name () : "null");
}

void traverse_fn (Entity& entity)
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
  printf ("Results of scene_basic_test:\n");

  Scene scene;
  
  scene.SetName ("scene1");
  
  printf ("create entities\n");
  
  for (int i=0; i<10; i++)
  {
    char name [40];
    
    xtl::xsnprintf (name, sizeof (name), "entity%u", i);
    
    TestEntity::Pointer entity = TestEntity::Create ();
    
    entity->SetName (name);
    
    entity->RegisterEventHandler (NodeEvent_BeforeDestroy, &on_destroy);
    entity->RegisterEventHandler (NodeEvent_BeforeSceneDetach, &on_scene_detach);
    entity->RegisterEventHandler (NodeEvent_AfterSceneAttach, &on_scene_attach);
    entity->RegisterEventHandler (NodeEvent_AfterSceneChange, &on_scene_change);
    
    entity->BindToScene (scene, NodeBindMode_AddRef);
  }
  
  printf ("%u entities in scene\n", scene.EntitiesCount ());
  
  printf ("traverse\n");

  scene.Traverse (&traverse_fn);
  
  printf ("visit_each\n");

  MyVisitor visitor;

  scene.VisitEach (visitor);

  printf ("exit\n");

  return 0;
}
