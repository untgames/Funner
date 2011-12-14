#include "shared.h"

using namespace media::rfx;
using namespace common;

namespace
{

/*
    Дескриптор материала
*/

struct MaterialDesc
{
  Material    material;
  stl::string name;
  
  MaterialDesc (const char* in_name, const Material& in_material)
    : material (in_material)
    , name (in_name)
  {
  }
};

}

/*
    Описание реализации библиотеки материалов
*/

typedef stl::hash_map<stl::hash_key<const char*>, MaterialDesc> MaterialMap;

struct MaterialLibrary::Impl
{
  stl::string name;      //имя библиотеки
  MaterialMap materials; //библиотека материалов
  
  Impl ()
  {
  }
  
  Impl (const Impl& impl)
    : name (impl.name)
  {
    for (MaterialMap::const_iterator iter=impl.materials.begin (), end=impl.materials.end (); iter!=end; ++iter)
      materials.insert_pair (iter->first, MaterialDesc (iter->second.name.c_str (), iter->second.material.Clone ()));
  }
};

/*
    Конструкторы / деструктор
*/

MaterialLibrary::MaterialLibrary ()
  : impl (new Impl)
{
}

MaterialLibrary::MaterialLibrary (const char* file_name)
  : impl (new Impl)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    static ComponentLoader loader ("media.rfx.material.loaders.*");

    MaterialLibraryManager::GetLoader (file_name, SerializerFindMode_ByName)(file_name, *this);

    SetName (file_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::MaterialLibrary::MaterialLibrary");
    throw;
  }
}

MaterialLibrary::MaterialLibrary (const MaterialLibrary& library)
  : impl (new Impl (*library.impl))
{
}

MaterialLibrary::~MaterialLibrary ()
{
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

void MaterialLibrary::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::MaterialLibrary::SetName", name);
    
  impl->name = name;
}

/*
    Количество материалов / проверка на пустоту
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
  template <class T1> T& operator () (T1& value) const { return value.second.material; }
};

}

MaterialLibrary::Iterator MaterialLibrary::CreateIterator ()
{
  return Iterator (impl->materials.begin (), impl->materials.begin (), impl->materials.end (), material_selector<Material> ());
}

MaterialLibrary::ConstIterator MaterialLibrary::CreateIterator () const
{
  return ConstIterator (impl->materials.begin (), impl->materials.begin (), impl->materials.end (), material_selector<Material> ());
}

/*
    Получение идентификатора материала
*/

const char* MaterialLibrary::ItemId (const ConstIterator& i) const
{
  const MaterialMap::iterator* iter = i.target<MaterialMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("media::rfx::MaterialLibrary::ItemId", "iterator", "wrong-type");

  return (*iter)->second.name.c_str ();
}

/*
    Поиск
*/

Material* MaterialLibrary::Find (const char* name)
{
  return const_cast<Material*> (const_cast<const MaterialLibrary&> (*this).Find (name));
}

const Material* MaterialLibrary::Find (const char* name) const
{
  if (!name)
    return 0;
    
  MaterialMap::const_iterator iter = impl->materials.find (name);
  
  return iter != impl->materials.end () ? &iter->second.material : 0;
}

/*
    Присоединение материалов
*/

void MaterialLibrary::Attach (const char* id, const Material& material)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::rfx::MaterialLibrary::Insert", "id");
    
  impl->materials.insert_pair (id, MaterialDesc (id, material));
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

void MaterialLibrary::Load (const char* file_name)
{
  try
  {
    MaterialLibrary (file_name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::MaterialLibrary::Load");
    throw;
  }
}

void MaterialLibrary::Save (const char* file_name)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");    
      
    static ComponentLoader loader ("media.rfx.material.savers.*");

    MaterialLibraryManager::GetSaver (file_name, SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::MaterialLibrary::Save");
    throw;
  }
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

/*
    Регистрация сериализаторов по умолчанию
*/

void MaterialLibrary::RegisterDefaultSavers ()
{
  static ComponentLoader loader ("media.rfx.material.savers.*");
}

void MaterialLibrary::RegisterDefaultLoaders ()
{
  static ComponentLoader loader ("media.rfx.material.loaders.*");
}
