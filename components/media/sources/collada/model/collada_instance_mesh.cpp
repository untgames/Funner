#include "shared.h"

using namespace media::collada;

/*
    Описание реализации InstanceMesh
*/

struct InstanceMesh::Impl: public xtl::reference_counter
{
  stl::string            mesh;  //меш
  collada::MaterialBinds binds; //присоединённые материалы  
};

/*
    Конструкторы / деструктор / присваивание
*/

InstanceMesh::InstanceMesh ()
  : impl (new Impl, false)
  {}
  
InstanceMesh::InstanceMesh (const InstanceMesh& imesh, media::CloneMode mode)
  : impl (media::clone (imesh.impl, mode, "media::collada::InstanceMesh::InstanceMesh"))
  {}  

InstanceMesh::~InstanceMesh ()
{
}

InstanceMesh& InstanceMesh::operator = (const InstanceMesh& imesh)
{
  impl = imesh.impl;
  
  return *this;
}

/*
    Меш
*/

const char* InstanceMesh::Mesh () const
{
  return impl->mesh.c_str ();
}

void InstanceMesh::SetMesh (const char* mesh_id)
{
  if (!mesh_id)
    common::RaiseNullArgument ("media::collada::InstanceMesh::SetMesh", "mesh_id");
    
  impl->mesh = mesh_id;
}

/*
    Присоединённые материалы
*/

media::collada::MaterialBinds& InstanceMesh::MaterialBinds ()
{
  return impl->binds;
}

const media::collada::MaterialBinds& InstanceMesh::MaterialBinds () const
{
  return impl->binds;
}
