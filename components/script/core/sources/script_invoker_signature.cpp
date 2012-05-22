#include "shared.h"

using namespace script;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_PARAMETERS_COUNT = 4;   //количество параметров по умолчанию
const size_t TO_STRING_RESERVE_SIZE   = 256; //количество символов, резервируемых в строке дампа

}

/*
    Реализация дескриптора шлюза
*/

typedef stl::vector<const std::type_info*> InfoArray;

struct InvokerSignature::Impl: public xtl::reference_counter
{
  const std::type_info*      result_type;        //тип результата
  InfoArray                  parameter_types;    //типы параметров  
  stl::auto_ptr<stl::string> string;             //строка с текстом сигнатуры
  bool                       need_update_string; //нужно ли обновлять строку с текстом сигнатуры
  
  Impl ()
    : result_type (&typeid (void))
    , need_update_string (true)
  {
    parameter_types.reserve (DEFAULT_PARAMETERS_COUNT);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

InvokerSignature::InvokerSignature ()
  : impl (new Impl)
{
}

InvokerSignature::InvokerSignature (const InvokerSignature& signature)
  : impl (signature.impl)
{
  addref (impl);
}

InvokerSignature::~InvokerSignature ()
{
  release (impl);
}

InvokerSignature& InvokerSignature::operator = (const InvokerSignature& signature)
{
  InvokerSignature (signature).Swap (*this);
  
  return *this;
}

/*
    Тип возвращаемого значения
*/

const std::type_info& InvokerSignature::ResultType () const
{
  return *impl->result_type;
}

void InvokerSignature::SetResultType (const std::type_info& type)
{
  impl->result_type        = &type;
  impl->need_update_string = true;
}

/*
    Количество параметров
*/

size_t InvokerSignature::ParametersCount () const
{
  return impl->parameter_types.size ();
}

/*
    Типы параметров
*/

const std::type_info& InvokerSignature::ParameterType (size_t index) const
{
  if (index >= impl->parameter_types.size ())
    throw xtl::make_range_exception ("script::InvokerSignature::ParameterType", "index", index, impl->parameter_types.size ());
    
  return *impl->parameter_types [index];
}

void InvokerSignature::SetParameterType (size_t index, const std::type_info& type)
{
  if (index >= impl->parameter_types.size ())
    throw xtl::make_range_exception ("script::InvokerSignature::ParameterType", "index", index, impl->parameter_types.size ());
    
  impl->need_update_string = true;
    
  impl->parameter_types [index] = &type;
}

/*
    Добавление / удаление типов параметров
*/

size_t InvokerSignature::AddParameterType (const std::type_info& type)
{
  impl->parameter_types.push_back (&type);
 
  impl->need_update_string = true;
  
  return impl->parameter_types.size () - 1;
}

void InvokerSignature::RemoveParameterType (size_t index)
{
  if (index >= impl->parameter_types.size ())
    return;
    
  impl->need_update_string = true;

  impl->parameter_types.erase (impl->parameter_types.begin () + index);
}

void InvokerSignature::RemoveAllParameterTypes ()
{
  impl->parameter_types.clear ();

  impl->need_update_string = true;
}

/*
    Приведение к строке
*/

namespace
{

const char* get_type_string (const std::type_info& t)
{
  if (&t == &typeid (unsigned char))  return "uchar";
  if (&t == &typeid (char))           return "char";
  if (&t == &typeid (unsigned short)) return "ushort";
  if (&t == &typeid (signed short))   return "short";
  if (&t == &typeid (unsigned int))   return "uint";
  if (&t == &typeid (signed int))     return "int";
  if (&t == &typeid (unsigned long))  return "ulong";
  if (&t == &typeid (signed long))    return "long";
  if (&t == &typeid (float))          return "float";  
  if (&t == &typeid (double))         return "double";
  if (&t == &typeid (long double))    return "long double";
  if (&t == &typeid (wchar_t))        return "wchar_t";
  if (&t == &typeid (const char*))    return "const char*";
  
  return t.name ();
}

}

const char* InvokerSignature::ToString () const
{
  if (!impl->need_update_string)
    return impl->string->c_str ();
    
  stl::string result;
  
  result.reserve (TO_STRING_RESERVE_SIZE);
  
  result += get_type_string (ResultType ());
  result += " (";
  
  for (InfoArray::const_iterator i=impl->parameter_types.begin (), end=impl->parameter_types.end (); i!=end; ++i)
  {
    if (i != impl->parameter_types.begin ())
      result += ", ";
      
    result += get_type_string (**i);
  }
  
  result += ")";
  
  if (!impl->string) impl->string  = new stl::string (result);
  else               *impl->string = result;

  impl->need_update_string = false;
  
  return impl->string->c_str ();
}

/*
    Сравнение
*/

bool InvokerSignature::operator == (const InvokerSignature& signature) const
{
  return impl->result_type == signature.impl->result_type && impl->parameter_types == signature.impl->parameter_types;
}

bool InvokerSignature::operator != (const InvokerSignature& signature) const
{
  return !(*this == signature);
}

/*
    Обмен
*/

void InvokerSignature::Swap (InvokerSignature& signature)
{
  stl::swap (impl, signature.impl);
}

namespace script
{

void swap (InvokerSignature& signature1, InvokerSignature& signature2)
{
  signature1.Swap (signature2);
}

}
