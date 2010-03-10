#include "shared.h"

class MyController: public Controller
{
  public:
    typedef xtl::com_ptr<MyController> Pointer;
  
    MyController (Node& in_node, const vec3f& in_pos) : Controller (in_node), pos (in_pos) {}
    
    ~MyController ()
    {
      printf ("MyController::~MyController\n");
    }
  
    void Update (float)
    {
      AttachedNode ()->SetPosition (pos); 
    }
    
  private:
    vec3f pos;
};

void on_node_destroy (Node&, NodeEvent)
{
  printf ("node destroyed\n");
}

int main ()
{
  printf ("Results of controller_test:\n");
  
  {
    printf ("-- basic usage --\n");
      
    Node::Pointer node = Node::Create ();
    
    node->RegisterEventHandler (NodeEvent_AfterDestroy, &on_node_destroy);
    
    vec3f pos (1.0f, 2.0f, 3.0f);
    
    MyController::Pointer controller (new MyController (*node, pos), false);
      
    node->Update (1.0f);

    dump_position (*node);
    
    printf ("\n");  
  }
  
  {
    printf ("-- node owns controller --\n");
    
    Node::Pointer node = Node::Create ();
    
    node->RegisterEventHandler (NodeEvent_AfterDestroy, &on_node_destroy);
    
    vec3f pos (1.0f, 2.0f, 3.0f);
    
    MyController::Pointer controller (new MyController (*node, pos), false);

    controller->NodeOwnsController ();
    
    controller = 0;

    printf ("exit\n");
  }
  
  {
    printf ("-- controller owns node --\n");
    
    Node::Pointer node = Node::Create ();
    
    node->RegisterEventHandler (NodeEvent_AfterDestroy, &on_node_destroy);
    
    vec3f pos (1.0f, 2.0f, 3.0f);
    
    MyController::Pointer controller (new MyController (*node, pos), false);

    controller->ControllerOwnsNode ();
    
    node = 0;

    printf ("exit\n");
  }
  
  {
    printf ("-- change ownership --\n");
    
    Node::Pointer node = Node::Create ();
    
    node->RegisterEventHandler (NodeEvent_AfterDestroy, &on_node_destroy);
    
    vec3f pos (1.0f, 2.0f, 3.0f);
    
    MyController::Pointer controller (new MyController (*node, pos), false);

    controller->NodeOwnsController ();
    
    MyController* controller_save = controller.get ();
    
    controller = 0;
    
    controller_save->ControllerOwnsNode ();

    printf ("exit\n");
  }
  
  {
    printf ("-- detach --\n");
    
    Node::Pointer node = Node::Create ();
    
    node->RegisterEventHandler (NodeEvent_AfterDestroy, &on_node_destroy);
    
    vec3f pos (1.0f, 2.0f, 3.0f);
    
    MyController::Pointer controller (new MyController (*node, pos), false);

    controller->NodeOwnsController ();
    
    MyController* controller_save = controller.get ();
    
    controller = 0;
    
    controller_save->Detach ();

    printf ("exit\n");
  }

  return 0;
}
