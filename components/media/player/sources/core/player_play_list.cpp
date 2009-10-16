#include "shared.h"

using namespace media::player;

/*
    Описание реализации списка проигрывания
*/

struct PlayList::Impl
{
};

/*
    Конструкторы / деструктор / присваивание
*/

PlayList::PlayList ()
{
}

PlayList::PlayList (const PlayList&)
{
}

PlayList::~PlayList ()
{
}

PlayList& PlayList::operator = (const PlayList&)
{
}

/*
    Копирование
*/

PlayList PlayList::Clone () const
{
}

/*
    Количество источников
*/

size_t PlayList::Size () const
{
}

/*
    Получение источников
*/

const char** PlayList::Items () const
{
}

const char* PlayList::Item (size_t index) const
{
}

/*
    Добавление и удаление источников
*/

size_t PlayList::AddSource (const char* source_name)
{
}

void PlayList::RemoveSource (size_t source_index)
{
}

void PlayList::RemoveSource (const char* source_name)
{
}

void PlayList::Clear ()
{
}

/*
    Обмен
*/

void PlayList::Swap (PlayList&);

namespace media
{

namespace player
{

void swap (PlayList&, PlayList&)
{
}

}

}
