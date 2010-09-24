#include "shared.h"

using namespace script;

/*
===================================================================================================
    Invoker
===================================================================================================
*/

/*
    Константы
*/

namespace
{

const size_t OVERLOADS_RESERVE_SIZE = 2; //резервируемое число перегрузок

/*
    Перегрузка
*/

struct Overload: public xtl::reference_counter
{
  InvokerSignature        signature; //сигнатура
  stl::auto_ptr<IInvoker> invoker;   //шлюз
  
  Overload (IInvoker* in_invoker)
  {
    if (!in_invoker)
      throw xtl::make_null_argument_exception ("script::Overload::Overload", "invoker");
      
    invoker   = in_invoker;
    signature = invoker->GetSignature ();
  }
};

}

/*
    Описание реализации шлюза
*/

typedef xtl::intrusive_ptr<Overload> OverloadPtr;
typedef stl::vector<OverloadPtr>     OverloadArray;

struct Invoker::Impl: public xtl::reference_counter
{
  OverloadArray overloads;
 
  Impl ()
  {
    overloads.reserve (OVERLOADS_RESERVE_SIZE);
  }
};

/*
    Конструкторы / деструктор
*/

Invoker::Invoker ()
  : impl (new Impl)
{
}

Invoker::Invoker (const Invoker& invoker)
  : impl (invoker.impl)
{
  addref (impl);
}

Invoker::~Invoker ()
{
  release (impl);
}

void Invoker::Init (IInvoker* overload)
{
  try
  {
    impl = new Impl;
    
    try
    {
      AddOverload (overload);
    }
    catch (...)
    {
      delete impl;
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    delete overload;
    e.touch ("script::Invoker::Init");
    throw;
  }
  catch (...)
  {
    delete overload;    
    throw;
  }
}

Invoker& Invoker::operator = (const Invoker& invoker)
{
  Invoker (invoker).Swap (*this);
  return *this;
}

/*
    Перегрузки
*/

size_t Invoker::OverloadsCount () const
{
  return impl->overloads.size ();
}

const InvokerSignature& Invoker::OverloadSignature (size_t index) const
{
  if (index >= impl->overloads.size ())
    throw xtl::make_range_exception ("script::Invoker::OverloadSignature", "index", index, impl->overloads.size ());
    
  return impl->overloads [index]->signature;
}

IInvoker* Invoker::OverloadInvoker (size_t index) const
{
  if (index >= impl->overloads.size ())
    throw xtl::make_range_exception ("script::Invoker::OverloadInvoker", "index", index, impl->overloads.size ());
    
  return &*impl->overloads [index]->invoker;
}

/*
    Добавление и удаление перегрузок
*/

size_t Invoker::AddOverload (IInvoker* invoker)
{
  try
  {
    impl->overloads.push_back (OverloadPtr (new Overload (invoker), false));
    
    return impl->overloads.size () - 1;
  }
  catch (xtl::exception& e)
  {
    e.touch ("script::Invoker::AddOverload");
    throw;
  }
}

size_t Invoker::AddOverloads (const Invoker& invoker)
{
  try
  {
    impl->overloads.insert (impl->overloads.end (), invoker.impl->overloads.begin (), invoker.impl->overloads.end ());
    
    return impl->overloads.size () - invoker.impl->overloads.size ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("script::Invoker::AddOverload");
    throw;
  }
}

void Invoker::RemoveOverload (size_t index)
{
  impl->overloads.erase (impl->overloads.begin () + index);
}

void Invoker::RemoveAllOverloads ()
{
  impl->overloads.clear ();
}

/*
    Вызов
*/

size_t Invoker::operator () (IStack& stack) const
{
  try
  {
    switch (impl->overloads.size ())
    {
      case 0:
        return 0;
      case 1:
        return (*impl->overloads.front ()->invoker)(stack);
      default:
      {
        for (volatile OverloadArray::iterator iter=impl->overloads.begin (), end=impl->overloads.end (); iter!=end; ++iter)
        {
          Overload& overload = **iter;

            //попытка вызова шлюза

          try
          {
            return (*overload.invoker)(stack); //если шлюз вызван успешно, то остальные перегрузки проверять не нужно
          }
          catch (script::ArgumentException&)
          {
            //поглощение исключений приведения типов аргументов
          }
        }

          //если шлюз не найден, то генерируем исключение о недопустимой перегрузке
          
        size_t arguments_count = stack.Size ();

        if (arguments_count)
          arguments_count--; //удаление имени функции из стека        
          
        stl::string overloads_string;
        
        overloads_string.reserve (impl->overloads.size ());
        
        for (OverloadArray::iterator iter=impl->overloads.begin (), end=impl->overloads.end (); iter!=end; ++iter)
        {
          Overload& overload = **iter;
          
          if (overloads_string.empty ()) overloads_string += "    could be ";
          else                           overloads_string += "\n          or ";

          overloads_string += overload.signature.ToString ();
        }

        throw xtl::format_exception<RuntimeException> ("", "Bad overloaded invoker call (%u arguments in stack)\n%s", arguments_count, overloads_string.c_str ());
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("script::Invoker::operator()(IStack&)");
    throw;
  }
}

size_t Invoker::operator () (size_t overload_index, IStack& stack) const
{
  if (overload_index >= impl->overloads.size ())
    throw xtl::make_range_exception ("script::Invoker::operator()(size_t,IStack&)", "overload_index", overload_index, impl->overloads.size ());

  try
  {
    return (*impl->overloads [overload_index]->invoker)(stack);
  }
  catch (xtl::exception& e)
  {
    e.touch ("script::Invoker::operator()(index=%u, signature=<%s>)", overload_index, impl->overloads [overload_index]->signature.ToString ());
    throw;
  }
}

/*
    Обмен
*/

void Invoker::Swap (Invoker& invoker)
{
  stl::swap (impl, invoker.impl);
}

namespace script
{

void swap (Invoker& invoker1, Invoker& invoker2)
{
  invoker1.Swap (invoker1);
}

}

/*
===================================================================================================
    SimpleInvoker
===================================================================================================
*/

InvokerSignature SimpleInvoker::GetSignature ()
{
  InvokerSignature signature;
  
  signature.SetResultType    (xtl::get_type<size_t> ());
  signature.AddParameterType (xtl::get_type<IStack> ());
  
  return signature;
}

/*
===================================================================================================
    Создание перегруженного шлюза
===================================================================================================
*/

namespace script
{

Invoker make_invoker (size_t invokers_count, const Invoker* invokers)
{
  Invoker overloaded_invoker;

  while (!invokers_count--)
    overloaded_invoker.AddOverloads (*invokers++);

  return overloaded_invoker;
}

Invoker make_invoker (const Invoker& invoker1, const Invoker& invoker2)
{
  Invoker overloaded_invoker;
  
  overloaded_invoker.AddOverloads (invoker1);
  overloaded_invoker.AddOverloads (invoker2);
  
  return overloaded_invoker;
}

Invoker make_invoker (const Invoker& invoker1, const Invoker& invoker2, const Invoker& invoker3)
{
  Invoker overloaded_invoker;

  overloaded_invoker.AddOverloads (invoker1);
  overloaded_invoker.AddOverloads (invoker2);
  overloaded_invoker.AddOverloads (invoker3);

  return overloaded_invoker;
}

Invoker make_invoker (const Invoker& invoker1, const Invoker& invoker2, const Invoker& invoker3, const Invoker& invoker4)
{
  Invoker overloaded_invoker;

  overloaded_invoker.AddOverloads (invoker1);
  overloaded_invoker.AddOverloads (invoker2);
  overloaded_invoker.AddOverloads (invoker3);
  overloaded_invoker.AddOverloads (invoker4);

  return overloaded_invoker;
}

Invoker make_invoker
 (const Invoker& invoker1,
  const Invoker& invoker2,
  const Invoker& invoker3,
  const Invoker& invoker4,
  const Invoker& invoker5)
{
  Invoker overloaded_invoker;

  overloaded_invoker.AddOverloads (invoker1);
  overloaded_invoker.AddOverloads (invoker2);
  overloaded_invoker.AddOverloads (invoker3);
  overloaded_invoker.AddOverloads (invoker4);
  overloaded_invoker.AddOverloads (invoker5);

  return overloaded_invoker;
}

Invoker make_invoker
 (const Invoker& invoker1,
  const Invoker& invoker2,
  const Invoker& invoker3,
  const Invoker& invoker4,
  const Invoker& invoker5,
  const Invoker& invoker6)
{
  Invoker overloaded_invoker;

  overloaded_invoker.AddOverloads (invoker1);
  overloaded_invoker.AddOverloads (invoker2);
  overloaded_invoker.AddOverloads (invoker3);
  overloaded_invoker.AddOverloads (invoker4);
  overloaded_invoker.AddOverloads (invoker5);
  overloaded_invoker.AddOverloads (invoker6);

  return overloaded_invoker;
}

Invoker make_invoker
 (const Invoker& invoker1,
  const Invoker& invoker2,
  const Invoker& invoker3,
  const Invoker& invoker4,
  const Invoker& invoker5,
  const Invoker& invoker6,
  const Invoker& invoker7)
{
  Invoker overloaded_invoker;

  overloaded_invoker.AddOverloads (invoker1);
  overloaded_invoker.AddOverloads (invoker2);
  overloaded_invoker.AddOverloads (invoker3);
  overloaded_invoker.AddOverloads (invoker4);
  overloaded_invoker.AddOverloads (invoker5);
  overloaded_invoker.AddOverloads (invoker6);
  overloaded_invoker.AddOverloads (invoker7);

  return overloaded_invoker;
}

Invoker make_invoker
 (const Invoker& invoker1,
  const Invoker& invoker2,
  const Invoker& invoker3,
  const Invoker& invoker4,
  const Invoker& invoker5,
  const Invoker& invoker6,
  const Invoker& invoker7,
  const Invoker& invoker8)
{
  Invoker overloaded_invoker;

  overloaded_invoker.AddOverloads (invoker1);
  overloaded_invoker.AddOverloads (invoker2);
  overloaded_invoker.AddOverloads (invoker3);
  overloaded_invoker.AddOverloads (invoker4);
  overloaded_invoker.AddOverloads (invoker5);
  overloaded_invoker.AddOverloads (invoker6);
  overloaded_invoker.AddOverloads (invoker7);
  overloaded_invoker.AddOverloads (invoker8);

  return overloaded_invoker;
}

}
