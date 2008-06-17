#include "shared.h"

using namespace media::rfx;
using namespace media;
using namespace stl;
using namespace common;

/*
    Описание реализации библиотеки материалов
*/

typedef hash_map<string, Material::Pointer> MaterialMap;

struct MaterialLibrary::Impl
{
  string      name;      //имя модели
  MaterialMap materials; //библиотека материалов
};

/*
    Утилиты
*/

namespace
{

//функция протоколирования по умолчанию
void default_log_handler (const char*)
{
}

}

/*
    Конструкторы / деструктор
*/

MaterialLibrary::MaterialLibrary ()
  : impl (new Impl)
  {}

MaterialLibrary::MaterialLibrary (const char* file_name, const LogHandler& log_handler)
  : impl (new Impl)
{
  Init (file_name, log_handler);
}

MaterialLibrary::MaterialLibrary (const char* file_name)
  : impl (new Impl)
{
  Init (file_name, &default_log_handler);
}

MaterialLibrary::MaterialLibrary (const MaterialLibrary& library)
  : impl (new Impl (*library.impl))
  {}

MaterialLibrary::~MaterialLibrary ()
{
}

void MaterialLibrary::Init (const char* file_name, const LogHandler& log_handler)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    static ComponentLoader loader ("media.rfx.loaders.*");

    MaterialLibraryManager::GetLoader (file_name, SerializerFindMode_ByName)(file_name, *this, log_handler);

    Rename (file_name);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::MaterialLibrary::MaterialLibrary");
    throw;
  }
}

/*
    Присваивание
*/

MaterialLibrary& MaterialLibrary::operator = (const MaterialLibrary& library)
{
  MaterialLibrary (library).Swap (*this);
  return *this;
}

/*
    Имя библиотеки
*/

const char* MaterialLibrary::Name () const
{
  return impl->name.c_str ();
}

void MaterialLibrary::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::MaterialLibrary::Rename", name);
    
  impl->name = name;
}

/*
    Количество мешей / проверка на пустоту
*/

size_t MaterialLibrary::Size () const
{
  return impl->materials.size ();
}

bool MaterialLibrary::IsEmpty () const
{
  return impl->materials.empty ();
}

/*
    Получение итератора
*/

namespace
{

//селектор материала
template <class T>
struct material_selector
{
  template <class T1> T& operator () (T1& value) const { return value.second; }
};

}

MaterialLibrary::Iterator MaterialLibrary::CreateIterator ()
{
  return const_cast<const MaterialLibrary&> (*this).CreateIterator ();
}

MaterialLibrary::Iterator MaterialLibrary::CreateIterator () const
{
  return ConstIterator (impl->materials.begin (), impl->materials.begin (), impl->materials.end (), material_selector<Material::Pointer> ());
}

/*
    Получение идентификатора материала
*/

const char* MaterialLibrary::ItemId (const ConstIterator& i) const
{
  const MaterialMap::iterator* iter = i.target<MaterialMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("media::rfx::MaterialLibrary::ItemId", "iterator", "wrong-type");

  return (*iter)->first.c_str ();
}

/*
    Поиск
*/

Material::Pointer MaterialLibrary::Find (const char* name)
{
  return xtl::const_pointer_cast<Material> (const_cast<const MaterialLibrary&> (*this).Find (name));
}

Material::ConstPointer MaterialLibrary::Find (const char* name) const
{
  if (!name)
    return 0;
    
  MaterialMap::const_iterator iter = impl->materials.find (name);
  
  return iter != impl->materials.end () ? iter->second : 0;
}

/*
    Присоединение материалов
*/

void MaterialLibrary::Attach (const char* id, const Material::Pointer& material)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::rfx::MaterialLibrary::Insert", "id");
    
  impl->materials.insert_pair (id, material);
}

void MaterialLibrary::Detach (const char* id)
{
  if (!id)
    return;
    
  impl->materials.erase (id);
}

void MaterialLibrary::DetachAll ()
{
  impl->materials.clear ();
}

/*
    Очистка
*/

void MaterialLibrary::Clear ()
{
  DetachAll ();
  
  impl->name.clear ();
}

/*
    Загрузка / сохранение
*/

void MaterialLibrary::Load (const char* file_name, const LogHandler& log_handler)
{
  try
  {
    MaterialLibrary (file_name, log_handler).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::MaterialLibrary::Load");
    throw;
  }
}

void MaterialLibrary::Load (const char* file_name)
{
  Load (file_name, &default_log_handler);
}

void MaterialLibrary::Save (const char* file_name, const LogHandler& log_handler)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");    
      
    static ComponentLoader loader ("media.rfx.savers.*");      

    MaterialLibraryManager::GetSaver (file_name, SerializerFindMode_ByName)(file_name, *this, log_handler);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::MaterialLibrary::Save");
    throw;
  }
}

void MaterialLibrary::Save (const char* file_name)
{
  Save (file_name, &default_log_handler);
}

/*
    Обмен
*/

void MaterialLibrary::Swap (MaterialLibrary& library)
{
  swap (library.impl, impl);
}

namespace media
{

namespace rfx
{

void swap (MaterialLibrary& material1, MaterialLibrary& material2)
{
  material1.Swap (material2);
}

}

}
