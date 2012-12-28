#include "shared.h"

using namespace social;

/*
   Реализация таблицы рекордов
*/

struct Leaderboard::Impl : public xtl::reference_counter
{
  stl::string           id;                      //идентификатор
  stl::string           title;                   //название
  Score                 user_score;              //очки залогиненного пользователя
  common::PropertyMap   properties;              //свойства
  ScoreList             scores;                  //очки таблицы
  const void*           handle;                  //низкоуровневый дескриптор
  ReleaseHandleFunction handle_release_function; //функция, вызываемая при освобождении дескриптора

  Impl ()
    : handle (0)
    {}

  ~Impl ()
  {
    if (handle_release_function)
      handle_release_function (handle);
  }
};

/*
   Конструктор / деструктор / копирование
*/

Leaderboard::Leaderboard ()
  : impl (new Impl)
  {}

Leaderboard::Leaderboard (const Leaderboard& source)
  : impl (source.impl)
{
  addref (impl);
}

Leaderboard::~Leaderboard ()
{
  release (impl);
}

Leaderboard& Leaderboard::operator = (const Leaderboard& source)
{
  Leaderboard (source).Swap (*this);

  return *this;
}

/*
   Идентификатор
*/

const char* Leaderboard::Id () const
{
  return impl->id.c_str ();
}

void Leaderboard::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("social::Leaderboard::SetId", "id");

  impl->id = id;
}

/*
   Название
*/

const char* Leaderboard::Title () const
{
  return impl->title.c_str ();
}

void Leaderboard::SetTitle (const char* title)
{
  if (!title)
    throw xtl::make_null_argument_exception ("social::Leaderboard::SetTitle", "title");

  impl->title = title;
}

/*
   Очки залогиненного пользователя
*/

const Score& Leaderboard::UserScore () const
{
  return impl->user_score;
}

void Leaderboard::SetUserScore (const Score& score)
{
  impl->user_score = score;
}

/*
   Другие свойства
*/

const common::PropertyMap& Leaderboard::Properties () const
{
  return const_cast<Leaderboard&> (*this).Properties ();
}

common::PropertyMap& Leaderboard::Properties ()
{
  return impl->properties;
}

void Leaderboard::SetProperties (const common::PropertyMap& properties)
{
  impl->properties = properties;
}

/*
   Рекорды
*/

const ScoreList& Leaderboard::Scores () const
{
  return const_cast<Leaderboard&> (*this).Scores ();
}

ScoreList& Leaderboard::Scores ()
{
  return impl->scores;
}

void Leaderboard::SetScores (const ScoreList& scores)
{
  impl->scores = scores;
}

/*
   Получение/установка низкоуровневого дескриптора
*/

const void* Leaderboard::Handle () const
{
  return impl->handle;
}

void Leaderboard::SetHandle (const void* handle, const ReleaseHandleFunction& release_function)
{
  if (impl->handle_release_function)
    impl->handle_release_function (impl->handle);

  impl->handle = handle;
  impl->handle_release_function = release_function;
}

/*
   Обмен
*/

void Leaderboard::Swap (Leaderboard& source)
{
  stl::swap (impl, source.impl);
}

namespace social
{

/*
   Обмен
*/

void swap (Leaderboard& leaderboard1, Leaderboard& leaderboard2)
{
  leaderboard1.Swap (leaderboard2);
}

}
