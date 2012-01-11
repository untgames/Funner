#include "shared.h"

using namespace social;

/*
   Реализация достижения в таблице рекордов
*/

struct Score::Impl : public xtl::reference_counter
{
  stl::string         player_id;       //идентификатор игрока, которому принадлежит достижение
  stl::string         leaderboard_id;  //идентификатор таблицы рекордов, которой принадлежит достижение
  stl::string         user_data;       //данные, ассоциированные с достижением
  stl::string         formatted_value; //текстовое представление значения
  double              value;           //значение
  size_t              rank;            //ранг в таблице
  common::PropertyMap properties;      //другие свойства
  const void*         handle;          //низкоуровневый дескриптор

  Impl ()
    : value (0)
    , rank (0)
    , handle (0)
    {}
};

/*
   Конструктор / деструктор / копирование
*/

Score::Score ()
  : impl (new Impl)
  {}

Score::Score (const Score& source)
  : impl (source.impl)
{
  addref (impl);
}

Score::~Score ()
{
  release (impl);
}

Score& Score::operator = (const Score& source)
{
  Score (source).Swap (*this);

  return *this;
}

/*
   Идентификатор игрока, которому принадлежит достижение
*/

const char* Score::PlayerId () const
{
  return impl->player_id.c_str ();
}

void Score::SetPlayerId (const char* player_id)
{
  if (!player_id)
    throw xtl::make_null_argument_exception ("social::Score::SetPlayerId", "player_id");

  impl->player_id = player_id;
}

/*
   Идентификатор таблицы рекордов, которой принадлежит достижение
*/

const char* Score::LeaderboardId () const
{
  return impl->leaderboard_id.c_str ();
}

void Score::SetLeaderboardId (const char* leaderboard_id)
{
  if (!leaderboard_id)
    throw xtl::make_null_argument_exception ("social::Score::SetLeaderboardId", "leaderboard_id");

  impl->leaderboard_id = leaderboard_id;
}

/*
   Данные, ассоциированные с достижением
*/

const char* Score::UserData () const
{
  return impl->user_data.c_str ();
}

void Score::SetUserData (const char* user_data)
{
  if (user_data)
    throw xtl::make_null_argument_exception ("social::Score::SetUserData", "user_data");

  impl->user_data = user_data;
}

/*
   Значение
*/

double Score::Value () const
{
  return impl->value;
}

void Score::SetValue (double value)
{
  impl->value = value;
}

/*
   Текстовое представление значения
*/

const char* Score::FormattedValue () const
{
  return impl->formatted_value.c_str ();
}

void Score::SetFormattedValue (const char* formatted_value)
{
  if (formatted_value)
    throw xtl::make_null_argument_exception ("social::Score::SetFormattedValue", "formatted_value");

  impl->formatted_value = formatted_value;
}

/*
   Ранг в таблице
*/

size_t Score::Rank () const
{
  return impl->rank;
}

void Score::SetRank (size_t rank)
{
  impl->rank = rank;
}

/*
   Другие свойства
*/

const common::PropertyMap& Score::Properties () const
{
  return impl->properties;
}

common::PropertyMap& Score::Properties ()
{
  return impl->properties;
}

void Score::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   Низкоуровневый дескриптор
*/

const void* Score::Handle () const
{
  return impl->handle;
}

void Score::SetHandle (const void* handle)
{
  impl->handle = handle;
}

/*
   Обмен
*/

void Score::Swap (Score& source)
{
  stl::swap (source.impl, impl);
}

namespace social
{

/*
   Обмен
*/

void swap (Score& score1, Score& score2)
{
  score1.Swap (score2);
}

}
