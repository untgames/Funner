#include "shared.h"

using namespace network;

/*
    Описание реализации URL
*/

struct Url::Impl: public xtl::reference_counter
{
};

/*
    Конструкторы / деструктор / присваивание
*/

Url::Url (const char* spec)
{
  throw xtl::make_not_implemented_exception ("network::Url::Url(const char*)");
}

Url::Url (const char* protocol, const char* host_name, unsigned short port, const char* file)
{
  throw xtl::make_not_implemented_exception ("network::Url::Url(const char*,const char*,unsigned short,const char*)");
}

Url::Url (const char* protocol, const char* host_name, const char* file)
{
  throw xtl::make_not_implemented_exception ("network::Url::Url(const char*,const char*,const char*)");
}

Url::Url (const Url& url)
  : impl (url.impl)
{
  addref (impl);
}

Url::~Url ()
{
  release (impl);
}

Url& Url::operator = (const Url& url)
{
  Url (url).Swap (*this);

  return *this;
}

/*
    Параметры URL
*/

const char* Url::Protocol () const
{
  throw xtl::make_not_implemented_exception ("network::Url::Protocol");
}

const char* Url::Host () const
{
  throw xtl::make_not_implemented_exception ("network::Url::Host");
}

unsigned short Url::Port () const
{
  throw xtl::make_not_implemented_exception ("network::Url::Port");
}

const char* Url::File () const
{
  throw xtl::make_not_implemented_exception ("network::Url::File");
}

const char* Url::Query () const
{
  throw xtl::make_not_implemented_exception ("network::Url::Query");
}

/*
    Параметры запроса
*/

size_t Url::RequestPropertiesCount () const
{
  throw xtl::make_not_implemented_exception ("network::Url::RequestPropertiesCount");
}

void Url::SetRequestProperty (const char* key, const char* value)
{
  throw xtl::make_not_implemented_exception ("network::Url::SetRequestProperty");
}

const char* Url::GetRequestProperty (const char* key) const
{
  throw xtl::make_not_implemented_exception ("network::Url::GetRequestProperty(const char*)");
}

const char* Url::GetRequestProperty (size_t index) const
{
  throw xtl::make_not_implemented_exception ("network::Url::GetRequestProperty(size_t)");
}

int Url::FindRequestProperty (const char* key) const
{
  throw xtl::make_not_implemented_exception ("network::Url::FindRequestProperty");
}

bool Url::HasRequestProperty (const char* key) const
{
  throw xtl::make_not_implemented_exception ("network::Url::HasRequestProperty");
}

void Url::RemoveRequestProperty (const char* key)
{
  throw xtl::make_not_implemented_exception ("network::Url::RemoveRequestProperty(const char*)");
}

void Url::RemoveRequestProperty (size_t index)
{
  throw xtl::make_not_implemented_exception ("network::Url::RemoveRequestProperty(size_t)");
}

void Url::RemoveAllRequestProperties ()
{
  throw xtl::make_not_implemented_exception ("network::Url::RemoveAllRequestProperties");
}

/*
    Приведение к строке
*/

const char* Url::ToString () const
{
  throw xtl::make_not_implemented_exception ("network::Url::ToString");
}

/*
    Сравнение двух URL
*/

bool Url::operator == (const Url& url) const
{
  throw xtl::make_not_implemented_exception ("network::Url::operator ==");
}

bool Url::operator != (const Url& url) const
{
  return !(*this == url);
}

/*
    Обмен
*/

void Url::Swap (Url& url)
{
  stl::swap (impl, url.impl);
}

namespace network
{

void swap (Url& url1, Url& url2)
{
  url1.Swap (url2);
}

}
