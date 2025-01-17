#ifndef XTL_ANY_REFERENCE_HEADER
#define XTL_ANY_REFERENCE_HEADER

#include <xtl/any_cast_exception.h>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Легковесный класс для передачи ссылки на параметр произвольного типа
///////////////////////////////////////////////////////////////////////////////////////////////////
class any_reference
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
                       any_reference  ();
    template <class T> any_reference  (T& referenced_object);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип и данные
///////////////////////////////////////////////////////////////////////////////////////////////////
    const std::type_info& type    () const;
    template <class T> T* content () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение к указанному типу (исключение в случае неудачи)
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> T& cast () const;

  private:
    any_reference (const any_reference&); //no impl
    any_reference& operator = (const any_reference&); //no impl
    
  private:
    void*                 data;
    const std::type_info* data_type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Приведение типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T* any_cast (any_reference*);
template <class T> T& any_cast (any_reference&);

#include <xtl/detail/any_reference.inl>

}

#endif

