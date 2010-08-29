#include "shared.h"

using namespace network;

//+mutex на ToString

/*
    Описание реализации адреса в интернете
*/

struct InetAddress::Impl: public xtl::reference_counter
{
  char                       address [16];
  size_t                     address_size;
  stl::auto_ptr<stl::string> string;
  
  Impl ()
    : address_size (4)
  {
    memset (address, 0, sizeof (address));
  }
  
  Impl (size_t in_address_size, const unsigned char* in_address)
  {
    switch (in_address_size)
    {
      case 4:
      case 16:
        break;
      default:
        throw xtl::make_argument_exception ("", "address_size", in_address_size, "Address size must be 4 or 16");
    }
    
    if (!in_address)
      throw xtl::make_null_argument_exception ("", "address");

    address_size = in_address_size;
    
    memset (address, 0, sizeof (address));    
    
    for (size_t i=0; i<address_size; i++)
      address [i] = in_address [i];
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

InetAddress::InetAddress ()
  : impl (new Impl)
{
}

InetAddress::InetAddress (const char* host_name)
{
  throw xtl::make_not_implemented_exception ("network::InetAddress::InetAddress(const char*)");
}

InetAddress::InetAddress (size_t address_size, const unsigned char* address)
{
  try
  {
    impl = new Impl (address_size, address);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::InetAddress::InetAddress(size_t,const unsigned char*)");
    throw;
  }
}

InetAddress::InetAddress (const unsigned char (&address)[4])
{
  try
  {
    impl = new Impl (4, &address [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::InetAddress::InetAddress(const unsigned char (&)[4])");
    throw;
  }
}

InetAddress::InetAddress (const unsigned char (&address)[16])
{
  try
  {
    impl = new Impl (16, &address [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::InetAddress::InetAddress(const unsigned char (&)[16])");
    throw;
  }
}

InetAddress::InetAddress (const InetAddress& address)
  : impl (address.impl)
{
  addref (impl);
}

InetAddress::~InetAddress ()
{
  release (impl);
}

InetAddress& InetAddress::operator = (const InetAddress& address)
{
  InetAddress (address).Swap (*this);

  return *this;
}

/*
    Размер адреса
*/

size_t InetAddress::Size () const
{
  return impl->address_size;
}

/*
    Получение частей адреса
*/

unsigned char InetAddress::operator [] (size_t index) const
{
  if (index >= impl->address_size)
    throw xtl::make_range_exception ("network::InetAddress::operator[]", "index", index, impl->address_size);
    
  return impl->address [index];
}

/*
    Сравнение двух адресов
*/

bool InetAddress::operator == (const InetAddress& address) const
{
  return impl->address_size == address.impl->address_size && !memcmp (impl->address, address.impl->address, impl->address_size * sizeof (unsigned char));
}

bool InetAddress::operator != (const InetAddress& address) const
{
  return !(*this == address);
}

/*
    Строковое представление адреса
*/

const char* InetAddress::ToString () const
{
  if (impl->string)
    return impl->string->c_str ();

  switch (impl->address_size)
  {  
    case 4:
      impl->string = stl::auto_ptr<stl::string> (new stl::string (common::format ("%u.%u.%u.%u", impl->address [3], impl->address [2],
        impl->address [1], impl->address [0])));
      break;
    case 16:
    {
      impl->string = stl::auto_ptr<stl::string> (new stl::string);
      
      impl->string->reserve (16*3);
      
      bool has_skip = false;

      for (size_t i=0; i<16; i++)
      {
        size_t index = 15 - i;
        
        if (!impl->address [index])
        {
          if (!has_skip)
            *impl->string += ":";
          
          has_skip = true;
          
          continue;
        }
        
        has_skip = false;
          
        if (i)
          *impl->string += ":";
          
        *impl->string += common::format ("%02x", impl->address [index]);
      }
      
      if (has_skip)
        *impl->string += "0";
      
      break;
    }
    default:
      throw xtl::format_operation_exception ("network::InetAddress::ToString", "Bad address size %u", impl->address_size);
  }
  
  return impl->string->c_str ();
}

/*
    Стандартные адреса
*/

namespace
{

unsigned char localhost_address [4] = {127, 0, 0, 1};

struct InetAddressesHolder
{
  InetAddress loopback;
  
  InetAddressesHolder () : loopback (localhost_address) {}
};

}

InetAddress InetAddress::Loopback ()
{
  return common::Singleton<InetAddressesHolder>::Instance ()->loopback;
}

/*
    Свойства адреса
*/

bool InetAddress::IsAny () const
{
  throw xtl::make_not_implemented_exception ("network::InetAddress::IsAny");
}

bool InetAddress::IsLoopback () const
{
  throw xtl::make_not_implemented_exception ("network::InetAddress::IsLoopback");
}

bool InetAddress::IsBroadcast () const
{
  throw xtl::make_not_implemented_exception ("network::InetAddress::IsBroadcast");
}

bool InetAddress::IsNone () const
{
  for (size_t i=0; i<impl->address_size; i++)
    if (impl->address [i])
      return false;
      
  return true;
}

bool InetAddress::IsMulticast () const
{
  throw xtl::make_not_implemented_exception ("network::InetAddress::IsMulticast");
}

/*
    Обмен
*/

void InetAddress::Swap (InetAddress& address)
{
  stl::swap (impl, address.impl);
}

namespace network
{

void swap (InetAddress& address1, InetAddress& address2)
{
  address1.Swap (address2);
}

}
