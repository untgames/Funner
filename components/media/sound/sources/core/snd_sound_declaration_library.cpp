#include "shared.h"

using namespace stl;
using namespace common;
using namespace media;

/*
    Описание реализации SoundDeclarationLibrary
*/

typedef ResourceLibrary<SoundDeclaration> SoundDeclarationLibraryHolder;

struct SoundDeclarationLibrary::Impl
{
  string                        name;         //имя библиотеки
  SoundDeclarationLibraryHolder declarations; //библиотека звуков
};

/*
    Конструкторы / деструктор
*/

SoundDeclarationLibrary::SoundDeclarationLibrary ()
  : impl (new Impl)
  {}

SoundDeclarationLibrary::SoundDeclarationLibrary (const char* file_name)
  : impl (new Impl)
{
  if (!file_name)
    RaiseNullArgument ("media::SoundDeclarationLibrary::SoundDeclarationLibrary", "file_name");

  try
  {
    static ComponentLoader loader ("media.sound.loaders.*");

    SoundDeclarationManager::GetLoader (file_name, SerializerFindMode_ByName) (file_name, *this);
    
    Rename  (file_name);
  }
  catch (Exception& exception)
  {
    exception.Touch ("media::SoundDeclarationLibrary::SoundDeclarationLibrary");
    throw;
  }
}

SoundDeclarationLibrary::SoundDeclarationLibrary (const SoundDeclarationLibrary& library)
  : impl (new Impl (*library.impl))
  {}

SoundDeclarationLibrary::~SoundDeclarationLibrary ()
{
}

/*
    Присваивание
*/

SoundDeclarationLibrary& SoundDeclarationLibrary::operator = (const SoundDeclarationLibrary& library)
{
  SoundDeclarationLibrary (library).Swap (*this);
  return *this;
}

/*
    Имя модели
*/

const char* SoundDeclarationLibrary::Name () const
{
  return impl->name.c_str ();
}

void SoundDeclarationLibrary::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::SoundDeclarationLibrary::Rename", name);
    
  impl->name = name;
}

/*
    Количество звуков / проверка на пустоту
*/

size_t SoundDeclarationLibrary::Size () const
{
  return impl->declarations.Size ();
}

bool SoundDeclarationLibrary::IsEmpty () const
{
  return impl->declarations.IsEmpty ();
}

/*
    Получение итератора
*/

SoundDeclarationLibrary::Iterator SoundDeclarationLibrary::CreateIterator ()
{
  return impl->declarations.CreateIterator ();
}

SoundDeclarationLibrary::ConstIterator SoundDeclarationLibrary::CreateIterator () const
{
  return impl->declarations.CreateConstIterator ();
}

/*
    Имя декларации по итератору
*/

const char* SoundDeclarationLibrary::ItemId (const ConstIterator& i) const
{
  return impl->declarations.ItemId (i);
}

/*
    Поиск
*/

SoundDeclaration* SoundDeclarationLibrary::Find (const char* name)
{
  return impl->declarations.Find (name);
}

const SoundDeclaration* SoundDeclarationLibrary::Find (const char* name) const
{
  return impl->declarations.Find (name);
}

/*
    Присоединение звуков
*/

void SoundDeclarationLibrary::Attach (const char* name, SoundDeclaration& decl)
{
  impl->declarations.Insert (name, decl);
}

void SoundDeclarationLibrary::Detach (const char* name)
{
  impl->declarations.Remove (name);
}

void SoundDeclarationLibrary::DetachAll ()
{
  impl->declarations.Clear ();
}

/*
    Загрузка / сохранение
*/

void SoundDeclarationLibrary::Load (const char* file_name)
{
  try
  {
    SoundDeclarationLibrary (file_name).Swap (*this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::SoundDeclarationLibrary::Load");
    throw;
  }
}

void SoundDeclarationLibrary::Save (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("media::SoundDeclarationLibrary::Save", "file_name");
    
  try
  {
    static ComponentLoader loader ("media.sound.savers.*");

    SoundDeclarationManager::GetSaver (file_name, SerializerFindMode_ByName) (file_name, *this);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("media::SoundDeclarationLibrary::Save");
    throw;
  }
}

/*
    Обмен
*/

void SoundDeclarationLibrary::Swap (SoundDeclarationLibrary& library)
{
  swap (library.impl, impl);
}

namespace media
{

void swap (SoundDeclarationLibrary& library1, SoundDeclarationLibrary& library2)
{
  library1.Swap (library2);
}

}
