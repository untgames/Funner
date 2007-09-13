#include "shared.h"

using namespace stl;
using namespace common;
using namespace media;

const size_t DEFAULT_SOUND_SOURCE_ARRAY_RESERVE = 32;

/*
    Описание реализации библиотеки звуков
*/

typedef SharedResourceHolder<SoundSource> SoundSourceHolder;
typedef vector<SoundSourceHolder>         SoundSourceArray;

struct SoundSourceLibrary::Impl: public SharedResource
{
  string           name;          //имя модели
  SoundSourceArray sound_sources; //массив материалов

  Impl ();
  Impl (const Impl&);
};

/*
    SoundSourceModel::Impl::Impl
*/

SoundSourceLibrary::Impl::Impl ()
{
  sound_sources.reserve (DEFAULT_SOUND_SOURCE_ARRAY_RESERVE);
}

SoundSourceLibrary::Impl::Impl (const Impl& impl)
  : name (impl.name)
{
  sound_sources.reserve (impl.sound_sources.size ());
  
  for (SoundSourceArray::const_iterator i = impl.sound_sources.begin (), end = impl.sound_sources.end (); i != end; ++i)
    sound_sources.push_back (SoundSourceHolder (*i, ForceClone));
}

/*
    SoundSourceLibrary
*/

/*
    Конструкторы / деструктор
*/

SoundSourceLibrary::SoundSourceLibrary ()
  : impl (new Impl)
  {}

SoundSourceLibrary::SoundSourceLibrary (const char* file_name)
  : impl (new Impl)
{
  if (!file_name)
    RaiseNullArgument ("media::SoundSourceLibrary::SoundSourceLibrary", "file_name");
    
  try
  {
    SoundSourceSystemSingleton::Instance ().Load (file_name, *this);
    
    Rename (file_name);
  }
  catch (common::Exception& exception)
  {
    exception.Raise ("media::SoundSourceLibrary::SoundSourceLibrary");
  }
}

SoundSourceLibrary::SoundSourceLibrary (const SoundSourceLibrary& library)
  : impl (new Impl (*library.impl))
  {}

SoundSourceLibrary::~SoundSourceLibrary ()
{
  delete impl;
}

/*
    Присваивание
*/

SoundSourceLibrary& SoundSourceLibrary::operator = (const SoundSourceLibrary& library)
{
  SoundSourceLibrary (library).Swap (*this);
  return *this;
}

/*
    Имя модели
*/

const char* SoundSourceLibrary::Name () const
{
  return impl->name.c_str ();
}

void SoundSourceLibrary::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::SoundSourceLibrary::Rename", name);
    
  impl->name = name;
}

/*
    Количество мешей
*/

size_t SoundSourceLibrary::SoundSourceCount () const
{
  return impl->sound_sources.size ();
}

/*
    Получение мешей
*/

const SoundSource& SoundSourceLibrary::SoundSource (size_t index) const
{
  if (index >= impl->sound_sources.size ())
    RaiseOutOfRange ("media::SoundSourceLibrary::SoundSource", "index", index, impl->sound_sources.size ());
    
  return impl->sound_sources [index].Resource ();
}

SoundSource& SoundSourceLibrary::SoundSource (size_t index)
{
  return const_cast<media::SoundSource&> (const_cast<const SoundSourceLibrary&> (*this).SoundSource (index));
}

/*
    Присоединение мешей
*/

size_t SoundSourceLibrary::Attach (media::SoundSource& sound_source, CloneMode mode)
{
  impl->sound_sources.push_back (SoundSourceHolder (sound_source, mode));

  return impl->sound_sources.size () - 1;
}

void SoundSourceLibrary::Detach (size_t index)
{
  if (index >= impl->sound_sources.size ())
    return;
    
  impl->sound_sources.erase (impl->sound_sources.begin () + index);
}

void SoundSourceLibrary::DetachAll ()
{
  impl->sound_sources.clear ();
}

/*
    Загрузка / сохранение
*/

void SoundSourceLibrary::Load (const char* file_name)
{
  SoundSourceLibrary (file_name).Swap (*this);
}

void SoundSourceLibrary::Save (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("media::SoundSourceLibrary::Save", "file_name");
    
  try
  {
    SoundSourceSystemSingleton::Instance ().Save (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Raise ("media::SoundSourceLibrary::Save");
  }
}

/*
    Обмен
*/

void SoundSourceLibrary::Swap (SoundSourceLibrary& library)
{
  stl::swap (library.impl, impl);
}

namespace media
{

void swap (SoundSourceLibrary& library1, SoundSourceLibrary& library2)
{
  library1.Swap (library2);
}

}
