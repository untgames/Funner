#include "shared.h"

using namespace medialib::geometry;
using namespace medialib;
using namespace stl;
using namespace common;

const size_t DEFAULT_MESH_ARRAY_RESERVE = 32;

/*
    Описание реализации меш-модели
*/

typedef SharedResourceHolder<Mesh> MeshHolder;
typedef vector<MeshHolder>         MeshArray;

struct MeshModel::Impl: public SharedResource
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
    meshes.push_back (MeshHolder (*i, ForceClone));
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
  if (!file_name)
    RaiseNullArgument ("medialib::MeshModel::MeshModel", "file_name");

  try
  {
    MeshSystemSingleton::Instance ().Load (file_name, *this);
    
    Rename (file_name);
  }
  catch (common::Exception& exception)
  {
    exception.Raise ("medialib::MeshModel::MeshModel");
  }
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
    
  return impl->meshes [index].Resource ();
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
  impl->meshes.push_back (MeshHolder (mesh, mode));

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
  try
  {
    MeshSystemSingleton::Instance ().Save (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Raise ("medialib::MeshModel::Save");
  }
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
