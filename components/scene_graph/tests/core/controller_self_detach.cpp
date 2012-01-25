#include "shared.h"

template <int I> class MyController: public Controller
{
  public:
    typedef xtl::com_ptr<MyController> Pointer;
    
    static Pointer Create (Node& node)
    {
      return Pointer (new MyController (node), false);
    }
  
    MyController (Node& in_node) : Controller (in_node)
    {
      NodeOwnsController ();
    }
    
    void Update (const TimeValue&)
    {
      printf ("MyController<%d>::Update\n", I);
      
      if (I == 2)
      {
        Controller::Pointer next = NextController ();
        
        Detach ();
        
        if (next)
          next->Detach ();
      }
      
      if (I == 4)
        MyController<5>::Create (*AttachedNode ());          
    }
};

int main ()
{
  printf ("Results of controller_self_detach_test:\n");
  
  Node::Pointer node = Node::Create ();
  
  MyController<1>::Create (*node);
  MyController<2>::Create (*node);  
  MyController<3>::Create (*node);  
  MyController<4>::Create (*node);  
  
  printf ("first update\n");

  node->Update (TimeValue (1, 2));
  
  printf ("second update\n");  
  
  node->Update (TimeValue (1, 2));  

  return 0;
}

