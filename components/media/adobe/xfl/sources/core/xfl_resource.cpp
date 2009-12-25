#include "shared.h"

using namespace media::adobe::xfl;

/*
    Описание реализации ресурса
*/

struct Resource::Impl : public xtl::reference_counter
{
  stl::string name; //имя ресурса
  stl::string path; //путь к файлу
};

/*
    Конструкторы / деструктор / присваивание
*/

Resource::Resource ()
  : impl (new Impl)
  {}
  
Resource::Resource (const Resource& source)
  : impl (source.impl)
{
  addref (impl);
}

Resource::~Resource ()
{
  release (impl);
}

Resource& Resource::operator = (const Resource& source)
{
  Resource (source).Swap (*this);

  return *this;
}

/*
   Имя
*/

const char* Resource::Name () const
{
  return impl->name.c_str ();
}

void Resource::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Resource::SetName", "name");

  impl->name = name;
}

/*
   Путь к ресурсу
*/

const char* Resource::Path () const
{
  return impl->path.c_str ();
}

void Resource::SetPath (const char* path)
{
  if (!path)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Resource::SetPath", "path");

  impl->path = path;
}

/*
   Обмен
*/

void Resource::Swap (Resource& resource)
{
  stl::swap (impl, resource.impl);
}

namespace media
{

namespace adobe
{

namespace xfl
{

/*
   Обмен
*/

void swap (Resource& resource1, Resource& resource2)
{
  resource1.Swap (resource2);
}

}

}

}
