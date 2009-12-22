#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    Реализация Mesh
*/

typedef media::CollectionImpl<Surface, ICollection<Surface> > SurfaceCollection;

struct Mesh::Impl: public xtl::reference_counter
{
  SurfaceCollection surfaces; //поверхности меша
  stl::string       id;       //идентификатор меша
};

/*
    Конструктор
*/

Mesh::Mesh ()
  : impl (new Impl, false)
  {}

Mesh::Mesh (Impl* in_impl)
  : impl (in_impl, false)
  {}  
  
Mesh::Mesh (const Mesh& mesh)
  : impl (mesh.impl)
  {}
  
Mesh::~Mesh ()
{
}

Mesh& Mesh::operator = (const Mesh& mesh)
{
  impl = mesh.impl;
  
  return *this;
}

/*
    Создание копии
*/

Mesh Mesh::Clone () const
{
  return Mesh (new Impl (*impl));
}

/*
    Идентификатор меша
*/

const char* Mesh::Id () const
{
  return impl->id.c_str ();
}

void Mesh::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::collada::Mesh::SetId", "id");
    
  impl->id = id;
}

/*
    Список поверхностей
*/

Mesh::SurfaceList& Mesh::Surfaces ()
{
  return impl->surfaces;
}

const Mesh::SurfaceList& Mesh::Surfaces () const
{
  return impl->surfaces;
}
