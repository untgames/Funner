#include "shared.h"

using namespace network;

/*
    Описание реализации адреса сокета
*/

struct SocketAddress::Impl: public xtl::reference_counter
{
  network::InetAddress       inet_address;
  unsigned short             port;
  stl::auto_ptr<stl::string> string;
  
  Impl (unsigned short in_port = 0) : port (in_port) {}
  
  Impl (const network::InetAddress& in_inet_address, unsigned short in_port) : inet_address (in_inet_address), port (in_port) {}
};

/*
    Конструкторы / деструктор / присваивание
*/

SocketAddress::SocketAddress ()
  : impl (new Impl)
{
}

SocketAddress::SocketAddress (const network::InetAddress& inet_address, unsigned short port)
  : impl (new Impl (inet_address, port))
{
}

SocketAddress::SocketAddress (unsigned short port)
  : impl (new Impl (port))
{
}

SocketAddress::SocketAddress (const SocketAddress& address)
  : impl (address.impl)
{
  addref (impl);
}

SocketAddress::~SocketAddress ()
{
  release (impl);
}

SocketAddress& SocketAddress::operator = (const SocketAddress& address)
{
  SocketAddress (address).Swap (*this);

  return *this;
}

/*
    Сетевой адрес / порт
*/

const InetAddress& SocketAddress::InetAddress () const
{
  return impl->inet_address;
}

unsigned short SocketAddress::Port () const
{
  return impl->port;
}

/*
    Сравнение двух адресов
*/

bool SocketAddress::operator == (const SocketAddress& address) const
{
  return impl->inet_address == address.impl->inet_address && impl->port == address.impl->port;
}

bool SocketAddress::operator != (const SocketAddress& address) const
{
  return !(*this == address);
}

/*
    Строковое представление
*/

const char* SocketAddress::ToString () const
{
  if (impl->string)
    return impl->string->c_str ();
    
  impl->string = stl::auto_ptr<stl::string> (new stl::string (common::format ("%s:%u", impl->inet_address.ToString (), impl->port)));

  return impl->string->c_str ();
}

/*
    Обмен
*/

void SocketAddress::Swap (SocketAddress& address)
{
  stl::swap (impl, address.impl);
}

namespace network
{

void swap (SocketAddress& address1, SocketAddress& address2)
{
  address1.Swap (address2);
}

}
