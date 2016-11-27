#include "shared.h"

using namespace store;

///Реализация продукта
struct Product::Impl : public xtl::reference_counter
{
  stl::string           description;             //описание
  stl::string           id;                      //идентификатор
  common::PropertyMap   properties;              //другие свойства
  const void*           handle;                  //низкоуровневый дескриптор
  ReleaseHandleFunction handle_release_function; //функция, вызываемая при освобождении дескриптора

  Impl ()
    : handle (0)
    {}

  ~Impl ()
  {
    if (handle_release_function)
      handle_release_function (handle);
  }
};

/*
   Конструктор / деструктор / копирование
*/

Product::Product ()
  : impl (new Impl)
  {}

Product::Product (const Product& source)
  : impl (source.impl)
{
  addref (impl);
}

Product::~Product ()
{
  release (impl);
}

Product& Product::operator = (const Product& source)
{
  Product (source).Swap (*this);

  return *this;
}

/*
   Описание
*/

const char* Product::Description () const
{
  return impl->description.c_str ();
}

void Product::SetDescription (const char* description)
{
  if (!description)
    throw xtl::make_null_argument_exception ("store::Product::SetDescription", "description");

  impl->description = description;
}

/*
   Идентификатор
*/

const char* Product::Id () const
{
  return impl->id.c_str ();
}

void Product::SetId (const char* id) const
{
  if (!id)
    throw xtl::make_null_argument_exception ("store::Product::SetId", "id");

  impl->id = id;
}

/*
   Другие свойства
*/

const common::PropertyMap& Product::Properties () const
{
  return impl->properties;
}

common::PropertyMap& Product::Properties ()
{
  return impl->properties;
}

void Product::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   Низкоуровневый дескриптора
*/

const void* Product::Handle () const
{
  return impl->handle;
}

void Product::SetHandle (const void* handle, const ReleaseHandleFunction& release_function)
{
  if (impl->handle_release_function)
    impl->handle_release_function (impl->handle);

  impl->handle = handle;
  impl->handle_release_function = release_function;
}

/*
   Обмен
*/

void Product::Swap (Product& source)
{
  stl::swap (source.impl, impl);
}

namespace store
{

/*
   Обмен
*/

void swap (Product& product1, Product& product2)
{
  product1.Swap (product2);
}

}
