#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;

/*
    Конструктор / деструктор
*/

SpriteList::SpriteList (size_t sprites_count)
  : sprites (sprites_count)
{
  log.Printf ("Create sprite list (id=%u)", Id ());
}

SpriteList::~SpriteList ()
{
  try
  {
    log.Printf ("Destroy sprite list (id=%u)", Id ());
  }
  catch (...)
  {
    //подавление исключений
  }
}

/*
    Количество спрайтов в списке
*/

size_t SpriteList::GetSize ()
{
  return sprites.size ();
}

/*
    Установка / чтение данных
*/

void SpriteList::SetSprites (size_t first, size_t count, const Sprite* in_sprites)
{
  static const char* METHOD_NAME = "render::mid_level::debug::SpriteList::SetSprites";

  if (!count)
    return;
    
  if (first > sprites.size ())
    throw xtl::make_range_exception (METHOD_NAME, "first", first, sprites.size ());
    
  if (first + count > sprites.size ())
    throw xtl::format_exception<xtl::argument_range_exception> (METHOD_NAME, "Arguments pair first+count=%u > sprites_count=%u",
      first+count, sprites.size ());

  if (!in_sprites)
    throw xtl::make_null_argument_exception (METHOD_NAME, "sprites");
    
  const Sprite* src = in_sprites;
  Sprite*       dst = &sprites [first];

  for (; count--; src++, dst++)
    *dst = *src;
}

void SpriteList::GetSprites (size_t first, size_t count, Sprite* in_sprites)
{
  static const char* METHOD_NAME = "render::mid_level::debug::SpriteList::GetSprites";

  if (!count)
    return;
    
  if (first > sprites.size ())
    throw xtl::make_range_exception (METHOD_NAME, "first", first, sprites.size ());
    
  if (first + count > sprites.size ())
    throw xtl::format_exception<xtl::argument_range_exception> (METHOD_NAME, "Arguments pair first+count=%u > sprites_count=%u",
      first+count, sprites.size ());

  if (!in_sprites)
    throw xtl::make_null_argument_exception (METHOD_NAME, "sprites");
    
  const Sprite* src = &sprites [first];
  Sprite*       dst = in_sprites;

  for (; count--; src++, dst++)
    *dst = *src;
}
