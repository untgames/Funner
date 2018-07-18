#include "shared.h"

void dump_joints (SkinMesh& model, const char* title)
{
  printf ("%s:\n", title);

  for (size_t i=0, count=model.JointsCount (); i<count; i++)
  {
    Node::Pointer node = model.Joint (i);

    printf ("  #%u: %s\n", i+1, node ? node->Name () : "null");
  }
}

void update_node_notifier ()
{
  printf ("   joint nodes are updated\n");
}

void update_matrix_notifier ()
{
  printf ("   joint matrixes are updated\n");
}

int main ()
{
  printf ("Results of mesh_skin_basic_test:\n");
  
  SkinMesh::Pointer model (SkinMesh::Create ());

  xtl::auto_connection c1 = model->RegisterEventHandler (SkinMeshEvent_AfterJointNodesUpdate, xtl::bind (&update_node_notifier)),
                       c2 = model->RegisterEventHandler (SkinMeshEvent_AfterJointMatricesUpdate, xtl::bind (&update_matrix_notifier));

  model->SetJointsCount (3);

  model->SetJointName (0, "node1");
  model->SetJointName (1, "node2");
  model->SetJointName (2, "node3");

  dump_joints (*model, "initial");

  Node::Pointer node1 = Node::Create (), node2 = Node::Create (), node3 = Node::Create ();

  node1->SetName ("node1");
  node2->SetName ("node2");
  node3->SetName ("node3");

  node1->BindToParent (*model);
  node2->BindToParent (*model);
  node3->BindToParent (*model);

  dump_joints (*model, "linear structure");

  node2->BindToParent (*node1);

  dump_joints (*model, "rebind with hierarchy");

  node2->Unbind ();

  dump_joints (*model, "unbind");

  node3->SetName ("node2");

  dump_joints (*model, "rename");

  model->SetJointInvMatrix (0, math::mat4f (2.0f));

  printf ("check inv matrix: ");

  math::vec3f p = model->JointInvMatrix (0) * math::vec3f (1.0f);

  dump (p);

  printf ("\n");

  return 0;
}
