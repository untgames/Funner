#include "shared.h"

void dump (Node& node)
{
  printf ("%s position [%.2f %.2f %.2f] scale [%.2f %.2f %.2f]\n", node.Name (), node.Position ().x, node.Position ().y, node.Position ().z,
    node.Scale ().x, node.Scale ().y, node.Scale ().z);
}

int main ()
{
  printf ("Results of animation1_test:\n");
  
  try
  {  
    AnimationManager manager;
    
    manager.LoadResource ("data/test.xanim");

    Node::Pointer node = Node::Create (), target1 = Node::Create (), target2 = Node::Create ();
    
    target1->SetName ("target1");
    target2->SetName ("target2");

    target1->BindToParent (*node);

    Animation animation = manager.PlayAnimation ("animation1", *node);
    
    target2->BindToParent (*node);    
    
    printf ("f(0):\n");

    node->Update (TimeValue (2, 1));

    dump (*target1);
    dump (*target2);
    
    printf ("f(1):\n");    
    
    node->Update (TimeValue (3, 1));    
    
    dump (*target1);
    dump (*target2);    
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
