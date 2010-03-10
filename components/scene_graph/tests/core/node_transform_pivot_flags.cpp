#include "shared.h"

const char* get_string (bool value)
{
  return value ? "true" : "false";
}

void test (Node& node)
{
  printf ("pivot=%s orientation_pivot=%s scale_pivot=%s\n", get_string (node.PivotEnabled ()),
    get_string (node.OrientationPivotEnabled ()), get_string (node.ScalePivotEnabled ()));
}

int main ()
{
  printf ("Results of node_transform_pivot_flags_test:\n");
  
  Node::Pointer node (Node::Create ());
  
  test (*node);
  
  node->SetPivotPosition (1.0f, 0.0f, 0.0f);
  
  test (*node);
  
  node->SetOrientationPivotEnabled (false);
  
  test (*node);
  
  node->SetScalePivotEnabled (false);
  
  test (*node);
  
  node->SetOrientationPivotEnabled (true);
  node->SetScalePivotEnabled (true);
  node->ResetPivotPosition ();
  
  test (*node);
  
  return 0;
}
