#include "shared.h"

const char* get_ort_name (NodeOrt ort)
{
  switch (ort)
  {
    case NodeOrt_X: return "X";
    case NodeOrt_Y: return "Y";
    case NodeOrt_Z: return "Z";
    default:        return "";
  }
}

void test (Node& node, NodeOrt dir, NodeOrt inv)
{
  try
  {
    printf ("test direction=%s invariant=%s: ", get_ort_name (dir), get_ort_name (inv));
    
    node.ResetOrientation ();
    node.LookTo (vec3f (1, 1, 1), dir, inv, NodeTransformSpace_Parent);
    
    vec3f x = node.WorldOrtX (),
          y = node.WorldOrtY (),
          z = node.WorldOrtZ ();
    
    printf ("Ox=[%.2f %.2f %.2f] Oy=[%.2f %.2f %.2f] Oz=[%.2f %.2f %.2f]\n",
      x.x, x.y, x.z, y.x, y.y, y.z, z.x, z.y, z.z);
  }
  catch (std::exception& exception)
  {
    printf ("%s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of node_look_to_test:\n");
  
  Node::Pointer node (Node::Create ());

  for (int i=0; i<NodeOrt_Num; i++)
    for (int j=0; j<NodeOrt_Num; j++)
    {
      test (*node, (NodeOrt)i, (NodeOrt)j);
    }

  return 0;
}
