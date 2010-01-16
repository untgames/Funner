#include "shared.h"

using namespace media::rfx;
using namespace common;

namespace
{

/*
    Дескриптор эффекта
*/

struct EffectDesc
{
  Effect      effect;
  stl::string name;
  
  EffectDesc (const char* in_name, const Effect& in_effect)
    : effect (in_effect)
    , name (in_name)
  {
  }
};

}

/*
    Описание реализации библиотеки эффектов
*/

typedef stl::hash_map<stl::hash_key<const char*>, EffectDesc> EffectMap;

struct EffectLibrary::Impl
{
  stl::string name;    //имя библиотеки
  EffectMap   effects; //библиотека эффектов
  
  Impl ()
  {
  }
  
  Impl (const Impl& impl)
    : name (impl.name)
  {
    for (EffectMap::const_iterator iter=impl.effects.begin (), end=impl.effects.end (); iter!=end; ++iter)
      effects.insert_pair (iter->first, EffectDesc (iter->second.name.c_str (), iter->second.effect.Clone ()));
  }
};

/*
    Конструкторы / деструктор
*/

EffectLibrary::EffectLibrary ()
  : impl (new Impl)
{
}

EffectLibrary::EffectLibrary (const char* file_name)
  : impl (new Impl)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    static ComponentLoader loader ("media.rfx.effect.loaders.*");

    EffectLibraryManager::GetLoader (file_name, SerializerFindMode_ByName)(file_name, *this);

    SetName (file_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::EffectLibrary::EffectLibrary");
    throw;
  }
}

EffectLibrary::EffectLibrary (const EffectLibrary& library)
  : impl (new Impl (*library.impl))
{
}

EffectLibrary::~EffectLibrary ()
{
}

/*
    Присваивание
*/

EffectLibrary& EffectLibrary::operator = (const EffectLibrary& library)
{
  EffectLibrary (library).Swap (*this);
  return *this;
}

/*
    Имя библиотеки
*/

const char* EffectLibrary::Name () const
{
  return impl->name.c_str ();
}

void EffectLibrary::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::EffectLibrary::SetName", name);
    
  impl->name = name;
}

/*
    Количество эффектов / проверка на пустоту
*/

size_t EffectLibrary::Size () const
{
  return impl->effects.size ();
}

bool EffectLibrary::IsEmpty () const
{
  return impl->effects.empty ();
}

/*
    Получение итератора
*/

namespace
{

//селектор эффекта
template <class T>
struct Effect_selector
{
  template <class T1> T& operator () (T1& value) const { return value.second.effect; }
};

}

EffectLibrary::Iterator EffectLibrary::CreateIterator ()
{
  return Iterator (impl->effects.begin (), impl->effects.begin (), impl->effects.end (), Effect_selector<Effect> ());
}

EffectLibrary::ConstIterator EffectLibrary::CreateIterator () const
{
  return ConstIterator (impl->effects.begin (), impl->effects.begin (), impl->effects.end (), Effect_selector<Effect> ());
}

/*
    Получение идентификатора эффекта
*/

const char* EffectLibrary::ItemId (const ConstIterator& i) const
{
  const EffectMap::iterator* iter = i.target<EffectMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("media::rfx::EffectLibrary::ItemId", "iterator", "wrong-type");

  return (*iter)->second.name.c_str ();
}

/*
    Поиск
*/

Effect* EffectLibrary::Find (const char* name)
{
  return const_cast<Effect*> (const_cast<const EffectLibrary&> (*this).Find (name));
}

const Effect* EffectLibrary::Find (const char* name) const
{
  if (!name)
    return 0;
    
  EffectMap::const_iterator iter = impl->effects.find (name);
  
  return iter != impl->effects.end () ? &iter->second.effect : 0;
}

/*
    Присоединение эффектов
*/

void EffectLibrary::Attach (const char* id, const Effect& effect)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::rfx::EffectLibrary::Insert", "id");
    
  impl->effects.insert_pair (id, EffectDesc (id, effect));
}

void EffectLibrary::Detach (const char* id)
{
  if (!id)
    return;
    
  impl->effects.erase (id);
}

void EffectLibrary::DetachAll ()
{
  impl->effects.clear ();
}

/*
    Очистка
*/

void EffectLibrary::Clear ()
{
  DetachAll ();
  
  impl->name.clear ();
}

/*
    Загрузка / сохранение
*/

void EffectLibrary::Load (const char* file_name)
{
  try
  {
    EffectLibrary (file_name).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::EffectLibrary::Load");
    throw;
  }
}

void EffectLibrary::Save (const char* file_name)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");    

    static ComponentLoader loader ("media.rfx.effect.savers.*");

    EffectLibraryManager::GetSaver (file_name, SerializerFindMode_ByName)(file_name, *this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("media::rfx::EffectLibrary::Save");
    throw;
  }
}

/*
    Обмен
*/

void EffectLibrary::Swap (EffectLibrary& library)
{
  swap (library.impl, impl);
}

namespace media
{

namespace rfx
{

void swap (EffectLibrary& effect1, EffectLibrary& effect2)
{
  effect1.Swap (effect2);
}

}

}
