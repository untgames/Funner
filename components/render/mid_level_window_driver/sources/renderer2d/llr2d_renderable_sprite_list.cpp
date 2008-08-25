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

/*
    Временный буфер хранения вершинных данных
*/

typedef xtl::uninitialized_storage<RenderableVertex> VertexArrayCache;

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

RenderableSpriteList::RenderableSpriteList ()
{
  VertexArrayCacheHolder::Lock ();

  vertex_buffer_vertices_count = 0;
}

RenderableSpriteList::~RenderableSpriteList ()
{
  VertexArrayCacheHolder::Unlock ();
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
    Создание вершинного буфера
*/

void RenderableSpriteList::SetVertexBufferSize (IDevice& device, size_t new_vertices_count)
{
  BufferDesc buffer_desc;

  memset (&buffer_desc, 0, sizeof buffer_desc);

  buffer_desc.usage_mode   = UsageMode_Stream;
  buffer_desc.bind_flags   = BindFlag_VertexBuffer;
  buffer_desc.access_flags = AccessFlag_ReadWrite;
  buffer_desc.size         = sizeof (RenderableVertex) * new_vertices_count;

  vertex_buffer                = BufferPtr (device.CreateBuffer (buffer_desc), false);
  vertex_buffer_vertices_count = new_vertices_count;
}

/*
    Обновление вершинного буфера
*/

void RenderableSpriteList::UpdateVertexBuffer (IDevice& device)
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

  RenderableVertex*        dst_vertex = cache.data ();
  const RenderableSprite** src_sprite = data_buffer.data ();

  for (size_t count=data_buffer.size (); count--; src_sprite++, dst_vertex += SPRITE_VERTICES_COUNT)
    memcpy (dst_vertex, (*src_sprite)->vertices, sizeof (RenderableVertex) * SPRITE_VERTICES_COUNT);

    //обновление вершинного буфера

  vertex_buffer->SetData (0, cache.size () * sizeof (RenderableVertex), cache.data ());
}
