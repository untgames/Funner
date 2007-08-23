#include "shared.h"

using namespace medialib::geometry;
using namespace stl;
using namespace common;

const size_t DEFAULT_MESH_ARRAY_RESERVE = 32;

/*
    Обёртка над мешем
*/

namespace
{

struct MeshWrapper: public Mesh
{
  MeshWrapper (const Mesh& mesh, CloneMode in_clone_mode) : Mesh (mesh, in_clone_mode), clone_mode (in_clone_mode) {}
  MeshWrapper (const MeshWrapper& wrapper) : Mesh (wrapper, CloneMode_Instance), clone_mode (wrapper.clone_mode) {}

  MeshWrapper& operator = (const MeshWrapper& wrapper)
  {
    Assign (wrapper, CloneMode_Instance);
    clone_mode = wrapper.clone_mode;
    return *this;
  }

  CloneMode clone_mode;
};

}

/*
    Описание реализации меш-модели
*/

typedef vector<MeshWrapper> MeshArray;

struct MeshModel::Impl: public InstanceResource
{
  string    name;   //имя модели
  MeshArray meshes; //массив мешей

  Impl ();
  Impl (const Impl&);
};

/*
    MeshModel::Impl::Impl
*/

MeshModel::Impl::Impl ()
{
  meshes.reserve (DEFAULT_MESH_ARRAY_RESERVE);
}

MeshModel::Impl::Impl (const Impl& impl)
  : name (impl.name)
{
  meshes.reserve (impl.meshes.size ());
  
  for (MeshArray::const_iterator i=impl.meshes.begin (), end=impl.meshes.end (); i!=end; ++i)
    meshes.push_back (MeshWrapper (*i, i->clone_mode));
}

/*
    MeshModel
*/

/*
    Конструкторы / деструктор
*/

MeshModel::MeshModel ()
  : impl (new Impl)
  {}

MeshModel::MeshModel (const char* file_name)
  : impl (new Impl)
{
  MeshModelSystemSingleton::Instance ().Load (file_name, *this);
}

MeshModel::MeshModel (const MeshModel& model)
  : impl (new Impl (*model.impl))
  {}

MeshModel::~MeshModel ()
{
  delete impl;
}

/*
    Присваивание
*/

MeshModel& MeshModel::operator = (const MeshModel& model)
{
  MeshModel (model).Swap (*this);
  return *this;
}

/*
    Имя модели
*/

const char* MeshModel::Name () const
{
  return impl->name.c_str ();
}

void MeshModel::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("medialib::geometry::MeshModel::Rename", name);
    
  impl->name = name;
}

/*
    Количество мешей
*/

size_t MeshModel::MeshesCount () const
{
  return impl->meshes.size ();
}

/*
    Получение мешей
*/

const Mesh& MeshModel::Mesh (size_t index) const
{
  if (index >= impl->meshes.size ())
    RaiseOutOfRange ("medialib::geometry::MeshModel::Mesh", "index", index, impl->meshes.size ());
    
  return impl->meshes [index];
}

Mesh& MeshModel::Mesh (size_t index)
{
  return const_cast<medialib::geometry::Mesh&> (const_cast<const MeshModel&> (*this).Mesh (index));
}

/*
    Присоединение мешей
*/

size_t MeshModel::Attach (medialib::geometry::Mesh& mesh, CloneMode mode)
{
  switch (mode)
  {
    case CloneMode_Instance:
    case CloneMode_Copy:
    case CloneMode_Source:
      break;
    default:
      RaiseInvalidArgument ("medialib::geometry::Mesh::Attach", "mode", mode);
      break;
  }

  impl->meshes.push_back (MeshWrapper (mesh, mode));

  return impl->meshes.size () - 1;
}

void MeshModel::Detach (size_t index)
{
  if (index >= impl->meshes.size ())
    return;
    
  impl->meshes.erase (impl->meshes.begin () + index);
}

void MeshModel::DetachAll ()
{
  impl->meshes.clear ();
}

/*
    Загрузка / сохранение
*/

void MeshModel::Load (const char* file_name)
{
  MeshModel (file_name).Swap (*this);
}

void MeshModel::Save (const char* file_name)
{
  MeshModelSystemSingleton::Instance ().Save (file_name, *this);
}

/*
    Обмен
*/

void MeshModel::Swap (MeshModel& model)
{
  stl::swap (model.impl, impl);
}

namespace medialib
{

namespace geometry
{

void swap (MeshModel& model1, MeshModel& model2)
{
  model1.Swap (model2);
}

}

}
