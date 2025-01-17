#ifndef NETWORK_SOCKET_ADDRESS_HEADER
#define NETWORK_SOCKET_ADDRESS_HEADER

#include <network/inet_address.h>

namespace network
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Адрес сокета
///////////////////////////////////////////////////////////////////////////////////////////////////
class SocketAddress
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    SocketAddress  ();
    SocketAddress  (const network::InetAddress& inet_address, unsigned short port);
    SocketAddress  (unsigned short port);
    SocketAddress  (const SocketAddress&);
    ~SocketAddress ();

    SocketAddress& operator = (const SocketAddress&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сетевой адрес / порт
///////////////////////////////////////////////////////////////////////////////////////////////////
    const network::InetAddress& InetAddress () const;
    unsigned short              Port        () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение двух адресов
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const SocketAddress&) const;
    bool operator != (const SocketAddress&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Строковое представление
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ToString () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (SocketAddress&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (SocketAddress&, SocketAddress&);

}

#endif
