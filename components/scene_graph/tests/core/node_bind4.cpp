#include "shared.h"

void dump (const math::mat4f& m)
{
  for (size_t i = 0; i < 4; i++)
  {
    printf ("  [ ");
    for (size_t j = 0; j < 4; j++)
      printf ("%0.2f ", m [i] [j]);
    printf ("]\n");
  }
}

int main ()
{
  printf ("Results of test node_bind4:\n");

  Node::Pointer node (Node::Create ()), parent (Node::Create ());

  parent->Rotate (math::degree (0.f), math::degree (0.f), math::degree (45.f));
  parent->SetScale (2.f, 1.f, 1.f);

  node->Rotate (math::degree (0.f), math::degree (45.f), math::degree (0.f));
  
  node->BindToParent (*parent);

  math::mat4f calculated_matrix = parent->LocalTM () * node->LocalTM ();

  if (!math::equal (calculated_matrix, node->WorldTM (), 0.01f))
  {
    printf ("Not equal!!!!!!!!\n");

    printf ("calculated matrix:\n");
    dump (calculated_matrix);
    printf ("actual matrix:\n");
    dump (node->WorldTM ());
  }
  else
    printf ("Matrix correct\n");

  return 0;
}
