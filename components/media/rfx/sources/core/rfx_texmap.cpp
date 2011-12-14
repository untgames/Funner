#include "shared.h"

using namespace media::rfx;

namespace
{

/*
    Реестр повторяющихся строк
*/

struct SharedString;

typedef xtl::intrusive_ptr<SharedString>                         SharedStringPtr;
typedef stl::hash_map<stl::hash_key<const char*>, SharedString*> SharedStringMap;
typedef common::Singleton<SharedStringMap>                       SharedStringMapSingleton;

struct SharedString: public xtl::reference_counter
{
  stl::string                value;
  stl::hash_key<const char*> hash;
  
  SharedString (const char* in_value)
    : value (in_value)
    , hash (in_value)
  {
    SharedStringMapSingleton::Instance ()->insert_pair (hash, this);
  }
  
  ~SharedString ()
  {
    try
    {
      SharedStringMapSingleton::Instance ()->erase (hash);
    }
    catch (...)
    {
    }
  }
  
  static SharedStringPtr GetString (const char* value)
  {
    if (!value)
      return SharedStringPtr ();
    
    SharedStringMapSingleton::Instance strings;
    
    SharedStringMap::iterator iter = strings->find (value);
    
    if (iter != strings->end ())
      return iter->second;
      
    return SharedStringPtr (new SharedString (value), false);
  }
};

}

/*
    Описание реализации текстурной карты
*/

struct Texmap::Impl: public xtl::reference_counter
{
  stl::string     image;    //имя текстуры
  SharedStringPtr sampler;  //имя сэмплера
  SharedStringPtr semantic; //имя семантики 
};

/*
    Конструкторы / деструктор / присваивание
*/

Texmap::Texmap ()
  : impl (new Impl)
{
}

Texmap::Texmap (const Texmap& map)
  : impl (map.impl)
{
  addref (map.impl);
}

Texmap::Texmap (Impl* in_impl)
  : impl (in_impl)
{
}

Texmap::~Texmap ()
{
  release (impl);
}

Texmap& Texmap::operator = (const Texmap& map)
{
  Texmap (map).Swap (*this);
  
  return *this;
}

/*
    Идентификатор
*/

size_t Texmap::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Копирование
*/

Texmap Texmap::Clone () const
{
  try
  {
    return Texmap (new Impl (*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rfx::Texmap::Clone");
    throw;
  }
}

/*
    Имя текстуры
*/

void Texmap::SetImage (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Texmap::SetImage", "name");
    
  impl->image = name;
}

const char* Texmap::Image () const
{
  return impl->image.c_str ();
}

/*
    Имя сэмплера
*/

void Texmap::SetSampler (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Texmap::SetSampler", "name");
    
  impl->sampler = SharedString::GetString (name);
}

const char* Texmap::Sampler () const
{
  return impl->sampler ? impl->sampler->value.c_str () : "";
}

/*
    Имя семантики
*/

void Texmap::SetSemantic (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::rfx::Texmap::SetSemantic", "name");
    
  impl->semantic = SharedString::GetString (name);
}

const char* Texmap::Semantic () const
{
  return impl->semantic ? impl->semantic->value.c_str () : "";
}

/*
    Обмен
*/

void Texmap::Swap (Texmap& map)
{
  stl::swap (impl, map.impl);
}

namespace media
{

namespace rfx
{

void swap (Texmap& map1, Texmap& map2)
{
  map1.Swap (map2);
}

}

}
