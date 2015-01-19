#include "shared.h"

using namespace media;
using namespace common;

/*
    Описание реализации библиотек параметров создания шрифта
*/

typedef ResourceLibrary<FontCreationParams> FontCreationParamsLibraryHolder;

struct FontCreationParamsLibrary::Impl
{
  stl::string                     name;                    //имя библиотеки
  FontCreationParamsLibraryHolder params;                  //библиотека параметров создания шрифта
  size_t                          default_horizontal_dpi;  //целевое разрешение создания шрифтов по умолчанию
  size_t                          default_vertical_dpi;    //целевое разрешение создания шрифтов по умолчанию

  Impl ()
    : default_horizontal_dpi (0)
    , default_vertical_dpi (0)
    {}
};

/*
    Конструкторы / деструктор
*/

FontCreationParamsLibrary::FontCreationParamsLibrary ()
  : impl (new Impl)
  {}

FontCreationParamsLibrary::FontCreationParamsLibrary (const FontCreationParamsLibrary& model)
  : impl (new Impl (*model.impl))
  {}

FontCreationParamsLibrary::~FontCreationParamsLibrary ()
{
}

/*
    Присваивание
*/

FontCreationParamsLibrary& FontCreationParamsLibrary::operator = (const FontCreationParamsLibrary& model)
{
  FontCreationParamsLibrary (model).Swap (*this);
  return *this;
}

/*
    Имя библиотеки
*/

const char* FontCreationParamsLibrary::Name () const
{
  return impl->name.c_str ();
}

void FontCreationParamsLibrary::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::FontCreationParamsLibrary::Rename", name);
    
  impl->name = name;
}

/*
    Количество параметров создания шрифта / проверка на пустоту
*/

size_t FontCreationParamsLibrary::Size () const
{
  return impl->params.Size ();
}

bool FontCreationParamsLibrary::IsEmpty () const
{
  return impl->params.IsEmpty ();
}

/*
    Получение итератора
*/

FontCreationParamsLibrary::Iterator FontCreationParamsLibrary::CreateIterator ()
{
  return impl->params.CreateIterator ();
}

FontCreationParamsLibrary::ConstIterator FontCreationParamsLibrary::CreateIterator () const
{
  return impl->params.CreateIterator ();
}

/*
    Идентификатор параметров создания шрифта в библиотеке
*/

const char* FontCreationParamsLibrary::ItemId (const ConstIterator& i) const
{
  return impl->params.ItemId (i);
}

/*
    Поиск
*/

FontCreationParams* FontCreationParamsLibrary::Find (const char* name)
{
  return impl->params.Find (name);
}

const FontCreationParams* FontCreationParamsLibrary::Find (const char* name) const
{
  return impl->params.Find (name);
}

/*
    Присоединение параметров создания шрифта
*/

void FontCreationParamsLibrary::Attach (const char* name, FontCreationParams& params)
{
  impl->params.Insert (name, params);
}

void FontCreationParamsLibrary::Detach (const char* name)
{
  impl->params.Remove (name);
}

void FontCreationParamsLibrary::DetachAll ()
{
  impl->params.Clear ();
}

/*
    Очистка библиотеки
*/

void FontCreationParamsLibrary::Clear ()
{
  DetachAll ();
  
  impl->name.clear ();
}

/*
   Целевое разрешение создания шрифтов по умолчанию
*/

size_t FontCreationParamsLibrary::DefaultHorizontalDpi () const
{
  return impl->default_horizontal_dpi;
}

size_t FontCreationParamsLibrary::DefaultVerticalDpi () const
{
  return impl->default_vertical_dpi;
}

void FontCreationParamsLibrary::SetDefaultHorizontalDpi (size_t horizontal_dpi)
{
  impl->default_horizontal_dpi = horizontal_dpi;
}

void FontCreationParamsLibrary::SetDefaultVerticalDpi (size_t vertical_dpi)
{
  impl->default_vertical_dpi = vertical_dpi;
}

/*
    Обмен
*/

void FontCreationParamsLibrary::Swap (FontCreationParamsLibrary& model)
{
  stl::swap (model.impl, impl);
}

namespace media
{

void swap (FontCreationParamsLibrary& model1, FontCreationParamsLibrary& model2)
{
  model1.Swap (model2);
}

}
