#include "shared.h"

Node::Pointer child_to_release;

void on_scene_change ()
{
  child_to_release = 0;
}

int main ()
{
  printf ("Results of node_parent_unbind_test:\n");
  
  try
  {
    Scene scene;

    Node::Pointer node = Node::Create ();
    
    node->BindToScene (scene);

    Node::Pointer child1 = Node::Create (), child2 = Node::Create (), child3 = Node::Create (), child_of_child2 = Node::Create ();

    child1->BindToParent (*node);
    child2->BindToParent (*node);
    child3->BindToParent (*node);
    child_of_child2->BindToParent (*child2);

    child_to_release = child1;

    xtl::connection on_change_scene = child1->RegisterEventHandler (NodeEvent_AfterSceneChange, xtl::bind (&on_scene_change));

    printf ("Child1 has scene is %c, child2 has scene is %c, child3 has scene is %c, child of child 2 has scene is %c\n",
            child1->Scene () ? 'y' : 'n', child2->Scene () ? 'y' : 'n', child3->Scene () ? 'y' : 'n', child_of_child2->Scene () ? 'y' : 'n');

    child1 = 0;

    node->Unbind ();

    on_change_scene.disconnect ();

    printf ("child2 has scene is %c, child3 has scene is %c, child of child 2 has scene is %c\n",
            child2->Scene () ? 'y' : 'n', child3->Scene () ? 'y' : 'n', child_of_child2->Scene () ? 'y' : 'n');
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }
  
  return 0;
}
