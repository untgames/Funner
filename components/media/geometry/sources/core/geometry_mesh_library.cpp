#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    Описание реализации библиотек мешей
*/

typedef ResourceLibrary<Mesh> MeshLibraryHolder;

struct MeshLibrary::Impl
{
  stl::string       name;   //имя модели
  MeshLibraryHolder meshes; //библиотека мешей
};

/*
    Конструкторы / деструктор
*/

MeshLibrary::MeshLibrary ()
  : impl (new Impl)
  {}

MeshLibrary::MeshLibrary (const char* file_name)
  : impl (new Impl)
{
  Load (file_name);
  Rename (file_name);
}

MeshLibrary::MeshLibrary (const MeshLibrary& model)
  : impl (new Impl (*model.impl))
  {}

MeshLibrary::~MeshLibrary ()
{
}

/*
    Присваивание
*/

MeshLibrary& MeshLibrary::operator = (const MeshLibrary& model)
{
  MeshLibrary (model).Swap (*this);
  return *this;
}

/*
    Имя библиотеки
*/

const char* MeshLibrary::Name () const
{
  return impl->name.c_str ();
}

void MeshLibrary::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::geometry::MeshLibrary::Rename", name);
    
  impl->name = name;
}

/*
    Количество мешей / проверка на пустоту
*/

size_t MeshLibrary::Size () const
{
  return impl->meshes.Size ();
}

bool MeshLibrary::IsEmpty () const
{
  return impl->meshes.IsEmpty ();
}

/*
    Получение итератора
*/

MeshLibrary::Iterator MeshLibrary::CreateIterator ()
{
  return impl->meshes.CreateIterator ();
}

MeshLibrary::ConstIterator MeshLibrary::CreateIterator () const
{
  return impl->meshes.CreateIterator ();
}

/*
    Идентификатор меша в библиотеке
*/

const char* MeshLibrary::ItemId (const ConstIterator& i) const
{
  return impl->meshes.ItemId (i);
}

/*
    Поиск
*/

Mesh* MeshLibrary::Find (const char* name)
{
  return impl->meshes.Find (name);
}

const Mesh* MeshLibrary::Find (const char* name) const
{
  return impl->meshes.Find (name);
}

/*
    Присоединение мешей
*/

void MeshLibrary::Attach (const char* name, Mesh& mesh)
{
  impl->meshes.Insert (name, mesh);
}

void MeshLibrary::Detach (const char* name)
{
  impl->meshes.Remove (name);
}

void MeshLibrary::DetachAll ()
{
  impl->meshes.Clear ();
}

/*
    Очистка библиотеки
*/

void MeshLibrary::Clear ()
{
  DetachAll ();
  
  impl->name.clear ();
}

/*
    Загрузка / сохранение
*/

void MeshLibrary::Load (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("media::MeshLibrary::Load", "file_name");

  try
  {
    static ComponentLoader loader ("media.geometry.loaders.*");

    MeshLibraryManager::GetLoader (file_name, SerializerFindMode_ByName)(file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::MeshLibrary::Load");
    throw;
  }
}

void MeshLibrary::Save (const char* file_name)
{
  try
  {
    static ComponentLoader loader ("media.geometry.savers.*");
    
    MeshLibraryManager::GetSaver (file_name, SerializerFindMode_ByName)(file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::MeshLibrary::Save");
    throw;
  }
}

/*
    Обмен
*/

void MeshLibrary::Swap (MeshLibrary& model)
{
  stl::swap (model.impl, impl);
}

namespace media
{

namespace geometry
{

void swap (MeshLibrary& model1, MeshLibrary& model2)
{
  model1.Swap (model2);
}

}

}
