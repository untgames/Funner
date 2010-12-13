#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <common/string.h>

using namespace common;

namespace
{

/*
    Константы
*/

const size_t STRING_OFFSETS_RESERVE_SIZE = 16;  //резервируемое при инициализации число строк
const size_t STRING_BUFFER_RESERVE_SIZE  = 256; //резервируемый размер буфера строк

}

/*
    Реализация массива строк
*/

template <class Char> class BasicStringArray<Char>::Impl: public xtl::reference_counter
{
  public:
///Конструктор
    Impl () : need_update_pointers (false)
    {
      offsets.reserve (STRING_OFFSETS_RESERVE_SIZE);
      string_buffer.reserve (STRING_BUFFER_RESERVE_SIZE);
    }

///Копирование
    Impl (const Impl& buffer) :
      string_buffer (buffer.string_buffer),
      offsets (buffer.offsets),
      need_update_pointers (true) { }      

///Количество строк
    size_t Size () { return offsets.size (); }

///Зарезервированное число строк
    size_t Capacity () { return offsets.capacity (); }

///Размер буфера строк
    size_t BufferSize () { return string_buffer.size (); }
    
///Зарезервированный размер буфера строк
    size_t BufferCapacity () { return string_buffer.capacity (); }

///Добавление строк
    void Insert (size_t index, size_t count, const Char** strings)
    {
      if (count && !strings)
        throw xtl::make_null_argument_exception ("", "string_array");
        
      if (index > offsets.size ())
        throw xtl::make_range_exception ("", "index", index, offsets.size ());
        
      volatile size_t old_buffer_size   = string_buffer.size (),
                      old_offsets_count = offsets.size ();

      if (!count)
        return;

      need_update_pointers = true;

      try
      {
        offsets.reserve (offsets.size () + count);

        for (size_t i=0; i<count; i++)
        {
          const Char* string = strings [i];

          size_t offs = string_buffer.size ();

          if (string)
            string_buffer.append (string);            

          static Char null_buffer [] = {0};            

          string_buffer.append (null_buffer, 1);
          offsets.insert       (offsets.begin () + index + i, offs);
        }
      }
      catch (...)
      {
        string_buffer.erase (old_buffer_size);
        offsets.erase (offsets.begin () + index, offsets.begin () + index + (offsets.size () - old_offsets_count));
        throw;
      }      
    }
    
    size_t Add (size_t count, const Char** strings)
    {
      size_t old_strings_count = offsets.size ();
      
      Insert (old_strings_count, count, strings);
      
      return old_strings_count;
    }
    
///Обновление строки
    void Set (size_t index, const Char* string)
    {
      static const char* METHOD_NAME = "common::BasicStringArray<Char>::Set";
      
      if (!string)
        throw xtl::make_null_argument_exception (METHOD_NAME, "string");
        
      if (index >= offsets.size ())
        throw xtl::make_range_exception (METHOD_NAME, "index", index, offsets.size ());
        
      need_update_pointers = true;
      
      size_t current_string_len = xtl::xstrlen (string_buffer.c_str () + offsets [index]) + 1,
             new_string_len     = xtl::xstrlen (string) + 1;
             
      string_buffer.reserve (string_buffer.size () + new_string_len - current_string_len);

      string_buffer.replace (offsets [index], current_string_len, string, new_string_len); //nothrow (reserve called, scalar types)

      int diff = int (new_string_len - current_string_len);
      
      for (size_t i=index+1; i<offsets.size (); i++)
        offsets [i] += diff;
    }
    
///Удаление строк
    void Remove (size_t first, size_t count)
    {
      if (first >= offsets.size () || !offsets.size ())
        return;

      size_t last = first + count;

      if (last > offsets.size ())
        last = offsets.size ();
        
      size_t buffer_start_offset  = offsets [first],
             buffer_finish_offset = offsets [last-1] + xtl::xstrlen (&string_buffer [offsets [last-1]]) + 1,
             cut_size             = buffer_finish_offset - buffer_start_offset;

      offsets.erase (offsets.begin () + first, offsets.begin () + last);
      string_buffer.erase (buffer_start_offset, cut_size);

      for (size_t i=first, count=offsets.size (); i<count; i++)
        offsets [i] -= cut_size;

      need_update_pointers = true;
    }
    
///Очистка
    void Clear ()
    {
      string_buffer.clear ();
      offsets.clear ();
      pointers.clear ();
      
      need_update_pointers = false;
    }
    
///Получение строки
    const Char* Item (size_t index)
    {
      if (index >= offsets.size ())
        throw xtl::make_range_exception ("common::BasicStringArray<Char>::operator []", "index", index, offsets.size ());

      return string_buffer.c_str () + offsets [index];
    }
    
///Получение массива строк
   const Char** Items ()
   {
     if (offsets.empty ())
     {
       static Char        null_string [1] = {0};
       static const Char* null_array [1]  = {&null_string [0]};
      
       return null_array;
     }
    
     if (need_update_pointers)
     {
       pointers.clear ();
       pointers.resize (offsets.size ());

       typename PointerBuffer::iterator ptr_iter = pointers.begin ();
       const Char*                      base     = string_buffer.c_str ();
      
       for (typename OffsetBuffer::iterator iter=offsets.begin (), end=offsets.end (); iter!=end; ++iter, ++ptr_iter)
         *ptr_iter = base + *iter;

       need_update_pointers = false;
     }

     return &pointers [0];
   }
   
///Получение буфера
   const Char* Buffer () { return string_buffer.c_str (); }

///Резервирование числа строк
    void Reserve (size_t string_count)
    {
      size_t* old_start = &offsets [0];
      
      offsets.reserve (string_count);
      
      if (old_start != &offsets [0])
        need_update_pointers = true;
    }

///Резервирование места в буфере строк
    void ReserveBuffer (size_t size)
    {
      const Char* old_start = string_buffer.c_str ();
      
      string_buffer.reserve (size);
      
      if (old_start != string_buffer.c_str ())
        need_update_pointers = true;
    }
    
  private:
    typedef stl::vector<size_t>      OffsetBuffer;
    typedef stl::vector<const Char*> PointerBuffer;

  private:
    stl::basic_string<Char> string_buffer;        //буфер символов строк
    OffsetBuffer            offsets;              //смещения строк в буфере
    PointerBuffer           pointers;             //массив указателей на строки
    bool                    need_update_pointers; //нужно обновить массив указателей на строки
};

/*
    Конструкторы / деструктор
*/

template <class Char>
BasicStringArray<Char>::BasicStringArray ()
  : impl (new Impl)
{
}

template <class Char>
BasicStringArray<Char>::BasicStringArray (Impl* in_impl)
  : impl (in_impl)
{
}

template <class Char>
BasicStringArray<Char>::BasicStringArray (const BasicStringArray& sa)
  : impl (sa.impl)
{
  addref (impl);
}

template <class Char>
BasicStringArray<Char>::BasicStringArray (size_t string_count, const value_type** string_array)
  : impl (new Impl)
{
  Add (string_count, string_array);
}

template <class Char>
BasicStringArray<Char>::~BasicStringArray ()
{
  release (impl);
}

/*
    Копирование
*/

template <class Char>
BasicStringArray<Char> BasicStringArray<Char>::Clone () const
{
  return BasicStringArray (new Impl (*impl));
}

/*
    Присваивание
*/

template <class Char>
BasicStringArray<Char>& BasicStringArray<Char>::operator = (const BasicStringArray<Char>& sa)
{
  BasicStringArray (sa).Swap (*this);
  
  return *this;
}

/*
    Количество строк / проверка на пустоту
*/

template <class Char>
size_t BasicStringArray<Char>::Size () const
{
  return impl->Size ();
}

template <class Char>
bool BasicStringArray<Char>::IsEmpty () const
{
  return impl->Size () == 0;
}

template <class Char>
size_t BasicStringArray<Char>::Capacity () const
{
  return impl->Capacity ();
}

template <class Char>
size_t BasicStringArray<Char>::BufferSize () const
{
  return impl->BufferSize ();
}

template <class Char>
size_t BasicStringArray<Char>::BufferCapacity () const
{
  return impl->BufferCapacity ();
}

/*
    Резервирование числа строк / резервирование места в буфере строк
*/

template <class Char>
void BasicStringArray<Char>::Reserve (size_t string_count)
{
  impl->Reserve (string_count);
}

template <class Char>
void BasicStringArray<Char>::ReserveBuffer (size_t buffer_size)
{
  impl->ReserveBuffer (buffer_size);
}

/*
    Получение строки / массива строк
*/

template <class Char>
const typename BasicStringArray<Char>::value_type* BasicStringArray<Char>::operator [] (size_t index) const
{
  return impl->Item (index);
}

template <class Char>
const typename BasicStringArray<Char>::value_type** BasicStringArray<Char>::Data () const
{
  return impl->Items ();
}

template <class Char>
const typename BasicStringArray<Char>::value_type* BasicStringArray<Char>::Buffer () const
{
  return impl->Buffer ();
}

/*
    Добавление строк
*/

template <class Char>
BasicStringArray<Char>& BasicStringArray<Char>::operator += (const value_type* s)
{
  Add (1, &s);
  
  return *this;
}

template <class Char>
BasicStringArray<Char>& BasicStringArray<Char>::operator += (const BasicStringArray& sa)
{
  Add (sa);

  return *this;
}

template <class Char>
BasicStringArray<Char> BasicStringArray<Char>::operator + (const value_type* s) const
{
  BasicStringArray res = Clone ();

  return res += s;
}

template <class Char>
BasicStringArray<Char> BasicStringArray<Char>::operator + (const BasicStringArray& sa) const
{
  BasicStringArray res = Clone ();

  return res += sa;
}

template <class Char>
size_t BasicStringArray<Char>::Add (const value_type* string)
{
  return Add (1, &string);
}

template <class Char>
size_t BasicStringArray<Char>::Add (const BasicStringArray& sa)
{
  return Add (sa.Size (), sa.Data ());
}

template <class Char>
size_t BasicStringArray<Char>::Add (size_t string_count, const value_type** string_array)
{
  try
  {
    return impl->Add (string_count, string_array);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::BasicStringArray<Char>::Add(size_t,const value_type**)");
    throw;
  }
}

template <class Char>
void BasicStringArray<Char>::Insert (size_t index, const value_type* string)
{
  Insert (index, 1, &string);
}

template <class Char>
void BasicStringArray<Char>::Insert (size_t index, size_t string_count, const value_type** string_array)
{
  try
  {
    impl->Insert (index, string_count, string_array);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::BasicStringArray<Char>::Insert(size_t,size_t,const value_type**)");
    throw;
  }
}

template <class Char>
void BasicStringArray<Char>::Insert (size_t index, const BasicStringArray& sa)
{
  Insert (index, sa.Size (), sa.Data ());
}

/*
    Обновление строк
*/

template <class Char>
void BasicStringArray<Char>::Set (size_t index, const value_type* string)
{
  try
  {
    impl->Set (index, string);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::BasicStringArray<Char>::Set");
    throw;
  }
}

/*
    Удаление строк
*/

template <class Char>
void BasicStringArray<Char>::Remove (size_t first, size_t count)
{
  impl->Remove (first, count);
}

template <class Char>
void BasicStringArray<Char>::Remove (size_t index)
{
  impl->Remove (index, 1);
}

template <class Char>
void BasicStringArray<Char>::Clear ()
{
  impl->Clear ();
}

/*
    Обмен
*/

template <class Char>
void BasicStringArray<Char>::Swap (BasicStringArray& sa)
{
  stl::swap (impl, sa.impl);
}

namespace common
{

void swap (StringArray& sa1, StringArray& sa2)
{
  sa1.Swap (sa2);
}

void swap (WStringArray& sa1, WStringArray& sa2)
{
  sa1.Swap (sa2);
}

/*
    Принудительная генерация кода для StringArray, WStringArray
*/

template class BasicStringArray<char>;
template class BasicStringArray<wchar_t>;

}
