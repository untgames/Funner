#include "shared.h"

using namespace render::manager;

#ifdef _MSC_VER
  #pragma warning (disable : 4250) //inheritance via dominance
#endif

//TODO: check IB size (64k)

namespace
{

/*
    Константы
*/

const bool ENTITY_INDEPENDENT = false;
const bool ENTITY_DEPENDENT   = true;

/*
    Утилиты
*/

render::low_level::UsageMode get_mode (MeshBufferUsage usage)
{
  switch (usage)
  {
    case MeshBufferUsage_Static:  return render::low_level::UsageMode_Static;
    case MeshBufferUsage_Dynamic: return render::low_level::UsageMode_Dynamic;
    case MeshBufferUsage_Stream:  return render::low_level::UsageMode_Stream;
    default:                      throw xtl::make_argument_exception ("", "usage", usage);
  }
}

/*
    Генератор линий
*/

struct LineGenerator
{
  enum {
    VERTICES_PER_PRIMITIVE = 2,
    INDICES_PER_PRIMITIVE  = 2
  };

  static const render::low_level::PrimitiveType PRIMITIVE_TYPE = render::low_level::PrimitiveType_LineList;

  static void Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
  {
    DynamicPrimitiveVertex* dst_vertex = dst_vertices;
    const LinePoint*        src_point  = src_line.point;

    for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++, src_point++)
    {
      dst_vertex->color     = src_point->color;
      dst_vertex->tex_coord = src_point->tex_offset;
    }

    dst_indices [0] = base_vertex;
    dst_indices [1] = base_vertex + 1;
  }
};

struct StandaloneLineGenerator: public LineGenerator
{
  static void Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
  {
    DynamicPrimitiveVertex* dst_vertex = dst_vertices;
    const LinePoint*        src_point  = src_line.point;

    for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++, src_point++)
      dst_vertex->position = src_point->position;
  }
};

class BatchingLineGenerator: public LineGenerator
{
  public:
    BatchingLineGenerator (const math::mat4f& in_world_tm)
      : world_tm (in_world_tm)
    {
    }

    void Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
    {
      DynamicPrimitiveVertex* dst_vertex = dst_vertices;
      const LinePoint*        src_point  = src_line.point;

      for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++, src_point++)
        dst_vertex->position = world_tm * src_point->position;
    }

  private:
    const math::mat4f& world_tm;
};

/*
    Генератор спрайтов
*/

struct SpriteGenerator
{
  enum {
    VERTICES_PER_PRIMITIVE = 4,
    INDICES_PER_PRIMITIVE  = 6
  };

  static const render::low_level::PrimitiveType PRIMITIVE_TYPE = render::low_level::PrimitiveType_TriangleList;

  static void Generate (const Sprite& src_sprite, const math::vec3f& normal, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
  {
    DynamicPrimitiveVertex* dst_vertex = dst_vertices;

    for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++)
    {
      dst_vertex->color  = src_sprite.color;
      dst_vertex->normal = normal;
    }

    dst_vertices [0].tex_coord = src_sprite.tex_offset;
    dst_vertices [1].tex_coord = src_sprite.tex_offset + math::vec2f (src_sprite.tex_size.x, 0);
    dst_vertices [2].tex_coord = src_sprite.tex_offset + src_sprite.tex_size;
    dst_vertices [3].tex_coord = src_sprite.tex_offset + math::vec2f (0, src_sprite.tex_size.y);

    static const DynamicPrimitiveIndex indices [INDICES_PER_PRIMITIVE] = {0, 2, 3, 0, 1, 2};

    const DynamicPrimitiveIndex* src_index = indices;
    DynamicPrimitiveIndex*       dst_index = dst_indices;

    for (size_t i=0; i<INDICES_PER_PRIMITIVE; i++, dst_index++, src_index++)
      *dst_index = *src_index + base_vertex;
  }
};

class BillboardSpriteGenerator: public SpriteGenerator
{
  public:
    BillboardSpriteGenerator (const math::vec3f& local_normal, const math::vec3f& local_up, const math::mat4f& in_world_tm)
      : world_tm (in_world_tm)
      , world_normal (world_tm * math::vec4f (local_normal, 0))
      , right (cross (local_up, local_normal))
      , up (cross (local_normal, right))
    {
    }

    void Generate (const BillboardSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
    {
      math::vec3f ortx = right * src_sprite.size.x * 0.5f,
                  orty = up * src_sprite.size.y * 0.5f;

      dst_vertices [0].position = world_tm * (src_sprite.position - ortx - orty);
      dst_vertices [1].position = world_tm * (src_sprite.position + ortx - orty);
      dst_vertices [2].position = world_tm * (src_sprite.position + ortx + orty);
      dst_vertices [3].position = world_tm * (src_sprite.position - ortx + orty);

      SpriteGenerator::Generate (src_sprite, world_normal, base_vertex, dst_vertices, dst_indices);
    }

  private:
    const math::mat4f& world_tm;
    math::vec3f        world_normal;
    math::vec3f        up;
    math::vec3f        right;
};

class StandaloneOrientedSpriteGenerator: public SpriteGenerator
{
  public:
    StandaloneOrientedSpriteGenerator (const math::vec3f& local_up)
      : up (local_up)
    {
    }

    void Generate (const OrientedSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
    {
      math::vec3f ortx = cross (up, src_sprite.normal);

      if (src_sprite.rotation != math::anglef ())
        ortx = math::rotate (src_sprite.rotation, src_sprite.normal) * math::vec4f (ortx, 0);

      math::vec3f orty = cross (src_sprite.normal, ortx);

      ortx *= src_sprite.size.x * 0.5f;
      orty *= src_sprite.size.y * 0.5f;

      dst_vertices [0].position = src_sprite.position - ortx - orty;
      dst_vertices [1].position = src_sprite.position + ortx - orty;
      dst_vertices [2].position = src_sprite.position + ortx + orty;
      dst_vertices [3].position = src_sprite.position - ortx + orty;

      SpriteGenerator::Generate (src_sprite, src_sprite.normal, base_vertex, dst_vertices, dst_indices);
    }

  private:
    const math::vec3f& up;    
};

class BatchingOrientedSpriteGenerator: public SpriteGenerator
{
  public:
    BatchingOrientedSpriteGenerator (const math::vec3f& local_up, const math::mat4f& in_world_tm)
      : up (local_up)
      , world_tm (in_world_tm)
    {
    }

    void Generate (const OrientedSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
    {
      math::vec3f ortx = cross (up, src_sprite.normal);

      if (src_sprite.rotation != math::anglef ())
        ortx = math::rotate (src_sprite.rotation, src_sprite.normal) * math::vec4f (ortx, 0);

      math::vec3f orty = cross (src_sprite.normal, ortx);

      ortx *= src_sprite.size.x * 0.5f;
      orty *= src_sprite.size.y * 0.5f;

      dst_vertices [0].position = world_tm * (src_sprite.position - ortx - orty);
      dst_vertices [1].position = world_tm * (src_sprite.position + ortx - orty);
      dst_vertices [2].position = world_tm * (src_sprite.position + ortx + orty);
      dst_vertices [3].position = world_tm * (src_sprite.position - ortx + orty);

      SpriteGenerator::Generate (src_sprite, src_sprite.normal, base_vertex, dst_vertices, dst_indices);
    }

  private:
    const math::mat4f& world_tm;
    const math::vec3f& up;    
};

/// Применение генератора к блоку спрайтов
template <class Generator, class Item>
inline void generate (Generator& generator, size_t items_count, const Item* item, size_t base_vertex, DynamicPrimitiveVertex* dst_vertex, DynamicPrimitiveIndex* dst_index)
{
  while (items_count--)
  {
    generator.Generate (*item, base_vertex, dst_vertex, dst_index);

    item++;
    
    base_vertex += Generator::VERTICES_PER_PRIMITIVE;
    dst_vertex  += Generator::VERTICES_PER_PRIMITIVE;
    dst_index   += Generator::INDICES_PER_PRIMITIVE;
  }
}

/*
    Хранилище для материалов
*/

class DynamicPrimitiveListMaterialHolder: virtual public DynamicPrimitiveListImplBase
{
  public:
/// Конструктор
    DynamicPrimitiveListMaterialHolder (const MaterialManagerPtr& in_material_manager, bool is_entity_dependent)
      : DynamicPrimitiveListImplBase (is_entity_dependent)
      , material_manager (in_material_manager)
    {
      if (!material_manager)
        throw xtl::make_null_argument_exception ("render::manager::DynamicPrimitiveListMaterialHolder<T>::DynamicPrimitiveListMaterialHolder", "material_manager");
    }

    void SetMaterial (const char* material_name)
    {
      try
      {
          //проверка корректности аргументов

        if (!material_name)
          throw xtl::make_null_argument_exception ("", "material");

          //замена материала

        MaterialProxy proxy = material_manager->GetMaterialProxy (material_name);

        if (material)
        {
          material->DetachCacheHolder (*this);

          *material = proxy;
        }
        else
        {
          material.reset (new MaterialProxy (proxy));
        }    

        try
        {
          proxy.AttachCacheHolder (*this);

          InvalidateCacheDependencies ();
        }
        catch (...)
        {
          material.reset ();
          throw;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::DynamicPrimitiveListMaterialHolder::SetMaterial");
        throw;
      }
    }

    const char* Material () 
    {
      return material ? material->Name () : "";
    }

    MaterialImpl* CachedMaterial () { return cached_material.get (); }

    render::manager::DeviceManager& DeviceManager () { return material_manager->DeviceManager (); }

  protected:
    void ResetCacheCore ()
    {
      cached_material = 0;
    }

    void UpdateCacheCore ()
    {
      try
      {
        cached_material = material ? material->Resource () : MaterialPtr ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::DynamicPrimitiveListMaterialHolder::UpdateCacheCore");
        throw;
      }
    }

  private:
    MaterialManagerPtr            material_manager; //менеджер материалов
    stl::auto_ptr<MaterialProxy>  material;         //материал
    MaterialPtr                   cached_material;  //закэшированный материал
};

/*
    Standalone primitve holder
*/

class DynamicPrimitiveListStandalonePrimitiveHolder: public DynamicPrimitiveListMaterialHolder
{
  public:
/// Конструктор
    DynamicPrimitiveListStandalonePrimitiveHolder (const MaterialManagerPtr& material_manager, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage, render::low_level::PrimitiveType in_primitive_type, bool entity_dependent)
      : DynamicPrimitiveListImplBase (entity_dependent)
      , DynamicPrimitiveListMaterialHolder (material_manager, entity_dependent)
      , primitive_type (in_primitive_type)
      , vb (get_mode (vb_usage), render::low_level::BindFlag_VertexBuffer)
      , ib (get_mode (ib_usage), render::low_level::BindFlag_IndexBuffer)
      , cached_state_block_mask_hash (0)
      , need_update_primitives (true)
    {
    }

  protected:
    DynamicVertexBuffer& VertexBuffer () { return vb; }
    DynamicIndexBuffer&  IndexBuffer  () { return ib; }

/// Оповещение о необходимости обновления примитива
    void PrimitivesUpdateNotify () { need_update_primitives = true; }

/// Сброс кэша
    void ResetCacheCore ()
    {
      DynamicPrimitiveListMaterialHolder::ResetCacheCore ();

      cached_state_block           = LowLevelStateBlockPtr ();
      cached_state_block_mask_hash = 0;
    }   

/// Обновление кэша
    void UpdateCacheCore ()
    {
      try
      {
        DynamicPrimitiveListMaterialHolder::UpdateCacheCore ();

        render::manager::DeviceManager&    device_manager = DeviceManager ();
        render::low_level::IDevice&        device         = device_manager.Device ();
        render::low_level::IDeviceContext& context        = device_manager.ImmediateContext ();

        if (need_update_primitives)
        {
          memset (&cached_primitive, 0, sizeof (cached_primitive));
          
            //запрос состояния материала 

          MaterialImpl* cached_material = CachedMaterial ();       
          
          LowLevelStateBlockPtr material_state_block = cached_material ? cached_material->StateBlock () : LowLevelStateBlockPtr ();
            
          render::low_level::StateBlockMask mask;

          if (material_state_block)
          {
            material_state_block->Apply (&context);
            material_state_block->GetMask (mask);
          }
          
            //установка вершинных/индексного буфера
            
          mask.is_index_buffer = true;
          mask.is_layout       = true;

          context.ISSetVertexBuffer (0, vb.LowLevelBuffer ().get ());
            
          mask.is_vertex_buffers [0] = true;

          context.ISSetInputLayout  (device_manager.InputLayoutManager ().DynamicPrimitivesInputLayout ().get ());
          context.ISSetIndexBuffer  (ib.LowLevelBuffer ().get ());
          
            //обновление блока состояний примитива
          
          size_t mask_hash = mask.Hash ();
          
          if (cached_state_block_mask_hash != mask_hash)
          {
            cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
            cached_state_block_mask_hash = mask_hash;
          }

          cached_state_block->Capture (&context);
          
            //кэширование параметров примитива для отрисовки
          
          cached_primitive.material    = cached_material;
          cached_primitive.state_block = cached_state_block.get ();
          cached_primitive.indexed     = true;
          cached_primitive.type        = primitive_type;
          cached_primitive.first       = 0;
          cached_primitive.count       = ib.Size ();
          cached_primitive.base_vertex = 0;
          cached_primitive.tags_count  = cached_material ? cached_material->TagsCount () : 0;
          cached_primitive.tags        = cached_material ? cached_material->Tags () : (const size_t*)0;
          
            //обновление зависимостей всегда, поскольку любые изменения материала/примитива должны быть отображены на зависимые кэши
            
          InvalidateCacheDependencies ();

          need_update_primitives = false;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::DynamicPrimitiveListStandalonePrimitiveHolder::UpdateCacheCore");
        throw;
      }
    }

  private:
    render::low_level::PrimitiveType   primitive_type;
    DynamicVertexBuffer                vb;
    DynamicIndexBuffer                 ib;
    render::manager::RendererPrimitive cached_primitive;
    LowLevelStateBlockPtr              cached_state_block;
    size_t                             cached_state_block_mask_hash;
    bool                               need_update_primitives;
};

/*
    Standalone Oriented Sprites & Lines
*/

template <class T, class Generator>
class StandaloneLineAndOrientedSpriteDynamicPrimitiveList: public DynamicPrimitiveListStandalonePrimitiveHolder, public DynamicPrimitiveListImpl<T>, private Generator
{
  public:
    using DynamicPrimitiveListImpl<T>::Item;

    enum { VERTICES_PER_PRIMITIVE = Generator::VERTICES_PER_PRIMITIVE, INDICES_PER_PRIMITIVE = Generator::INDICES_PER_PRIMITIVE };

    static const render::low_level::PrimitiveType PRIMITIVE_TYPE = Generator::PRIMITIVE_TYPE;

/// Конструктор
    StandaloneLineAndOrientedSpriteDynamicPrimitiveList (const MaterialManagerPtr& material_manager, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage, const Generator& generator)
      : DynamicPrimitiveListImplBase (ENTITY_INDEPENDENT)
      , DynamicPrimitiveListStandalonePrimitiveHolder (material_manager, vb_usage, ib_usage, PRIMITIVE_TYPE, ENTITY_INDEPENDENT)
      , DynamicPrimitiveListImpl<T> (ENTITY_INDEPENDENT)
      , Generator (generator)
      , need_update_buffers (true)
    {
    }

///Создание экземпляра
    DynamicPrimitive* CreateDynamicPrimitiveInstanceCore ()
    {
      throw xtl::format_not_supported_exception ("render::manager::StandaloneLineAndOrientedSpriteDynamicPrimitiveList<T>::CreateDynamicPrimitiveInstanceCore", "Dynamic primitives are not supported for this list");
    }

///Количество примитивов
    size_t Size () { return VertexBuffer ().Size () / VERTICES_PER_PRIMITIVE; }

///Добавление / обновление примитивов
    size_t Add (size_t count, const Item* items)
    {
      try
      {
        DynamicVertexBuffer& vb = VertexBuffer ();
        DynamicIndexBuffer&  ib = IndexBuffer ();

          //проверка корректности аргументов

        if (!count)
          return vb.Size () / VERTICES_PER_PRIMITIVE;

        if (!items)
          throw xtl::make_null_argument_exception ("", "items");      

          //добавление

        size_t new_verts_count = vb.Size () + count * VERTICES_PER_PRIMITIVE,
               new_inds_count  = ib.Size () + count * INDICES_PER_PRIMITIVE,
               base_vertex     = vb.Size (),
               base_index      = ib.Size ();

        vb.Reserve (new_verts_count);
        ib.Reserve (new_inds_count);
        vb.Resize (new_verts_count);
        ib.Resize (new_inds_count);

        generate (static_cast<Generator&> (*this), count, items, base_vertex, vb.Data () + base_vertex, ib.Data () + base_index);

        need_update_buffers = true;

        PrimitivesUpdateNotify ();

        InvalidateCacheDependencies ();

        return new_verts_count / VERTICES_PER_PRIMITIVE - 1;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::StandaloneLineAndOrientedSpriteDynamicPrimitiveList<T>::Add");
        throw;
      }
    }

    void Update (size_t first, size_t count, const Item* src_items)
    {
      try
      {
        DynamicVertexBuffer& vb = VertexBuffer ();
        DynamicIndexBuffer&  ib = IndexBuffer ();

          //проверка корректности аргументов

        if (!count)
          return;

        if (!src_items)
          throw xtl::make_null_argument_exception ("", "items");

        const size_t current_items_count = vb.Size () / VERTICES_PER_PRIMITIVE;

        if (first >= current_items_count)
          throw xtl::make_range_exception ("", "first", first, current_items_count);
        
        if (current_items_count - first < count)
          throw xtl::make_range_exception ("", "count", count, current_items_count - first);

          //обновление элементов

        size_t base_vertex = first * VERTICES_PER_PRIMITIVE,
               base_index  = first * INDICES_PER_PRIMITIVE;

        generate (static_cast<Generator&> (*this), count, src_items, base_vertex, vb.Data () + base_vertex, ib.Data () + base_index);

        need_update_buffers = true;

        InvalidateCache ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::StandaloneLineAndOrientedSpriteDynamicPrimitiveList<T>::Update");
        throw;
      }
    }

///Удаление примитивов
    void Remove (size_t first, size_t count)
    {
      DynamicVertexBuffer& vb = VertexBuffer ();
      DynamicIndexBuffer&  ib = IndexBuffer ();

      const size_t current_items_count = vb.Size () / VERTICES_PER_PRIMITIVE;

      if (first >= current_items_count)
        return;

      if (current_items_count - first < count)
        count = current_items_count - first;

      if (!count)
        return;

      size_t base_vertex = first * VERTICES_PER_PRIMITIVE,
             base_index  = first * INDICES_PER_PRIMITIVE,
             verts_count = count * VERTICES_PER_PRIMITIVE,
             inds_count  = count * INDICES_PER_PRIMITIVE;

      memmove (vb.Data () + first, vb.Data () + first + verts_count, sizeof (DynamicPrimitiveVertex) * verts_count);
      memmove (ib.Data () + first, ib.Data () + first + inds_count, sizeof (DynamicPrimitiveIndex) * inds_count);

      need_update_buffers = true;

      PrimitivesUpdateNotify ();

      InvalidateCacheDependencies ();
    }

    void Clear ()
    {
      VertexBuffer ().Clear ();
      IndexBuffer ().Clear ();

      need_update_buffers = true;

      PrimitivesUpdateNotify ();

      InvalidateCacheDependencies ();
    }

///Резервируемое пространство
    size_t Capacity () { return VertexBuffer ().Capacity () / VERTICES_PER_PRIMITIVE; }

    void Reserve (size_t count)
    {
      try
      {
        DynamicVertexBuffer& vb = VertexBuffer ();
        DynamicIndexBuffer&  ib = IndexBuffer ();

        const size_t current_capacity = vb.Capacity () / VERTICES_PER_PRIMITIVE;

        vb.Reserve (count * VERTICES_PER_PRIMITIVE);
        ib.Reserve (count * INDICES_PER_PRIMITIVE);

        if (current_capacity < count)
        {
          need_update_buffers = true;

          PrimitivesUpdateNotify ();  

          InvalidateCacheDependencies ();
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::StandaloneLineAndOrientedSpriteDynamicPrimitiveList<T>::Reserve");
        throw;
      }      
    }

  private:
    void UpdateCacheCore ()
    {
      try
      {
        if (need_update_buffers)
        {
          render::low_level::IDevice& device = DeviceManager ().Device ();

          VertexBuffer ().SyncBuffers (device);
          IndexBuffer ().SyncBuffers (device);

          need_update_buffers = false;
        }

        DynamicPrimitiveListStandalonePrimitiveHolder::UpdateCacheCore ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::StandaloneLineAndOrientedSpriteDynamicPrimitiveList::UpdateCacheCore");
        throw;
      }
    }

  private:
    bool need_update_buffers;
};

/*
    Хранилище элементов списка днамических примитивов
*/

template <class T> class DynamicPrimitiveListStorage: public DynamicPrimitiveListMaterialHolder, public DynamicPrimitiveListImpl<T>
{
  public:
    using DynamicPrimitiveListImpl<T>::Item;

/// Конструктор
    DynamicPrimitiveListStorage (const MaterialManagerPtr& material_manager)
      : DynamicPrimitiveListImplBase (ENTITY_DEPENDENT)
      , DynamicPrimitiveListMaterialHolder (material_manager, ENTITY_DEPENDENT)
      , DynamicPrimitiveListImpl<T> (ENTITY_DEPENDENT)
      , need_update_buffers (true)
    {
    }

/// Количество примитивов
    size_t Size () { return items.size (); }

/// Получение примитивов
    Item* Items () { return items.empty () ? (Item*)0 : &items [0]; }

/// Добавление / обновление примитивов
    size_t Add (size_t count, const Item* items)
    {
      try
      {
          //проверка корректности аргументов

        if (!count)
          return items.size ();

        if (!items)
          throw xtl::make_null_argument_exception ("", "items");

          //добавление

        items.insert (impl->items.end (), items, items + count);

        need_update_buffers = true;

        InvalidateCacheDependencies ();

        return items.size () - count;    
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::DynamicPrimitiveListStorage<T>::Add");
        throw;
      }
    }

    void Update (size_t first, size_t count, const Item* src_items)
    {
      try
      {
          //проверка корректности аргументов

        if (!count)
          return;

        if (!src_items)
          throw xtl::make_null_argument_exception ("", "items");

        if (first >= items.size ())
          throw xtl::make_range_exception ("", "first", first, items.size ());
        
        if (items.size () - first < count)
          throw xtl::make_range_exception ("", "count", count, items.size () - first);

          //обновление спрайтов

        stl::copy (src_items, src_items + count, &items [first]);

        InvalidateCache ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::DynamicPrimitiveListStorage<T>::Update");
        throw;
      }
    }

/// Удаление примитивов
    void Remove (size_t first, size_t count) 
    {
      if (first >= items.size ())
        return;

      if (items.size () - first < count)
        count = items.size () - first;

      if (!count)
        return;

      items.erase (items.begin () + first, items.begin () + first + count);

      need_update_buffers = true;

      InvalidateCacheDependencies ();
    }

    void Clear ()
    {
      items.clear ();

      need_update_buffers = true;

      InvalidateCacheDependencies ();
    }

/// Резервируемое пространство
    void Reserve (size_t count)
    {
      size_t capacity = items.capacity ();

      items.reserve (count);

      if (capacity < count)
      {
        need_update_buffers = true;

        InvalidateCacheDependencies ();
      }
    }

    size_t Capacity () { return items.capacity (); }

  protected:
    void UpdateCacheCore ()
    {
      try
      {
        DynamicPrimitiveListMaterialHolder::UpdateCacheCore ();

        if (need_update_buffers)
        {
          UpdateBuffersCore ();

          need_update_buffers = false;
        }
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::DynamicPrimitiveListStorage<T>::UpdateCacheCore");
        throw;
      }
    }

  private:
    virtual void UpdateBuffersCore () = 0;

  private:
    typedef stl::vector<Item> ItemArray;

  private:
    ItemArray items;
    bool      need_update_buffers;
};

}

/*
    Создание групп динамических примитивов
*/

namespace render
{

namespace manager
{

LineListImpl* create_standalone_line_list (const MaterialManagerPtr& material_manager, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  return new StandaloneLineAndOrientedSpriteDynamicPrimitiveList<Line, StandaloneLineGenerator> (material_manager, vb_usage, ib_usage, StandaloneLineGenerator ());
}

OrientedSpriteListImpl* create_standalone_oriented_sprite_list (const MaterialManagerPtr& material_manager, const math::vec3f& up, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  return new StandaloneLineAndOrientedSpriteDynamicPrimitiveList<OrientedSprite, StandaloneOrientedSpriteGenerator> (material_manager, vb_usage, ib_usage, StandaloneOrientedSpriteGenerator (up));
}

}

}
