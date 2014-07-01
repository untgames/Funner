#include "shared.h"

using namespace media;

/*
   Шрифт
*/

namespace media
{

struct FontDesc::Impl : public xtl::reference_counter
{
  stl::string             source; //имя файла
  size_t                  index;  //индекс используемого шрифта
  xtl::com_ptr<IFontDesc> desc;   //шрифт

  Impl (const char* in_source, IFontDesc* in_desc, size_t in_index)
    : source (in_source)
    , index (in_index)
    , desc (in_desc)
    {}
};

}

/*
   Конструктор / деструктор / присваивание
*/

FontDesc::FontDesc (const char* source, IFontDesc* desc, size_t index)
  : impl (new Impl (source, desc, index))
{
}

FontDesc::FontDesc (const FontDesc& source)
  : impl (source.impl)
{
  addref (impl);
}

FontDesc::~FontDesc ()
{
  release (impl);
}

FontDesc& FontDesc::operator = (const FontDesc& source)
{
  FontDesc (source).Swap (*this);

  return *this;
}

/*
   Имя файла
*/

const char* FontDesc::Source () const
{
  return impl->source.c_str ();
}

/*
   Имя гарнитуры / имя семейства / имя стиля
*/

const char* FontDesc::FamilyName () const
{
  return impl->desc->FamilyName (impl->index);
}

const char* FontDesc::StyleName  () const
{
  return impl->desc->StyleName (impl->index);
}
    
/*
   Создание экземпляра шрифта
*/

Font FontDesc::CreateFont (const FontCreationParams& params) const
{
  return impl->desc->CreateFont (impl->index, params);
}

bool FontDesc::CanCreateFont (const FontCreationParams& params) const
{
  return impl->desc->CanCreateFont (impl->index, params);
}

/*
   Обмен
*/

void FontDesc::Swap (FontDesc& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

/*
   Обмен
*/

void swap (FontDesc& font1, FontDesc& font2)
{
  font1.Swap (font2);
}

}
