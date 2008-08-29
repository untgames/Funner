#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::window_driver;
using namespace render::mid_level::window_driver::renderer2d;
using namespace render::low_level;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_SPRITES_RESERVE_SIZE = 512; //резервируемое число спрайтов в буферах

/*
    Временный буфер хранения вершинных данных
*/

typedef xtl::uninitialized_storage<char> TempBuffer;

class TempBufferHolder
{
  public:
      //получение кэша
    static TempBuffer& GetCache ()
    {
      if (!instance)
        throw xtl::format_operation_exception ("render::mid_level::window_driver::renderer2d::TempBuffer::GetCache",
          "Null instance (lock cache before use)");

      return instance->cache;
    }

      //захват кэша
    static void Lock ()
    {
      if (!instance)
      {
        instance = new TempBufferHolder;
      }
      else
      {
        instance->ref_count++;
      }
    }
    
      //освобождение кэша
    static void Unlock ()
    {
      if (!instance)
        return;

      if (!--instance->ref_count)
        delete instance;
    }

  private:
    TempBufferHolder () : ref_count (1) {}

    ~TempBufferHolder ()
    {
      instance = 0;
    }
  
  private:
    size_t                   ref_count;
    TempBuffer               cache;
    static TempBufferHolder* instance;
};

TempBufferHolder* TempBufferHolder::instance = 0;

}

/*
    Конструктор
*/

RenderableSpriteList::RenderableSpriteList ()
{
  TempBufferHolder::Lock ();

  buffers_sprites_count = 0;
}

RenderableSpriteList::~RenderableSpriteList ()
{
  TempBufferHolder::Unlock ();
}

/*
    Резервирование места для размещения указанного числа спрайтов
*/

void RenderableSpriteList::Reserve (size_t sprites_count)
{
  data_buffer.reserve (sprites_count);
}

/*
    Добавление спрайтов примитива
*/

void RenderableSpriteList::Add (size_t sprites_count, const RenderableSprite* sprites)
{
  const RenderableSprite* src = sprites;   

  size_t current_buffer_size = data_buffer.size ();  

  data_buffer.resize (data_buffer.size () + sprites_count);  

  const RenderableSprite** dst = data_buffer.data () + current_buffer_size;

  while (sprites_count--)
    *dst++ = &*src++;    
}

/*
    Очистка буфера
*/

void RenderableSpriteList::Clear ()
{
  data_buffer.resize (0);
}

/*
    Изменение размеров буферов
*/

void RenderableSpriteList::ResizeBuffers (IDevice& device, size_t new_sprites_count)
{
    //создание вершинного буфера

  BufferDesc buffer_desc;

  memset (&buffer_desc, 0, sizeof buffer_desc);

  buffer_desc.usage_mode   = UsageMode_Stream;
  buffer_desc.bind_flags   = BindFlag_VertexBuffer;
  buffer_desc.access_flags = AccessFlag_Write;
  buffer_desc.size         = sizeof (RenderableVertex) * new_sprites_count * SPRITE_VERTICES_COUNT;  

  BufferPtr new_vertex_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);
  
    //создание индексного буфера
    
  memset (&buffer_desc, 0, sizeof buffer_desc);

  buffer_desc.usage_mode   = UsageMode_Stream;
  buffer_desc.bind_flags   = BindFlag_IndexBuffer;
  buffer_desc.access_flags = AccessFlag_Write;
  buffer_desc.size         = sizeof (size_t) * new_sprites_count * SPRITE_INDICES_COUNT;  

  BufferPtr new_index_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

    //обновление параметров

  vertex_buffer         = new_vertex_buffer;
  index_buffer          = new_index_buffer;
  buffers_sprites_count = new_sprites_count;
}

/*
    Обновление вершинного буфера
*/

void RenderableSpriteList::UpdateVertexBuffer (IDevice& device)
{
  if (!data_buffer.size ())
    return;

    //подготовка вершинного и индексного буферов       

  if (data_buffer.size () > buffers_sprites_count)
    ResizeBuffers (device, data_buffer.size () + DEFAULT_SPRITES_RESERVE_SIZE);

    //формирование буфера вершин    

  TempBuffer& cache = TempBufferHolder::GetCache ();

  cache.resize (data_buffer.size () * SPRITE_VERTICES_COUNT * sizeof (RenderableVertex), false);

  RenderableVertex*        dst_vertex = reinterpret_cast<RenderableVertex*> (cache.data ());
  const RenderableSprite** src_sprite = data_buffer.data ();

  for (size_t count=data_buffer.size (); count--; src_sprite++, dst_vertex += SPRITE_VERTICES_COUNT)
    memcpy (dst_vertex, (*src_sprite)->vertices, sizeof (RenderableVertex) * SPRITE_VERTICES_COUNT);

    //обновление вершинного буфера    

  vertex_buffer->SetData (0, cache.size (), cache.data ());
  
    //формирование буфера индексов
    
  cache.resize (data_buffer.size () * SPRITE_INDICES_COUNT * sizeof (size_t), false);

  size_t *dst_index = reinterpret_cast<size_t*> (cache.data ()), src_index = 0;

  static const size_t index_offsets [] = {0, 1, 2, 3, 0, 2};

  for (size_t count=data_buffer.size (); count--; src_index += SPRITE_VERTICES_COUNT)
  {
    const size_t* offset = index_offsets;
    
    for (size_t count=SPRITE_INDICES_COUNT; count--; dst_index++, offset++)
      *dst_index = *offset + src_index;
  }

    //обновление индексного буфера

  index_buffer->SetData (0, cache.size (), cache.data ());    
}
