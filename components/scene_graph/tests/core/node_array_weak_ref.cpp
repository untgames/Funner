#include "shared.h"

void dump (const char* title, const NodeArray& array)
{
  const char* type = "unknown";
  
  switch (array.LinkMode ())
  {
    case NodeArrayLinkMode_AddRef:
      type = "intrusive-array";
      break;
    case NodeArrayLinkMode_WeakRef:
      type = "weak-ref-array";
      break;
    default:
      break;
  }

  printf ("%s: %s{", title, type);

  for (size_t i=0, count=array.Size (); i<count; i++)
  {
    if (i)
      printf (", ");
      
    printf ("%s", array.Item (i).Name ());
  }

  printf ("}\n");
}

int main ()
{
  printf ("Results of node_array_weak_ref_test:\n");
  
  try
  {
    NodeArray array (NodeArrayLinkMode_WeakRef);
    
    Node::Pointer node1 = Node::Create (),
                  node2 = Node::Create (),
                  node3 = Node::Create ();
                  
    node1->SetName ("Node1");
    node2->SetName ("Node2");
    node3->SetName ("Node3");
    
    size_t index = array.Add (*node1);

    array.Add (*node2);
    array.Add (*node3);
    
    dump ("after insert", array);
    
    node2 = 0;
    
    dump ("after node2 destroy", array);
    
    array.Remove (index);

    dump ("after remove node by index", array);

    array.Remove (*node3);

    dump ("after remove by value", array);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
