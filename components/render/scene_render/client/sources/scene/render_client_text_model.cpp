#include "shared.h"

#include <media/font.h>

#include <shared/font_manager.h>

using namespace render::scene;
using namespace render::scene::client;

/*
    Константы
*/

const size_t INVALID_INDEX = ~0u;

namespace render {
namespace scene {
namespace client {

/*
    Кэш рендеринга шрифтов
*/

/// Дескриптор ссылки на символ
struct CharDescRef
{
  const scene_graph::CharDesc* desc;             //дескриптор символа
  const RasterizedGlyphInfo*   rasterized_glyph; //растеризованный глиф
  size_t                       image_index; //индекс текстового блока

  CharDescRef (const scene_graph::CharDesc& in_desc) : desc (&in_desc), rasterized_glyph (), image_index (INVALID_INDEX) {}

  CharDescRef& operator = (const scene_graph::CharDesc& in_desc)
  {
    desc             = &in_desc;
    rasterized_glyph = 0;
    image_index      = INVALID_INDEX;

    return *this;
  }
};

typedef xtl::uninitialized_storage<interchange::SpriteDesc> SpriteBuffer;
typedef stl::vector<CharDescRef>                            CharArray;

struct FontRenderingTempCache
{
  SpriteBuffer sprites;
  CharArray    chars;
};

}}}

namespace
{

/*
    Константы
*/

const char*  DEFAULT_BATCH_NAME                = "";     //имя пакета для рендеринга текста по умолчанию
const size_t DEFAULT_RESERVED_SPRITES_COUNT    = 32;     //резервируемое количество спрайтов по умолчанию
const size_t DEFAULT_RESERVED_FONT_TEXT_BLOCKS = 4;      //резервируемое количество текстовых блоков

/*
    Текстовая модель
*/

class TextModel: public VisualModel
{
  public:
///Конструктор
    TextModel (scene_graph::TextModel& model, SceneManager& manager)
      : VisualModel (model, manager, interchange::NodeType_SpriteList)
      , font_manager (manager.Client ().FontManager ())
      , on_update_fonts_connection (model.RegisterEventHandler (scene_graph::TextModelEvent_AfterFontsUpdate, xtl::bind (&TextModel::UpdateFontsNotify, this)))
      , on_update_descs_connection (model.RegisterEventHandler (scene_graph::TextModelEvent_AfterCharDescsUpdate, xtl::bind (&TextModel::UpdateDescsNotify, this)))
      , need_update_fonts (true)
      , need_update_descs (true)
      , material_name_hash ()
      , texture_semantic_hash ()
      , cache (GetRenderingCache (manager))
      , sprites (cache->sprites)
      , chars (cache->chars)
    {
      font_text_blocks.reserve (DEFAULT_RESERVED_FONT_TEXT_BLOCKS);
    }

///Исходный узел
    scene_graph::TextModel& SourceNode () const { return static_cast<scene_graph::TextModel&> (Node::SourceNode ()); }

  protected:
///Реализация синхронизации
    void UpdateCore (client::Context& context)
    {
      try
      {
        VisualModel::UpdateCore (context);

        scene_graph::TextModel& model = SourceNode ();

          //проверка обновления материала и текстурной семантики

        size_t current_material_name_hash    = model.MaterialHash (),
               current_texture_semantic_hash = model.TextureSemanticHash ();

        if (current_material_name_hash != material_name_hash || current_texture_semantic_hash != texture_semantic_hash)
        {
          RemoveAllTextBlocks (context);

          need_update_fonts = true;
        }

          //проверка обновления шрифтов и символов

        if (!need_update_fonts && !need_update_descs)
          return;

          //копирование и сортировка символов

        const size_t chars_count = model.CharsCount ();

        chars.assign (model.Chars (), model.Chars () + chars_count);

        stl::sort (chars.begin (), chars.end (), CharSorterByFont ());

          //обновление шрифтов

        if (need_update_fonts)
        {
          UpdateFonts (context, model);

          need_update_fonts     = false;
          need_update_descs     = true;
          material_name_hash    = current_material_name_hash;
          texture_semantic_hash = current_texture_semantic_hash;
        }

          //обновление символов

        if (need_update_descs)
        {
          UpdateDescs (context, model);

          need_update_descs = false;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::TextModel::UpdateCore");
        throw;
      }
    }

  private:
/// Дескриптор текстового блока с одинаковым шрифтом
    struct FontTextBlock: public xtl::reference_counter
    {
      size_t          font_id;                //идентификатор шрифта
      bool            used;                   //флаг используемости блока (обновляется во время изменения шрифтов)
      FontMaterialPtr font_material;          //материал шрифта
      size_t          reserved_sprites_count; //резервируемое количество спрайтов в блоке
    };

    typedef xtl::intrusive_ptr<FontTextBlock> FontTextBlockPtr;
    typedef stl::vector<FontTextBlockPtr>     FontTextBlockArray;

  private:
    struct CharSorterByFont       { bool operator () (const CharDescRef& char1, const CharDescRef& char2) const { return char1.desc->font < char2.desc->font; } };
    struct CharSorterByImageIndex { bool operator () (const CharDescRef& char1, const CharDescRef& char2) const { return char1.image_index < char2.image_index; } };

/// Обработчики событий текстовой модели
    void UpdateDescsNotify () { need_update_descs = true; }
    void UpdateFontsNotify () { need_update_fonts = true; }

/// Инициализация кэша
    static FontRenderingTempCachePtr GetRenderingCache (SceneManager& manager)
    {
      try
      {
        FontManager& font_manager = manager.Client ().FontManager ();

        FontRenderingTempCachePtr cache = font_manager.FontRenderingTempCache ();

        if (cache)
          return cache;

        cache = FontRenderingTempCachePtr (new FontRenderingTempCache);

        font_manager.SetFontRenderingTempCache (cache);

        return cache;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::GetRenderingCache");
        throw;
      }
    }

/// Поиск текстового блока
    size_t FindFontTextBlock (const media::Font& font)
    {
      size_t id = font.Id ();

      for (FontTextBlockArray::iterator iter=font_text_blocks.begin (), end=font_text_blocks.end (); iter!=end; ++iter)
        if (*iter && (*iter)->font_id == id)
          return iter - font_text_blocks.begin ();

      return INVALID_INDEX;
    }

/// Добавление текстового блока
    size_t AddFontTextBlock (client::Context& context, const media::Font& font, scene_graph::TextModel& model)
    {
      try
      {
          //получение материала шрифта

        FontMaterialPtr font_material = font_manager.CreateFontMaterial (font, model.Material (), model.TextureSemantic ());

          //определение базового индекса блока

        size_t base_index = 0, free_slots_count = 0;

        for (FontTextBlockArray::iterator iter=font_text_blocks.begin (), end=font_text_blocks.end (); iter!=end;)
        {
          if (*iter)
          {
            ++iter;
            continue;
          }

          FontTextBlockArray::iterator base_iter = iter;

          for (++iter; iter != end && !*iter; ++iter);

          size_t slots_count = iter - base_iter;

          if (slots_count > free_slots_count)
          {
            free_slots_count = slots_count;
            base_index       = base_iter - font_text_blocks.begin ();
          }
        }

        size_t derived_materials_count = font_material->MaterialsCount ();

        if (derived_materials_count > free_slots_count)
        {
            //если свободного места не достаточно - выделяем дополнительную область в конце
 
          base_index = font_text_blocks.size ();

          font_text_blocks.resize (font_text_blocks.size () + derived_materials_count);
        }

          //создание текстовых блоков

        try
        {
          for (FontTextBlockArray::iterator iter=font_text_blocks.begin () + base_index, end=iter + derived_materials_count; iter!=end; ++iter)
          {
            FontTextBlockPtr font_text_block (new FontTextBlock, false);

            font_text_block->font_id                = font.Id ();
            font_text_block->used                   = true;
            font_text_block->font_material          = font_material;
            font_text_block->reserved_sprites_count = 0;

            *iter = font_text_block;
          }

            //синхронизация с сервером

          object_id_t id = Id ();

          for (size_t i=0; i<derived_materials_count; i++)
          {
            context.CreateSpriteList      (id, base_index + i, interchange::SpriteMode_Oriented, interchange::PrimitiveUsage_Batching, math::vec3f (0, 1.0f, 0), DEFAULT_BATCH_NAME);
            context.SetSpriteListMaterial (id, base_index + i, font_material->MaterialName (i));
          }

          return base_index;
        }
        catch (...)
        {
          for (FontTextBlockArray::iterator iter=font_text_blocks.begin () + base_index, end=iter + derived_materials_count; iter!=end; ++iter)
            *iter = FontTextBlockPtr ();

          throw;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::TextModel::AddFontTextBlock");
        throw;
      }
    }

/// Удаление всеъ текстовых блоков
    void RemoveAllTextBlocks (client::Context& context)
    {
      interchange::object_id_t id = Id ();

      for (FontTextBlockArray::iterator iter=font_text_blocks.begin (); iter!=font_text_blocks.end (); ++iter)
        if (*iter)
          context.RemoveSpriteList (id, iter - font_text_blocks.begin ());

      font_text_blocks.clear ();
    }

/// Обновление шрифтов
    void UpdateFonts (client::Context& context, scene_graph::TextModel& model)
    {
      try
      {
          //очистка флагов

        for (FontTextBlockArray::iterator iter=font_text_blocks.begin (), end=font_text_blocks.end (); iter!=end; ++iter)
          if (*iter)
            (*iter)->used = false;

          //обновление шрифтов

        if (!chars.empty ())
        {         
          for (const CharDescRef *char_desc = &chars [0], *char_desc_end = char_desc + chars.size (); char_desc!=char_desc_end;)
          {
            const media::Font* font = char_desc->desc->font;

            for (++char_desc; char_desc != char_desc_end && char_desc->desc->font == font; ++char_desc);

            if (!font)
              continue;

            size_t font_text_block_index = FindFontTextBlock (*font);

            if (font_text_block_index != INVALID_INDEX)
            {
                //обновление флагов существующего шрифта

              size_t font_id = font->Id ();

              for (FontTextBlockArray::iterator iter=font_text_blocks.begin () + font_text_block_index, end=font_text_blocks.end (); iter!=end && *iter && (*iter)->font_id == font_id; ++iter)
                (*iter)->used = true;
            }
            else
            {
                //создание нового текстового блока

              font_text_block_index = AddFontTextBlock (context, *font, model);
            }
          }
        }

          //сброс кэша

        for (FontTextBlockArray::iterator iter=font_text_blocks.begin (); iter!=font_text_blocks.end ();)
          if (*iter && !(*iter)->used)
          {
            context.RemoveSpriteList (Id (), iter - font_text_blocks.begin ());

            font_text_blocks.erase (iter);
          }
          else ++iter;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::TextModel::UpdateFonts");
        throw;
      }
    }

/// Обновление символов
    void UpdateDescs (client::Context& context, scene_graph::TextModel& model)
    {
      try
      {
          //сброс флагов

        for (FontTextBlockArray::iterator iter=font_text_blocks.begin (); iter!=font_text_blocks.end (); ++iter)
          if (*iter)
            (*iter)->used = false;        

        if (!chars.empty ())
        {         
            //обновление глифов

          for (CharDescRef *char_desc = &chars [0], *char_desc_end = char_desc + chars.size (); char_desc!=char_desc_end;)
          {
            const media::Font* src_font = char_desc->desc->font;

            CharDescRef* first_desc = char_desc;

            for (++char_desc; char_desc != char_desc_end && char_desc->desc->font == src_font; ++char_desc);

            if (!src_font)
              continue;

            size_t font_text_block_index = FindFontTextBlock (*src_font);

            if (font_text_block_index == INVALID_INDEX)
              continue;

            FontTextBlockPtr* text_blocks = &font_text_blocks [font_text_block_index];
            FontTextBlock&    text_block  = *text_blocks [0];
            Font&             font        = text_block.font_material->Font ();

            size_t glyphs_count = font.GlyphsCount (), first_glyph = font.FirstGlyphCode ();

            const RasterizedGlyphInfo* glyphs = font.RasterizedGlyphs ();

            for (CharDescRef* i=first_desc; i!=char_desc; ++i)
            {
              size_t glyph_index = i->desc->code - first_glyph;

              if (glyph_index >= glyphs_count)
                continue;

              i->rasterized_glyph = &glyphs [glyph_index];

              size_t image_index = i->rasterized_glyph->image_index;

              i->image_index = image_index;
            }

              //обновление списков спрайтов

            if (font.TexturesCount () > 1)
            {
              UpdateSpriteList (context, first_desc, char_desc, font_text_block_index);
            }
            else
            {
              UpdateSpriteList (context, first_desc, char_desc, font_text_block_index, text_block);
            }
          }
        }

          //чистка неиспользуемых текстовых блоков

        for (FontTextBlockArray::iterator iter=font_text_blocks.begin (); iter!=font_text_blocks.end (); ++iter)
        {
          if (!*iter || (*iter)->used)
            continue;

          context.SetSpriteListBuffer (Id (), iter - font_text_blocks.begin (), 0, (*iter)->reserved_sprites_count);          
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::client::TextModel::UpdateDescs");
        throw;
      }
    }

/// Обновление списков спрайтов
    void UpdateSpriteList (client::Context& context, CharDescRef* first, CharDescRef* last, size_t text_block_index)
    {
        //сортировка по индексу изображения

      stl::sort (first, last, CharSorterByImageIndex ());

        //группировка и обновление списков спрайтов

      while (first != last)
      {
        const size_t image_index = first->image_index;

        CharDescRef* i = first + 1;

        for (; i!=last && image_index == i->image_index; ++i);

        if (image_index != INVALID_INDEX)
        {
          size_t         subid      = text_block_index + image_index;
          FontTextBlock& text_block = *font_text_blocks [subid];

          UpdateSpriteList (context, first, i, subid, text_block);
        }

        first = i;
      }
    }

    void UpdateSpriteList (client::Context& context, const CharDescRef* first, const CharDescRef* last, size_t subid, FontTextBlock& text_block)
    {
        //генерация спрайтов

      sprites.resize (last - first, false);
      
      const CharDescRef*       char_desc = first;
      interchange::SpriteDesc* sprite    = sprites.data ();

      for (;char_desc != last; ++char_desc)
      {
        const RasterizedGlyphInfo& rasterized_glyph = *char_desc->rasterized_glyph;

        if (!&rasterized_glyph)
          continue;

        const scene_graph::CharDesc& sg_char_desc = *char_desc->desc;

        sprite->position   = sg_char_desc.position;
        sprite->size       = sg_char_desc.size;
        sprite->color      = sg_char_desc.color;
        sprite->tex_offset = rasterized_glyph.tex_offset;
        sprite->tex_size   = rasterized_glyph.tex_size;
        sprite->normal     = math::vec3f (0, 0, 1.0f);
        sprite->rotation   = math::anglef ();

        sprite++;
      }

        //обновление размеров списка

      size_t count = sprite - sprites.data ();

      if (count > text_block.reserved_sprites_count)
        text_block.reserved_sprites_count = count;

      if (text_block.reserved_sprites_count < DEFAULT_RESERVED_SPRITES_COUNT)
        text_block.reserved_sprites_count = DEFAULT_RESERVED_SPRITES_COUNT;

      interchange::object_id_t id = Id ();

      context.SetSpriteListBuffer (id, uint32 (subid), count, text_block.reserved_sprites_count);

        //обновление списка спрайтов

      if (count)
        context.SetSpriteListDescs (id, uint32 (subid), 0, interchange::RawArray<interchange::SpriteDesc> (sprites.data (), count));

      text_block.used = true;
    }

  private:
    FontManager&              font_manager;
    xtl::auto_connection      on_update_fonts_connection;
    xtl::auto_connection      on_update_descs_connection;
    bool                      need_update_fonts;
    bool                      need_update_descs;
    size_t                    material_name_hash;
    size_t                    texture_semantic_hash;
    FontTextBlockArray        font_text_blocks;
    FontRenderingTempCachePtr cache;
    SpriteBuffer&             sprites;
    CharArray&                chars;
};

}

namespace render {
namespace scene {
namespace client {

Node* create_node (scene_graph::TextModel& model, SceneManager& scene_manager)
{
  return new TextModel (model, scene_manager);
}

}}}
