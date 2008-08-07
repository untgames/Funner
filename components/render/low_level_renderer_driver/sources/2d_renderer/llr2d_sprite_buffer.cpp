#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;
using namespace render::low_level;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_VERTEX_BUFFER_SIZE = 8192; //размер вершинного буффера по умолчанию (количество вершин)
const size_t SPRITE_VERTICES_COUNT      = 6;    //количество вершин в одном спрайте

/*
    Временный буфер хранения вершинных данных
*/

typedef xtl::uninitialized_storage<RenderedSpriteVertex> VertexArrayCache;

class VertexArrayCacheHolder
{
  public:
      //получение кэша
    static VertexArrayCache& GetCache ()
    {
      if (!instance)
        throw xtl::format_operation_exception ("render::mid_level::low_level_driver::renderer2d::VertexArrayCache::GetCache",
          "Null instance (lock cache before use)");

      return instance->cache;
    }

      //захват кэша
    static void Lock ()
    {
      if (!instance)
      {
        instance = new VertexArrayCacheHolder;
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
    VertexArrayCacheHolder () : ref_count (1) {}

    ~VertexArrayCacheHolder ()
    {
      instance = 0;
    }
  
  private:
    size_t                         ref_count;
    VertexArrayCache               cache;
    static VertexArrayCacheHolder* instance;
};

VertexArrayCacheHolder* VertexArrayCacheHolder::instance = 0;

}

/*
    Конструктор
*/

SpriteBuffer::SpriteBuffer ()
{
  VertexArrayCacheHolder::Lock ();

  vertex_buffer_vertices_count = 0;
}

SpriteBuffer::~SpriteBuffer ()
{
  VertexArrayCacheHolder::Unlock ();
}

/*
    Резервирование места для размещения указанного числа спрайтов
*/

void SpriteBuffer::Reserve (size_t sprites_count)
{
  data_buffer.reserve (sprites_count);
}

/*
    Добавление спрайтов примитива
*/

void SpriteBuffer::Add (size_t sprites_count, const SpriteVertexData* sprites)
{
  const SpriteVertexData* src = sprites;   

  size_t current_buffer_size = data_buffer.size ();  

  data_buffer.resize (data_buffer.size () + sprites_count);  

  const SpriteVertexData** dst = data_buffer.data () + current_buffer_size;

  while (sprites_count--)
    *dst++ = &*src++;    
}

/*
    Очистка буфера
*/

void SpriteBuffer::Clear ()
{
  data_buffer.resize (0);
}

/*
    Создание вершинного буфера
*/

void SpriteBuffer::SetVertexBufferSize (IDevice& device, size_t new_vertices_count)
{
  BufferDesc buffer_desc;

  memset (&buffer_desc, 0, sizeof buffer_desc);

  buffer_desc.usage_mode   = UsageMode_Stream;
  buffer_desc.bind_flags   = BindFlag_VertexBuffer;
  buffer_desc.access_flags = AccessFlag_ReadWrite;
  buffer_desc.size         = sizeof (RenderedSpriteVertex) * new_vertices_count;

  vertex_buffer                = BufferPtr (device.CreateBuffer (buffer_desc), false);
  vertex_buffer_vertices_count = new_vertices_count;
}

/*
    Обновление вершинного буфера
*/

void SpriteBuffer::UpdateVertexBuffer (IDevice& device)
{
  if (!data_buffer.size ())
    return;

    //если размер вершинного буфера меньше необходимого - пересоздаём его
       
  size_t vertices_count = data_buffer.size () * SPRITE_VERTICES_COUNT;  

  if (data_buffer.size () > vertex_buffer_vertices_count)
    SetVertexBufferSize (device, (data_buffer.size () + DEFAULT_VERTEX_BUFFER_SIZE) * SPRITE_VERTICES_COUNT);

    //формирование буфера вершин    

  VertexArrayCache& cache = VertexArrayCacheHolder::GetCache ();

  cache.resize (data_buffer.size () * SPRITE_VERTICES_COUNT, false);

  RenderedSpriteVertex*    new_vertex = cache.data ();
  const SpriteVertexData** src_vertex = data_buffer.data ();

  for (size_t i=0, count=data_buffer.size (); i<count; i++, src_vertex++)
  {
    const SpriteVertexData& sprite_vertex_data = **src_vertex;

    for (size_t j=0; j<SPRITE_VERTICES_COUNT; j++, new_vertex++)
    {
      static size_t order [SPRITE_VERTICES_COUNT] = {0, 1, 2, 3, 2, 1};
      
      new_vertex->color    = sprite_vertex_data.color;
      new_vertex->position = sprite_vertex_data.vertices [order [j]].position;
      new_vertex->texcoord = sprite_vertex_data.vertices [order [j]].texcoord;
    }
  }  

    //обновление вершинного буфера    

  vertex_buffer->SetData (0, cache.size () * sizeof (RenderedSpriteVertex), cache.data ());
}
