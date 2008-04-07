#include "shared.h"

using namespace common;
using namespace script;

namespace
{

/*
    Константы
*/

const size_t OVERLOADED_INVOKERS_RESERVE_SIZE = 8; //количество резервируемых перегруженных шлюзов

/*
    Диспетчер вызова перегруженных шлюзов
*/

class OverloadedInvoker
{
  public:
      //конструктор
    OverloadedInvoker () : invokers (new InvokerArray)
    {
      invokers->reserve (OVERLOADED_INVOKERS_RESERVE_SIZE);
    }

      //добавление шлюза
    void Insert (const Invoker& invoker)
    {
      invokers->push_back (invoker);
    }
    
      //вызов перегруженного шлюза
    size_t operator () (IStack& stack)
    {
      for (InvokerArray::iterator iter=invokers->begin (), end=invokers->end (); iter!=end; ++iter)
      {
        Invoker& invoker = *iter;

          //попытка вызова шлюза

        try
        {
          return invoker (stack); //если шлюз вызван успешно, то остальные перегрузки проверять не нужно
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

      Raise<RuntimeException> ("script::OverloadedInvoker::operator ()", "Bad overloaded invoker call (%u arguments in stack)", arguments_count);

      return 0;
    }

  private:
    typedef stl::vector<Invoker> InvokerArray;

  private:
    xtl::shared_ptr<InvokerArray> invokers; //перегруженные шлюзы
};

}

namespace script
{

/*
    Создание перегруженного шлюза
*/

Invoker make_invoker (size_t invokers_count, const Invoker* invokers)
{
  OverloadedInvoker overloaded_invoker;

  while (!invokers_count--)
    overloaded_invoker.Insert (*invokers++);

  return overloaded_invoker;
}

Invoker make_invoker (const Invoker& invoker1, const Invoker& invoker2)
{
  OverloadedInvoker overloaded_invoker;
  
  overloaded_invoker.Insert (invoker1);
  overloaded_invoker.Insert (invoker2);
  
  return overloaded_invoker;
}

Invoker make_invoker (const Invoker& invoker1, const Invoker& invoker2, const Invoker& invoker3)
{
  OverloadedInvoker overloaded_invoker;

  overloaded_invoker.Insert (invoker1);
  overloaded_invoker.Insert (invoker2);
  overloaded_invoker.Insert (invoker3);

  return overloaded_invoker;
}

Invoker make_invoker (const Invoker& invoker1, const Invoker& invoker2, const Invoker& invoker3, const Invoker& invoker4)
{
  OverloadedInvoker overloaded_invoker;

  overloaded_invoker.Insert (invoker1);
  overloaded_invoker.Insert (invoker2);
  overloaded_invoker.Insert (invoker3);
  overloaded_invoker.Insert (invoker4);

  return overloaded_invoker;
}

Invoker make_invoker
 (const Invoker& invoker1,
  const Invoker& invoker2,
  const Invoker& invoker3,
  const Invoker& invoker4,
  const Invoker& invoker5)
{
  OverloadedInvoker overloaded_invoker;

  overloaded_invoker.Insert (invoker1);
  overloaded_invoker.Insert (invoker2);
  overloaded_invoker.Insert (invoker3);
  overloaded_invoker.Insert (invoker4);
  overloaded_invoker.Insert (invoker5);

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
  OverloadedInvoker overloaded_invoker;

  overloaded_invoker.Insert (invoker1);
  overloaded_invoker.Insert (invoker2);
  overloaded_invoker.Insert (invoker3);
  overloaded_invoker.Insert (invoker4);
  overloaded_invoker.Insert (invoker5);
  overloaded_invoker.Insert (invoker6);

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
  OverloadedInvoker overloaded_invoker;

  overloaded_invoker.Insert (invoker1);
  overloaded_invoker.Insert (invoker2);
  overloaded_invoker.Insert (invoker3);
  overloaded_invoker.Insert (invoker4);
  overloaded_invoker.Insert (invoker5);
  overloaded_invoker.Insert (invoker6);
  overloaded_invoker.Insert (invoker7);

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
  OverloadedInvoker overloaded_invoker;

  overloaded_invoker.Insert (invoker1);
  overloaded_invoker.Insert (invoker2);
  overloaded_invoker.Insert (invoker3);
  overloaded_invoker.Insert (invoker4);
  overloaded_invoker.Insert (invoker5);
  overloaded_invoker.Insert (invoker6);
  overloaded_invoker.Insert (invoker7);
  overloaded_invoker.Insert (invoker8);

  return overloaded_invoker;
}

}
