#include "shared.h"

using namespace scene_graph;

class MyController
{
  public:
//Конструктор
    MyController  () {}
    MyController  (const MyController&) {}
    ~MyController () {}

    void operator () (float dt) {}

  private:
    MyController& operator = (const MyController&); //no impl
};

class UnbindController
{
  public:
//Конструктор
    UnbindController  (Node* in_node_to_unbind)
      : node_to_unbind (in_node_to_unbind)
      {}

    UnbindController  (const UnbindController& source)
      : node_to_unbind (source.node_to_unbind)
      {}

    ~UnbindController () {}

    void operator () (float dt)
    {
      node_to_unbind->Unbind ();
    }

  private:
    UnbindController& operator = (const UnbindController&); //no impl

  private:
    Node* node_to_unbind;
};

int main ()
{
  printf ("Results of controller2_test:\n");

  Scene scene;
  
  Node::Pointer node = Node::Create (), node2 = Node::Create (), node3 = Node::Create ();

  node->AttachController (UnbindController (node3.get ()));
  node2->AttachController (MyController ());
  node3->AttachController (MyController ());

  node2->BindToScene (scene);
  node->BindToScene (scene);
  node3->BindToScene (scene);

  scene.Root ().Update (5);

  return 0;
}
