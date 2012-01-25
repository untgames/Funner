#include "shared.h"

using namespace scene_graph;

template <int I> class MyController: public Controller
{
  public:
    typedef xtl::com_ptr<MyController> Pointer;
  
    MyController (Node& node, int in_value) : Controller (node), value (in_value) {}
    
    static Pointer Create (Node& node, int value)
    {
      return Pointer (new MyController (node, value), false);
    }
    
    int value;
};

int main ()
{
  printf ("Results of controller3_test:\n");
  
  Node::Pointer node = Node::Create ();

  MyController<1>::Create (*node, 1);
  MyController<2>::Create (*node, 2);
  MyController<1>::Create (*node, 3);
  MyController<2>::Create (*node, 4);
  
  printf ("MyController<1>:\n");

  for (MyController<1>::Pointer ctrl=node->FirstController<MyController<1> > (); ctrl; ctrl=ctrl->NextController<MyController<1> > ())
    printf ("  value=%d\n", ctrl->value);
    
  printf ("MyController<2>:\n");

  for (MyController<2>::Pointer ctrl=node->FirstController<MyController<2> > (); ctrl; ctrl=ctrl->NextController<MyController<2> > ())
    printf ("  value=%d\n", ctrl->value);

  return 0;
}
