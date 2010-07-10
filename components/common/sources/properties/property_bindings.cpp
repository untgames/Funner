#if 0

#include "shared.h"

using namespace common;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_BINDINGS_ARRAY_RESERVE_SIZE = 8; //резервируемое число связываний

/*
    Связывание
*/

const size_t UNASSIGNED_PROPERTY_INDEX = ~0u;

struct Binding: public xtl::reference_counter
{
  stl::auto_ptr<IPropertySetter> setter; //функция установки свойства
  stl::auto_ptr<IPropertyGetter> getter; //функция получения свойства
  stl::string                    name;   //имя свойства
  size_t                         index;  //индекс свойства

  Binding (const char* in_name)
    : name (in_name)
    , index (UNASSIGNED_PROPERTY_INDEX)
  {
  }
};

typedef xtl::intrusive_ptr<Binding> BindingPtr;
typedef stl::vector<BindingPtr>     BindingArray;

}

/*
    Описание реализации связываний свойств
*/

struct PropertyBindings::Impl: public xtl::reference_counter
{
  PropertyLayout layout;   //расположение свойств
  BindingArray   bindings; //связывания  
  
///Конструктор
  Impl ()
  {
    bindings.reserve (DEFAULT_BINDINGS_ARRAY_RESERVE_SIZE);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

PropertyBindings::PropertyBindings ()
  : impl (new Impl)
{
}

PropertyBindings::PropertyBindings (const PropertyBindings& in_impl)
  : impl (in_impl)
{
  addref (impl);
}

PropertyBindings::~PropertyBindings ()
{
  release (impl);
}

PropertyBindings& PropertyBindings::operator = (const PropertyBindings& bindings)
{
  PropertyBindings (bindings).Swap (*this);

  return *this;
}

/*
    Количество связываний
*/

size_t PropertyBindings::Size () const
{
}

/*
    Связывание свойств
*/

void PropertyBindings::BindProperty (const char* name, IPropertyGetter* getter)
{
}

void PropertyBindings::BindProperty (const char* name, IPropertySetter* setter)
{
}

void PropertyBindings::UnbindProperty (const char* name)
{
}

void PropertyBindings::Clear ()
{
}

/*
    Обновление связываний
*/

void PropertyBindings::Update ()
{
}

void PropertyBindings::Commit () const
{
}

/*
    Обмен
*/

void PropertyBindings::Swap (PropertyBindings& bindings)
{
  stl::swap (impl, bindings.impl);
}

namespace common
{

void swap (PropertyBindings& bindings1, PropertyBindings& bindings2)
{
  bindings1.Swap (bindings2);
}

}

#endif