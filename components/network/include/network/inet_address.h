#ifndef NETWORK_INET_ADDRESS_HEADER
#define NETWORK_INET_ADDRESS_HEADER

#include <cstddef>

namespace network
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интернет адрес
///////////////////////////////////////////////////////////////////////////////////////////////////
class InetAddress
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    InetAddress  ();    
    InetAddress  (const char* host_name);
    InetAddress  (size_t address_size, const unsigned char* address);
    InetAddress  (const unsigned char (&address)[4]);
    InetAddress  (const unsigned char (&address)[16]);
    InetAddress  (const InetAddress&);
    ~InetAddress ();

    InetAddress& operator = (const InetAddress&);        

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер адреса
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение частей адреса
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char operator [] (size_t index) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение двух адресов
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const InetAddress&) const;
    bool operator != (const InetAddress&) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Строковое представление адреса
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* ToString () const;        
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Стандартные адреса
///////////////////////////////////////////////////////////////////////////////////////////////////
    static InetAddress Loopback ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Свойства адреса
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsAny       () const;
    bool IsLoopback  () const;
    bool IsBroadcast () const;
    bool IsNone      () const;
    bool IsMulticast () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (InetAddress&);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (InetAddress&, InetAddress&);

}

#endif
