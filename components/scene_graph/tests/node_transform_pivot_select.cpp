#include "shared.h"

void test (Node& node)
{
  printf ("  parent position: ");
  dump_position (*node.Parent ());
  printf ("\n");
  
  printf ("  parent zero point: ");
  dump (node.Parent ()->WorldTM () * vec3f (0.0f));
  printf ("\n");
  
  printf ("  node position: ");
  dump_position (node);
  printf ("\n");
  
  printf ("  node zero point: ");
  dump (node.WorldTM () * vec3f (0.0f));
  printf ("\n");
  
  printf ("  test point: ");
  dump (node.WorldTM () * vec3f (1.0f, 0.0f, 0.0f));
  printf ("\n");
}

int main ()
{
  printf ("Results of node_transform_pivot_select_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  parent->SetScale (3.0f, 3.0f, 1.0f);
  parent->SetOrientation (degree (0.0f), degree (0.0f), degree (90.0f));
  parent->SetPivotPosition (0.0f, 1.0f, 0.0f);  
  
  node->SetScale (2.0f, 2.0f, 1.0f);
  node->SetPosition (1.0f, 0.0f, 0.0f);
  node->SetPivotPosition (1.0f, 0.0f, 0.0f);
  node->SetOrientation (degree (0.0f), degree (0.0f), degree (90.0f));
  
  printf ("parent Orienation & Scale pivots, node Orientation & Scale pivots\n");
  
  node->BindToParent (*parent);  

  test (*node);
  
  printf ("parent Orienation pivot, node Orientation & Scale pivots\n");
  
  parent->SetScalePivotEnabled (false);
  
  test (*node);
  
  printf ("parent Scale pivot, node Orientation & Scale pivots\n");
  
  parent->SetScalePivotEnabled (true);
  parent->SetOrientationPivotEnabled (false);
  
  test (*node);
  
  printf ("parent Orientation & Scale pivots, node Orientation pivot\n");
  
  parent->SetOrientationPivotEnabled (true);
  node->SetScalePivotEnabled (false);

  test (*node);
  
  printf ("parent Orientation & Scale pivots, node Scale pivot\n");
  
  node->SetScalePivotEnabled (true);
  node->SetOrientationPivotEnabled (false);

  test (*node);
  
  printf ("parent Orientation & Scale pivots, node Scale pivot, no scale inherit\n");
  
  node->SetScaleInherit (false);
  
  test (*node);
  
  printf ("parent Orientation & Scale pivots, node Scale pivot, no orientation inherit\n");
  
  node->SetScaleInherit (true);
  node->SetOrientationInherit (false);
  
  test (*node);
  
  printf ("parent Orientation & Scale pivots, node Orientation pivot, no scale inherit\n");
  
  node->SetScaleInherit (false);
  node->SetOrientationInherit (true);
  node->SetScalePivotEnabled (false);
  node->SetOrientationPivotEnabled (true);
  
  test (*node);
  
  printf ("parent Orientation & Scale pivots, node Orientation pivot, no orientation inherit\n");
  
  node->SetScaleInherit (true);
  node->SetOrientationInherit (false);
  
  test (*node);
  
  return 0;
}
