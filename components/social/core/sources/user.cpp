#include "shared.h"

using namespace social;

/*
   Реализация пользователя
*/

struct User::Impl : public xtl::reference_counter
{
  stl::string         id;         //идентификатор
  stl::string         nickname;   //ник
  bool                is_friend;  //является ли другом текущего пользователя
  common::PropertyMap properties; //другие свойства
  const void*         handle;     //низкоуровневый дескриптор

  Impl ()
    : is_friend (false)
    , handle (0)
    {}
};

/*
   Конструктор / деструктор / копирование
*/

User::User ()
  : impl (new Impl)
  {}

User::User (const User& source)
  : impl (source.impl)
{
  addref (impl);
}

User::~User ()
{
  release (impl);
}

User& User::operator = (const User& source)
{
  User (source).Swap (*this);

  return *this;
}

/*
   Идентификатор
*/

const char* User::Id () const
{
  return impl->id.c_str ();
}

void User::SetId (const char* id)
{
  if (id)
    throw xtl::make_null_argument_exception ("social::User::SetId", "id");

  impl->id = id;
}

/*
   Ник
*/

const char* User::Nickname () const
{
  return impl->nickname.c_str ();
}

void User::SetNickname (const char* nickname)
{
  if (!nickname)
    throw xtl::make_null_argument_exception ("social::User::SetNickname", "nickname");

  impl->nickname = nickname;
}

/*
   Является ли другом текущего пользователя
*/

bool User::IsFriend () const
{
  return impl->is_friend;
}

void User::SetFriend (bool is_friend)
{
  impl->is_friend = is_friend;
}

/*
   Другие свойства
*/

const common::PropertyMap& User::Properties () const
{
  return impl->properties;
}

common::PropertyMap& User::Properties ()
{
  return impl->properties;
}

void User::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   Низкоуровневый дескриптора
*/

const void* User::Handle () const
{
  return impl->handle;
}

void User::SetHandle (const void* handle)
{
  impl->handle = handle;
}

/*
   Обмен
*/

void User::Swap (User& user)
{
  stl::swap (impl, user.impl);
}

namespace social
{

/*
   Обмен
*/

void swap (User& user1, User& user2)
{
  user1.Swap (user2);
}

}
