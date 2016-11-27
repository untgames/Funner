#include "shared.h"

using namespace debug;

/*
    Описание реализации символа
*/

struct Symbol::Impl: public xtl::reference_counter
{
  stl::string name;          //имя символа
  size_t      start_address; //начальный адрес
  size_t      size;          //размер
  
  Impl (const char* in_name, size_t in_start_address, size_t in_size)
    : name (in_name)
    , start_address (in_start_address)
    , size (in_size)
  {
  }
};

/*
    Конструкторы / присваивание / деструктор
*/

Symbol::Symbol ()
  : impl (new Impl ("", 0, 0))
{
}

Symbol::Symbol (const char* name, size_t start_address, size_t size)
{
  if (!name)
    throw xtl::make_null_argument_exception ("debug::Symbol::Symbol(const char*,size_t,size_t)", "name");
    
  impl = new Impl (name, start_address, size);
}

Symbol::Symbol (const Symbol& symbol)
  : impl (symbol.impl)
{
  addref (impl);
}

Symbol::~Symbol ()
{
  release (impl);
}

Symbol& Symbol::operator = (const Symbol& symbol)
{
  Symbol (symbol).Swap (*this);
  return *this;
}

/*
    Имя
*/

const char* Symbol::Name () const
{
  return impl->name.c_str ();
}

void Symbol::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("debug::Symbol::SetName", "name");
    
  impl->name = name;
}

/*
    Начальный адрес
*/

void Symbol::SetStartAddress (size_t addr)
{
  impl->start_address = addr;
}

size_t Symbol::StartAddress () const
{
  return impl->start_address;
}

/*
    Размер
*/

void Symbol::SetSize (size_t size)
{
  impl->size = size;
}

size_t Symbol::Size () const
{
  return impl->size;
}

/*
    Обмен
*/

void Symbol::Swap (Symbol& symbol)
{
  stl::swap (impl, symbol.impl);
}

namespace debug
{

void swap (Symbol& symbol1, Symbol& symbol2)
{
  symbol1.Swap (symbol2);
}

}
