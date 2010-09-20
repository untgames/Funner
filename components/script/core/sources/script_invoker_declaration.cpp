#include "shared.h"

using namespace script;

/*
    Реализация дескриптора шлюза
*/

struct InvokerDeclaration::Impl: public xtl::reference_counter
{
};

/*
    Конструкторы / деструктор / присваивание
*/

InvokerDeclaration::InvokerDeclaration (const Invoker& invoker)
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::InvokerDeclaration");
}

InvokerDeclaration::InvokerDeclaration (const InvokerDeclaration& decl)
  : impl (decl.impl)
{
  addref (impl);
}

InvokerDeclaration::~InvokerDeclaration ()
{
  release (impl);
}

InvokerDeclaration& InvokerDeclaration::operator = (const InvokerDeclaration& decl)
{
  InvokerDeclaration (decl).Swap (*this);
  
  return *this;
}

/*
    Тип возвращаемого значения
*/

const xtl::type_info& InvokerDeclaration::ResultType () const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::ResultType");
}

/*
    Количество параметров
*/

size_t InvokerDeclaration::ParametersCount () const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::ParametersCount");
}

/*
    Получение параметра
*/

const xtl::type_info& InvokerDeclaration::ParameterType (size_t index) const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::ParameterType");
}

/*
    Количество перегрузок
*/

size_t InvokerDeclaration::OverloadsCount () const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::OverloadsCount");
}

/*
    Получение перегрузки
*/

InvokerDeclaration InvokerDeclaration::Overload (size_t index) const
{
  throw xtl::make_not_implemented_exception ("script::InvokerDeclaration::Overload");
}

/*
    Обмен
*/

void InvokerDeclaration::Swap (InvokerDeclaration& decl)
{
  stl::swap (impl, decl.impl);
}

namespace script
{

void swap (InvokerDeclaration& decl1, InvokerDeclaration& decl2)
{
  decl1.Swap (decl2);
}

}
