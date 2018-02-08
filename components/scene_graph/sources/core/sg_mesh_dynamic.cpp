#include "shared.h"

using namespace scene_graph;
using namespace stl;
using namespace common;

/*
    DynamicMesh implementation description
*/

struct DynamicMesh::Impl: public xtl::instance_counter<DynamicMesh>
{
  media::geometry::Mesh mesh; //mesh

  Impl ()
  {
  }

  Impl (const media::geometry::Mesh& in_mesh)
    : mesh (in_mesh)
    {}
};

/*
    Constructor / destructor
*/

DynamicMesh::DynamicMesh ()
  : impl (new Impl)
  {}

DynamicMesh::DynamicMesh (const media::geometry::Mesh& mesh)
  : impl (new Impl (mesh))
  {}

DynamicMesh::~DynamicMesh ()
{
  delete impl;
}

/*
    Model creation
*/

DynamicMesh::Pointer DynamicMesh::Create ()
{
  return Pointer (new DynamicMesh, false);
}

DynamicMesh::Pointer DynamicMesh::Create (const media::geometry::Mesh& mesh)
{
  return Pointer (new DynamicMesh (mesh), false);
}

/*
   Set / get mesh
*/

const media::geometry::Mesh& DynamicMesh::Mesh () const
{
  return impl->mesh;
}

void DynamicMesh::SetMesh (const media::geometry::Mesh& mesh)
{
  impl->mesh = mesh;
}

/*
    Method called during object visiting
*/

void DynamicMesh::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}
