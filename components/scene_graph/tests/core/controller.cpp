#include "shared.h"

class MyController: public IController
{
  public:
    MyController (Node& in_node, const vec3f& in_pos) : node (in_node), pos (in_pos) {}
  
    void Update (float)
    {
      node.SetPosition (pos); 
    }
    
    static IController* Create (Node& node, const xtl::any_reference& param)
    {
      return new MyController (node, param.cast<vec3f> ());
    }
    
  private:
    Node& node;
    vec3f pos;
};

int main ()
{
  printf ("Results of controller_test:\n");
  
  ControllerManager::Register ("MyController", &MyController::Create);
  
  Node::Pointer node = Node::Create ();
  
  vec3f pos (1.0f, 2.0f, 3.0f);
  
  node->AttachController ("MyController", pos);
  
  node->Update (1.0f);

  dump_position (*node);

  return 0;
}
