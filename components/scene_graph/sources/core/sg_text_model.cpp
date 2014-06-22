#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации текстовой модели
*/

typedef xtl::signal<void (TextModel& sender, TextModelEvent event_id)> TextModelSignal;
typedef xtl::uninitialized_storage<CharDesc>                           CharArray;

struct TextModel::Impl
{
  media::FontLibrary font_library;                 //библиотека шрифтов
  CharArray          chars;                        //массив символов
  stl::string        material;                     //имя материала
  size_t             material_hash;                //хэш имени материала
  stl::string        texture_semantic;             //имя текстурной семантики
  size_t             texture_semantic_hash;        //хэш имени текстурной семантики
  TextModelSignal    signals [TextModelEvent_Num]; //сигналы модели 
  bool               need_rebuild_chars;           //требуется обновление таблицы символов

/// Конструктор
  Impl (const media::FontLibrary& in_font_library)
    : font_library (in_font_library)
    , material_hash (0xffffffff)
    , texture_semantic_hash (0xffffffff)
    , need_rebuild_chars ()
  {
  }

/// Оповещение о событии
  void Notify (TextModel& sender, TextModelEvent event_id)
  {
    try
    {
      if (!signals [event_id])
        return;

      signals [event_id] (sender, event_id);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Конструктор / деструктор
*/

TextModel::TextModel (const media::FontLibrary& font_library)
  : impl (new Impl (font_library))
{
}

TextModel::~TextModel ()
{
}

/*
    Материал
*/

void TextModel::SetMaterial (const char* in_material)
{
  if (!in_material)
    throw xtl::make_null_argument_exception ("scene_graph::TextModel::SetMaterial", "material");

  size_t hash = common::strhash (in_material);

  if (hash == impl->material_hash)
    return;

  impl->material      = in_material;  
  impl->material_hash = hash;

  UpdateNotify ();
}

const char* TextModel::Material () const
{
  return impl->material.c_str ();
}

size_t TextModel::MaterialHash () const
{
  return impl->material_hash;
}

/*
    Семантика текстуры-маски
*/

void TextModel::SetTextureSemantic (const char* in_texture_semantic)
{
  if (!in_texture_semantic)
    throw xtl::make_null_argument_exception ("scene_graph::TextModel::SetTextureSemantic", "texture_semantic");

  size_t hash = common::strhash (in_texture_semantic);

  if (hash == impl->texture_semantic_hash)
    return;

  impl->texture_semantic      = in_texture_semantic;  
  impl->texture_semantic_hash = hash;

  UpdateNotify ();
}

const char* TextModel::TextureSemantic () const
{
  return impl->texture_semantic.c_str ();
}

size_t TextModel::TextureSemanticHash () const
{
  return impl->texture_semantic_hash;
}

/*
    Перестроение таблицы символов
*/

void TextModel::RebuildChars ()
{
  if (!impl->need_rebuild_chars)
    return;

  try
  {
    RebuildCharsCore ();

    impl->need_rebuild_chars = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::TextModel::RebuildChars");
    throw;
  }
}

/*
    Получение символов
*/

size_t TextModel::CharsCount () const
{
  const_cast<TextModel&> (*this).RebuildChars ();

  return impl->chars.size ();
}

const CharDesc* TextModel::Chars () const
{
  const_cast<TextModel&> (*this).RebuildChars ();

  return impl->chars.data ();
}

CharDesc* TextModel::CharsForUpdate ()
{
  return impl->chars.data ();
}

/*
    Изменение символов
*/

void TextModel::ResizeChars (size_t count)
{
  impl->chars.resize (count);
}

void TextModel::UpdateCharsNotify ()
{
  if (impl->need_rebuild_chars)
    return;

  impl->need_rebuild_chars = true;

  impl->Notify (*this, TextModelEvent_AfterCharDescsUpdate);

  UpdateNotify ();
}

/*
    Резервирование символов
*/

size_t TextModel::CharsCapacity () const
{
  return impl->chars.capacity ();
}

void TextModel::ReserveChars (size_t count)
{
  impl->chars.reserve (count);
}

/*
    Подписка на события модели
*/

xtl::connection TextModel::RegisterEventHandler (TextModelEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case TextModelEvent_AfterCharDescsUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::TextModel::RegisterEventHandler", "event", event);
  }
  
  return impl->signals [event].connect (handler);
}

/*
    Библиотека шрифтов
*/

const media::FontLibrary& TextModel::FontLibrary () const
{
  return impl->font_library;
}

media::FontLibrary& TextModel::FontLibrary ()
{
  return impl->font_library;
}

/*
    Динамическая диспетчеризация
*/

void TextModel::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}
