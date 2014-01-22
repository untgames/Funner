#include "shared.h"

void on_finish_handler (AnimationEvent event, Animation& animation)
{
  printf ("On finish\n");
}

int main ()
{
  printf ("Results of animation3_test:\n");
  
  try
  {  
    AnimationManager manager;
    
    manager.LoadResource ("data/test.xanim");

    Node::Pointer node = Node::Create (), target1 = Node::Create ();
    
    target1->SetName ("target1");

    target1->BindToParent (*node);

    Animation animation = manager.PlayAnimation ("animation1", *node);
    
    animation.RegisterEventHandler (AnimationEvent_OnFinish, on_finish_handler);

    node->Update (TimeValue (0, 1));

    animation.Pause ();
    animation.Seek (0);

    printf ("Update with long time\n");

    node->Update (TimeValue (4, 1));

    printf ("Play again\n");
    
    animation.Play ();

    printf ("Update to same time\n");
    
    node->Update (TimeValue (4, 1));

    printf ("Updated\n");

    node->Update (TimeValue (8, 1));
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
