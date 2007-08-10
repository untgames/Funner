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
    
    _snprintf (name, sizeof (name), "entity%u", i);
    
    Entity* entity = TestEntity::Create ();
    
    entity->SetName (name);
    
    entity->Event (NodeEvent_BeforeDestroy).connect (&on_destroy);
    entity->Event (NodeEvent_BeforeSceneDetach).connect (&on_scene_detach);
    entity->Event (NodeEvent_AfterSceneAttach).connect (&on_scene_attach);
    
    entity->BindToScene (scene, NodeBindMode_Capture);
  }
  
  printf ("%u entities in scene\n", scene.EntitiesCount ());
  
  printf ("traverse\n");

  scene.Traverse (&traverse_fn);
  
  printf ("visit_each\n");

  scene.VisitEach (MyVisitor ());

  printf ("exit\n");

  return 0;
}
