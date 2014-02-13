#include "shared.h"

using namespace render::manager;

#ifdef _MSC_VER
  #pragma warning (disable : 4250) //inheritance via dominance
#endif

//TODO: check IB size (64k)

namespace
{

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
===================================================================================================
    Generators
===================================================================================================
*/

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

  static void Generate (const Line& src_line, DynamicPrimitiveVertex* dst_vertices)
  {
    DynamicPrimitiveVertex* dst_vertex = dst_vertices;
    const LinePoint*        src_point  = src_line.point;

    for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++, src_point++)
    {
      dst_vertex->position  = src_point->position;
      dst_vertex->color     = src_point->color;
      dst_vertex->tex_coord = src_point->tex_offset;
      dst_vertex->normal    = math::vec3f (0, 0, 1.0f);
    }
  }

  static void Generate (size_t base_vertex, DynamicPrimitiveIndex* dst_indices)
  {
    dst_indices [0] = base_vertex;
    dst_indices [1] = base_vertex + 1;
  }
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

  static void Generate (const Sprite& src_sprite, const math::vec3f& normal, DynamicPrimitiveVertex* dst_vertices)
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
  }

  static void Generate (size_t base_vertex, DynamicPrimitiveIndex* dst_indices)
  {
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
    BillboardSpriteGenerator (EntityImpl& entity, const math::vec3f& view_up, const math::mat4f& inv_view_proj_tm)
      : world_tm (entity.WorldMatrix ())
      , inv_mvp_tm (inv_view_proj_tm * entity.InverseWorldMatrix ())
      , local_normal (inv_mvp_tm * math::vec4f (0, 0, 1.0f, 0))
      , world_normal (world_tm * local_normal)
      , right (cross (math::vec3f (inv_mvp_tm * math::vec4f (view_up, 0)), local_normal))
      , up (cross (local_normal, right))
    {
    }

    using SpriteGenerator::Generate;

    void Generate (const Sprite& src_sprite, DynamicPrimitiveVertex* dst_vertices)
    {
      math::vec3f ortx = right * src_sprite.size.x * 0.5f,
                  orty = up * src_sprite.size.y * 0.5f;

      dst_vertices [0].position = world_tm * (src_sprite.position - ortx - orty);
      dst_vertices [1].position = world_tm * (src_sprite.position + ortx - orty);
      dst_vertices [2].position = world_tm * (src_sprite.position + ortx + orty);
      dst_vertices [3].position = world_tm * (src_sprite.position - ortx + orty);

      SpriteGenerator::Generate (src_sprite, world_normal, dst_vertices);
    }

  private:
    const math::mat4f& world_tm;
    math::mat4f        inv_mvp_tm;
    math::vec3f        local_normal;
    math::vec3f        world_normal;
    math::vec3f        right;
    math::vec3f        up;
};

class OrientedSpriteGenerator: public SpriteGenerator
{
  public:
    OrientedSpriteGenerator (const math::vec3f& local_up)
      : up (local_up)
    {
    }

    using SpriteGenerator::Generate;

    void Generate (const Sprite& src_sprite, DynamicPrimitiveVertex* dst_vertices)
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

      SpriteGenerator::Generate (src_sprite, src_sprite.normal, dst_vertices);
    }

  private:
    const math::vec3f& up;    
};

class OrientedBillboardSpriteGenerator: public SpriteGenerator
{
  public:
    OrientedBillboardSpriteGenerator (EntityImpl& entity, const math::vec3f& in_view_up, const math::mat4f& inv_view_proj_tm)
      : world_tm (entity.WorldMatrix ())
      , src_view_up (in_view_up)
      , inv_mvp_tm (inv_view_proj_tm * entity.InverseWorldMatrix ())
    {
    }

    using SpriteGenerator::Generate;

    void Generate (const Sprite& src_sprite, DynamicPrimitiveVertex* dst_vertices)
    {
      math::vec3f local_normal = inv_mvp_tm * math::vec4f (src_sprite.normal, 0), world_normal = world_tm * local_normal;
      math::vec4f view_up (src_view_up, 0);

      if (src_sprite.rotation != math::anglef ())
        view_up = math::rotate (src_sprite.rotation, src_sprite.normal) * view_up;

      math::vec3f local_up = inv_mvp_tm * view_up,
                  right    = cross (local_up, local_normal),
                  up       = cross (local_normal, right);

      math::vec3f ortx = right * src_sprite.size.x * 0.5f,
                  orty = up * src_sprite.size.y * 0.5f;

      dst_vertices [0].position = world_tm * (src_sprite.position - ortx - orty);
      dst_vertices [1].position = world_tm * (src_sprite.position + ortx - orty);
      dst_vertices [2].position = world_tm * (src_sprite.position + ortx + orty);
      dst_vertices [3].position = world_tm * (src_sprite.position - ortx + orty);

      SpriteGenerator::Generate (src_sprite, world_normal, dst_vertices);
    }

  private:
    const math::mat4f& world_tm;
    const math::vec3f& src_view_up;
    math::mat4f        inv_mvp_tm;
};

/// Применение генератора к блоку спрайтов
template <class Generator, class Item>
inline void generate (Generator& generator, size_t items_count, const Item* item, size_t base_vertex, DynamicPrimitiveVertex* dst_vertex, DynamicPrimitiveIndex* dst_index)
{
  while (items_count--)
  {
    generator.Generate (*item, dst_vertex);
    generator.Generate (base_vertex, dst_index);

    item++;
    
    base_vertex += Generator::VERTICES_PER_PRIMITIVE;
    dst_vertex  += Generator::VERTICES_PER_PRIMITIVE;
    dst_index   += Generator::INDICES_PER_PRIMITIVE;
  }
}

template <class Generator, class Item>
inline void generate (Generator& generator, size_t items_count, const Item* item, DynamicPrimitiveVertex* dst_vertex)
{
  while (items_count--)
  {
    generator.Generate (*item, dst_vertex);

    item++;
    
    dst_vertex += Generator::VERTICES_PER_PRIMITIVE;
  }
}

template <class Generator>
inline void generate (Generator& generator, size_t items_count, size_t base_vertex, DynamicPrimitiveIndex* dst_index)
{
  while (items_count--)
  {
    generator.Generate (base_vertex, dst_index);
   
    base_vertex += Generator::VERTICES_PER_PRIMITIVE;
    dst_index   += Generator::INDICES_PER_PRIMITIVE;
  }
}

template <class Generator>
inline void generate (size_t items_count, size_t base_vertex, DynamicPrimitiveIndex* dst_index)
{
  while (items_count--)
  {
    Generator::Generate (base_vertex, dst_index);
   
    base_vertex += Generator::VERTICES_PER_PRIMITIVE;
    dst_index   += Generator::INDICES_PER_PRIMITIVE;
  }
}

/*
===================================================================================================
    Common implementation classes
===================================================================================================
*/

/*
    Хранилище для материалов
*/

class MaterialHolder: virtual public SimplePrimitiveListImplBase
{
  public:
/// Конструктор
    MaterialHolder (const MaterialManagerPtr& in_material_manager)
      : material_manager (in_material_manager)
    {
      if (!material_manager)
        throw xtl::make_null_argument_exception ("render::manager::MaterialHolder<T>::MaterialHolder", "material_manager");
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
        e.touch ("render::manager::MaterialHolder::SetMaterial");
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
        e.touch ("render::manager::MaterialHolder::UpdateCacheCore");
        throw;
      }
    }

  private:
    MaterialManagerPtr            material_manager; //менеджер материалов
    stl::auto_ptr<MaterialProxy>  material;         //материал
    MaterialPtr                   cached_material;  //закэшированный материал
};

/*
    Standalone primitive holder
*/

class StandalonePrimitiveHolder: public MaterialHolder
{
  public:
/// Конструктор
    StandalonePrimitiveHolder (const MaterialManagerPtr& material_manager, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage, render::low_level::PrimitiveType in_primitive_type)
      : MaterialHolder (material_manager)
      , primitive_type (in_primitive_type)
      , vb (get_mode (vb_usage), render::low_level::BindFlag_VertexBuffer)
      , ib (get_mode (ib_usage), render::low_level::BindFlag_IndexBuffer)
      , cached_state_block_mask_hash (0)
      , need_update_primitives (true)
    {
    }

/// Оповещение о необходимости обновления примитива
    void PrimitivesUpdateNotify () { need_update_primitives = true; }

  protected:
    DynamicVertexBuffer& VertexBuffer () { return vb; }
    DynamicIndexBuffer&  IndexBuffer  () { return ib; }

    render::manager::RendererPrimitive& Primitive () { return cached_primitive; }

/// Сброс кэша
    void ResetCacheCore ()
    {
      MaterialHolder::ResetCacheCore ();

      cached_state_block           = LowLevelStateBlockPtr ();
      cached_state_block_mask_hash = 0;
    }   

/// Обновление кэша
    void UpdateCacheCore ()
    {
      try
      {
        MaterialHolder::UpdateCacheCore ();

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
        e.touch ("render::manager::StandalonePrimitiveHolder::UpdateCacheCore");
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
    Batching state block holder
*/

class BatchingStateBlockHolder: public MaterialHolder
{
  public:
    using MaterialHolder::UpdateCache;

/// Конструктор
    BatchingStateBlockHolder (const MaterialManagerPtr& material_manager, const BatchingManagerPtr& in_batching_manager)
      : MaterialHolder (material_manager)
      , batching_manager (in_batching_manager)
    {
      AttachCacheSource (*batching_manager);
    }

/// Менеджер пакетирования
    render::manager::BatchingManager& BatchingManager () { return *batching_manager; }

/// Блок состояния
    LowLevelStateBlockPtr StateBlock () { return cached_low_level_state_block; }

  protected:
/// Сброс кэша
    void ResetCacheCore ()
    {
      MaterialHolder::ResetCacheCore ();

      cached_state_block = BatchStateBlockPtr ();      
    }   

/// Обновление кэша
    void UpdateCacheCore ()
    {
      try
      {
        BatchStateBlockPtr    prev_state_block           = cached_state_block;
        LowLevelStateBlockPtr prev_low_level_state_block = cached_low_level_state_block;
        MaterialImpl*         prev_material              = CachedMaterial ();

        cached_state_block = BatchStateBlockPtr ();

        MaterialHolder::UpdateCacheCore ();

        MaterialImpl* cached_material = CachedMaterial ();

        cached_state_block = cached_material ? batching_manager->GetStateBlock (cached_material) : BatchStateBlockPtr ();

        if (cached_state_block)
          cached_state_block->UpdateCache ();

        if (prev_state_block != cached_state_block)
        {
          if (prev_state_block)   DetachCacheSource (*prev_state_block);
          if (cached_state_block) AttachCacheSource (*cached_state_block);
        }

        cached_low_level_state_block = cached_state_block ? cached_state_block->StateBlock () : LowLevelStateBlockPtr ();

        if (cached_material != prev_material || prev_low_level_state_block != cached_low_level_state_block)
          InvalidateCacheDependencies ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingStateBlockHolder::UpdateCacheCore");
        throw;
      }
    }

  private:
    BatchingManagerPtr    batching_manager;
    BatchStateBlockPtr    cached_state_block;
    LowLevelStateBlockPtr cached_low_level_state_block;
};

/*
    Хранилище элементов списка днамических примитивов
*/

template <class T, class Base> class PrimitiveListStorage: public Base, public SimplePrimitiveListImpl<T>
{
  public:
    typedef typename SimplePrimitiveListImpl<T>::Item Item;

    using Base::InvalidateCacheDependencies;
    using Base::InvalidateCache;

/// Конструктор
    PrimitiveListStorage (const MaterialManagerPtr& material_manager)
      : Base (material_manager)
      , need_update_buffers (true)
    {
    }

    template <class T1>
    PrimitiveListStorage (const MaterialManagerPtr& material_manager, const T1& arg1)
      : Base (material_manager, arg1)
      , need_update_buffers (true)
    {
    }

    template <class T1, class T2>
    PrimitiveListStorage (const MaterialManagerPtr& material_manager, const T1& arg1, const T2& arg2)
      : Base (material_manager, arg1, arg2)
      , need_update_buffers (true)
    {
    }

    template <class T1, class T2, class T3>
    PrimitiveListStorage (const MaterialManagerPtr& material_manager, const T1& arg1, const T2& arg2, const T3& arg3)
      : Base (material_manager, arg1, arg2, arg3)
      , need_update_buffers (true)
    {
    } 

/// Количество примитивов
    size_t Size () { return items.size (); }

/// Получение примитивов
    Item* Items () { return items.empty () ? (Item*)0 : &items [0]; }

/// Добавление / обновление примитивов
    size_t Add (size_t count, const Item* src_items)
    {
      try
      {
          //проверка корректности аргументов

        if (!count)
          return items.size ();

        if (!src_items)
          throw xtl::make_null_argument_exception ("", "items");

          //добавление

        items.insert (items.end (), src_items, src_items + count);

        need_update_buffers = true;

        InvalidateCacheDependencies ();

        return items.size () - count;    
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::PrimitiveListStorage<T>::Add");
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
        e.touch ("render::manager::PrimitiveListStorage<T>::Update");
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
        if (need_update_buffers)
        {
          UpdateBuffersCore ();

          need_update_buffers = false;
        }

        Base::UpdateCacheCore ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::PrimitiveListStorage<T>::UpdateCacheCore");
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

/*
    Экземпляр пакетного списка примитивов
*/

class BatchingInstance: public DynamicPrimitive, private render::manager::RendererPrimitiveGroup
{
  public:
    typedef xtl::intrusive_ptr<BatchingStateBlockHolder> PrototypePtr;

/// Конструктор
    BatchingInstance (const PrototypePtr& in_prototype, render::low_level::PrimitiveType primitive_type, size_t flags = 0)
      : DynamicPrimitive (*this, flags | DynamicPrimitiveFlag_EntityDependent)
      , prototype (in_prototype)
      , batching_manager (&prototype->BatchingManager ()) 
    {
      try
      {
        AttachCacheSource (*prototype);

        primitives_count = 1;
        primitives       = &cached_primitive;

        memset (&cached_primitive, 0, sizeof (cached_primitive));

        cached_primitive.type = primitive_type;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingInstance::BatchingInstance");
        throw;
      }
    }

/// Менеджер пакетирования
    render::manager::BatchingManager& BatchingManager () { return *batching_manager; }

  protected:
/// Прототип
    BatchingStateBlockHolder& Prototype () { return *prototype; }

/// Примитив
    render::manager::RendererPrimitive& Primitive ()
    {
      return cached_primitive; 
    }

///Заполнение примитива
    void UpdatePrimitive (size_t verts_count, size_t inds_count, DynamicPrimitiveVertex** out_vertices, DynamicPrimitiveIndex** out_indices, size_t* out_base_vertex)
    {
      try
      {
          //выделение вершин

        if (out_vertices) *out_vertices = batching_manager->AllocateDynamicVertices (verts_count, out_base_vertex);
        else              batching_manager->PreallocateDynamicVertices (verts_count);

            //выделение индексов

        const DynamicPrimitiveIndex * const* indices_base = batching_manager->TempIndexBuffer ();

        *out_indices = batching_manager->AllocateDynamicIndices (IndexPoolType_Temporary, inds_count);

            //формирование примитива

        cached_primitive.first = *out_indices - *indices_base;
        cached_primitive.count = inds_count;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingInstance::UpdatePrimitive");
        throw;
      }
    }

  private:
/// Обновление кэша
    void UpdateCacheCore ()
    {
      try
      {
        prototype->UpdateCache ();

        const DynamicPrimitiveIndex * const* indices_base = batching_manager->TempIndexBuffer ();

        cached_primitive.material         = prototype->CachedMaterial ();
        cached_primitive.tags_count       = cached_primitive.material ? cached_primitive.material->TagsCount () : 0;
        cached_primitive.tags             = cached_primitive.material ? cached_primitive.material->Tags () : (const size_t*)0;
        cached_primitive.state_block      = &*prototype->StateBlock ();
        cached_primitive.indexed          = true;
        cached_primitive.dynamic_indices  = indices_base;
        cached_primitive.batching_manager = &*batching_manager;
        cached_primitive.batching_hash    = get_batching_hash (cached_primitive);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingInstance::UpdateCacheCore");
        throw;
      }
    }

  private:
    PrototypePtr                       prototype;
    BatchingManagerPtr                 batching_manager;
    render::manager::RendererPrimitive cached_primitive;
};

/*
===================================================================================================
    Concrete classes
===================================================================================================
*/

/*
    Standalone Oriented Sprites & Lines
*/

template <class T, class Generator>
class StandaloneLineAndOrientedSpriteList: public StandalonePrimitiveHolder, public SimplePrimitiveListImpl<T>, private Generator
{
  public:
    typedef typename SimplePrimitiveListImpl<T>::Item Item;

    enum { VERTICES_PER_PRIMITIVE = Generator::VERTICES_PER_PRIMITIVE, INDICES_PER_PRIMITIVE = Generator::INDICES_PER_PRIMITIVE };

    static const render::low_level::PrimitiveType PRIMITIVE_TYPE = Generator::PRIMITIVE_TYPE;

/// Конструктор
    StandaloneLineAndOrientedSpriteList (const MaterialManagerPtr& material_manager, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage, const Generator& generator)
      : StandalonePrimitiveHolder (material_manager, vb_usage, ib_usage, PRIMITIVE_TYPE)
      , Generator (generator)
      , need_update_buffers (true)
    {
    }

///Статический примитив рендеринга
    render::manager::RendererPrimitive* StandaloneRendererPrimitive () { return &StandalonePrimitiveHolder::Primitive (); }

///Создание экземпляра
    DynamicPrimitive* CreateDynamicPrimitiveInstanceCore ()
    {
      throw xtl::format_not_supported_exception ("render::manager::StandaloneLineAndOrientedSpriteList<T>::CreateDynamicPrimitiveInstanceCore", "Dynamic primitives are not supported for this list");
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

        return base_vertex / VERTICES_PER_PRIMITIVE;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::StandaloneLineAndOrientedSpriteList<T>::Add");
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
        e.touch ("render::manager::StandaloneLineAndOrientedSpriteList<T>::Update");
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

      memmove (vb.Data () + base_vertex, vb.Data () + base_vertex + verts_count, sizeof (DynamicPrimitiveVertex) * verts_count);
      memmove (ib.Data () + base_index, ib.Data () + base_index + inds_count, sizeof (DynamicPrimitiveIndex) * inds_count);

      vb.Resize (vb.Size () - verts_count);
      ib.Resize (ib.Size () - inds_count);

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
        e.touch ("render::manager::StandaloneLineAndOrientedSpriteList<T>::Reserve");
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

        StandalonePrimitiveHolder::UpdateCacheCore ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::StandaloneLineAndOrientedSpriteList::UpdateCacheCore");
        throw;
      }
    }

  private:
    bool need_update_buffers;
};

/*
    Batching Oriented Sprites & Lines
*/

template <class T, class Generator>
class BatchingLineAndOrientedSpriteList: public BatchingStateBlockHolder, public SimplePrimitiveListImpl<T>, private Generator
{
  public:
    typedef typename SimplePrimitiveListImpl<T>::Item Item;

    enum { VERTICES_PER_PRIMITIVE = Generator::VERTICES_PER_PRIMITIVE, INDICES_PER_PRIMITIVE = Generator::INDICES_PER_PRIMITIVE };

    static const render::low_level::PrimitiveType PRIMITIVE_TYPE = Generator::PRIMITIVE_TYPE;

/// Экземпляр списка примитивов
    class Instance: public BatchingInstance
    {
      public:
/// Конструктор
        Instance (const PrototypePtr& prototype) : BatchingInstance (prototype, PRIMITIVE_TYPE) { }

      private:
///Обновление
        void UpdateOnPrerenderCore (EntityImpl& entity)
        {
          try
          {
            BatchingLineAndOrientedSpriteList& prototype = static_cast<BatchingLineAndOrientedSpriteList&> (Prototype ());

            size_t verts_count = prototype.VerticesCount (), items_count = verts_count / VERTICES_PER_PRIMITIVE, inds_count = items_count * INDICES_PER_PRIMITIVE, base_vertex = 0;

            DynamicPrimitiveVertex* vertices = 0;
            DynamicPrimitiveIndex*  indices  = 0;

            UpdatePrimitive (verts_count, inds_count, &vertices, &indices, &base_vertex);

              //формирование вершин и индексов

            generate (static_cast<Generator&> (prototype), items_count, base_vertex, indices);

            const math::mat4f& world_tm = entity.WorldMatrix ();
              
            const DynamicPrimitiveVertex* src_vert = prototype.Vertices ();
            DynamicPrimitiveVertex*       dst_vert = vertices;

              //TODO: кэшировать вершины (копировать, если не изменялись)

            for (size_t count=verts_count; count--; src_vert++, dst_vert++)
            {
              dst_vert->position = world_tm * src_vert->position;
              dst_vert->normal   = world_tm * math::vec4f (src_vert->normal, 0.0f);
            }
          }
          catch (xtl::exception& e)
          {
            e.touch ("render::manager::BatchingLineAndOrientedSpriteList::Instance::UpdateOnPrerenderCore");
            throw;
          }
        }

        void UpdateOnRenderCore (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix) {}
    };

/// Конструктор
    BatchingLineAndOrientedSpriteList (const BatchingManagerPtr& batching_manager, const MaterialManagerPtr& material_manager, const Generator& generator)
      : BatchingStateBlockHolder (material_manager, batching_manager)
      , Generator (generator)
    {
    }

///Статический примитив рендеринга
    render::manager::RendererPrimitive* StandaloneRendererPrimitive () { return 0; }

///Создание экземпляра
    DynamicPrimitive* CreateDynamicPrimitiveInstanceCore ()
    {
      try
      {
        return new Instance (this);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingLineAndOrientedSpriteList<T>::CreateDynamicPrimitiveInstanceCore");
        throw;
      }
    }

///Количество примитивов
    size_t Size () { return vertices.size () / VERTICES_PER_PRIMITIVE; }

///Добавление / обновление примитивов
    size_t Add (size_t count, const Item* items)
    {
      try
      {
          //проверка корректности аргументов

        if (!count)
          return vertices.size () / VERTICES_PER_PRIMITIVE;

        if (!items)
          throw xtl::make_null_argument_exception ("", "items");      

          //добавление

        size_t new_verts_count = vertices.size () + count * VERTICES_PER_PRIMITIVE, base_vertex = vertices.size ();

        vertices.resize (new_verts_count);

        generate (static_cast<Generator&> (*this), count, items, vertices.data () + base_vertex);

        return base_vertex / VERTICES_PER_PRIMITIVE;
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingLineAndOrientedSpriteList<T>::Add");
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

        const size_t current_items_count = vertices.size () / VERTICES_PER_PRIMITIVE;

        if (first >= current_items_count)
          throw xtl::make_range_exception ("", "first", first, current_items_count);
        
        if (current_items_count - first < count)
          throw xtl::make_range_exception ("", "count", count, current_items_count - first);

          //обновление элементов

        size_t base_vertex = first * VERTICES_PER_PRIMITIVE;

        generate (static_cast<Generator&> (*this), count, src_items, vertices.data () + base_vertex);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingLineAndOrientedSpriteList<T>::Update");
        throw;
      }
    }

///Удаление примитивов
    void Remove (size_t first, size_t count)
    {
      const size_t current_items_count = vertices.size () / VERTICES_PER_PRIMITIVE;

      if (first >= current_items_count)
        return;

      if (current_items_count - first < count)
        count = current_items_count - first;

      if (!count)
        return;

      size_t base_vertex = first * VERTICES_PER_PRIMITIVE,
             verts_count = count * VERTICES_PER_PRIMITIVE;

      memmove (vertices.data () + base_vertex, vertices.data () + base_vertex + verts_count, sizeof (DynamicPrimitiveVertex) * verts_count);

      vertices.resize (vertices.size () - verts_count);
    }

    void Clear ()
    {
      vertices.resize (0);
    }

///Резервируемое пространство
    size_t Capacity () { return vertices.capacity () / VERTICES_PER_PRIMITIVE; }

    void Reserve (size_t count)
    {
      try
      {
        const size_t current_capacity = vertices.capacity () / VERTICES_PER_PRIMITIVE;

        vertices.reserve (count * VERTICES_PER_PRIMITIVE);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingLineAndOrientedSpriteList<T>::Reserve");
        throw;
      }      
    }

  private:
/// Вершины
    size_t VerticesCount () { return vertices.size (); }

    const DynamicPrimitiveVertex* Vertices () { return vertices.data (); }

  private:
    typedef xtl::uninitialized_storage<DynamicPrimitiveVertex> VertexArray;

  private:
    VertexArray vertices;
};

/*
    Standalone billboard Sprite list
*/

class StandaloneBillboardSpriteList: public PrimitiveListStorage<Sprite, StandalonePrimitiveHolder>
{
  typedef PrimitiveListStorage<Sprite, StandalonePrimitiveHolder> Base;
  public:
    enum { VERTICES_PER_PRIMITIVE = BillboardSpriteGenerator::VERTICES_PER_PRIMITIVE, INDICES_PER_PRIMITIVE = BillboardSpriteGenerator::INDICES_PER_PRIMITIVE };

/// Экземпляр
    template <class Generator>
    class Instance: public DynamicPrimitive, private render::manager::RendererPrimitiveGroup
    {
      public:
        typedef xtl::intrusive_ptr<StandaloneBillboardSpriteList> PrototypePtr;

/// Конструктор
        Instance (const PrototypePtr& in_prototype)
          : DynamicPrimitive (*this, DynamicPrimitiveFlag_FrameDependent | DynamicPrimitiveFlag_EntityDependent)
          , prototype (in_prototype)
        {
          primitives_count = 1;
          primitives       = &prototype->Primitive ();
        }

      private:
///Обновление
        void UpdateOnPrerenderCore (EntityImpl&) {}

        void UpdateOnRenderCore (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix)
        {
          prototype->UpdateOnRenderCore<Generator> (frame, entity, context, mvp_matrix);
        }

      private:
        PrototypePtr prototype;
    };

/// Конструктор
    StandaloneBillboardSpriteList (const MaterialManagerPtr& material_manager, SpriteMode in_mode, const math::vec3f& in_view_up, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
      : Base (material_manager, vb_usage, ib_usage, BillboardSpriteGenerator::PRIMITIVE_TYPE)
      , mode (in_mode)
      , view_up (in_view_up)
    {
    }

///Статический примитив рендеринга
    render::manager::RendererPrimitive* StandaloneRendererPrimitive () { return 0; }

///Создание экземпляра
    DynamicPrimitive* CreateDynamicPrimitiveInstanceCore ()
    {
      try
      {
        if (mode == SpriteMode_Billboard) return new Instance<BillboardSpriteGenerator> (this);
        else                              return new Instance<OrientedBillboardSpriteGenerator> (this);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingBillboardSpriteList::CreateDynamicPrimitiveInstanceCore");
        throw;
      }
    }

  private:
/// Синхронизация буферов
    void SyncBuffers ()
    {
      render::low_level::IDevice& device = DeviceManager ().Device ();

      VertexBuffer ().SyncBuffers (device);
      IndexBuffer ().SyncBuffers (device);
    }

///Обновление
    template <class Generator> void UpdateOnRenderCore (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix)
    {
      try
      {           
        Generator generator (entity, view_up, context.InverseViewProjectionMatrix ());

        generate (generator, Base::Size (), Base::Items (), 0, VertexBuffer ().Data (), IndexBuffer ().Data ());

        SyncBuffers ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::StandaloneBillboardSpriteList::UpdateOnRenderCore");
        throw;
      }
    }

/// Обновление буферов
    void UpdateBuffersCore ()
    {      
      try
      {
        size_t sprites_count = Base::Size ();

        VertexBuffer ().Clear ();
        IndexBuffer ().Clear ();

        VertexBuffer ().Reserve (sprites_count * VERTICES_PER_PRIMITIVE);
        IndexBuffer ().Reserve (sprites_count * INDICES_PER_PRIMITIVE);

        SyncBuffers ();

        VertexBuffer ().Resize (sprites_count * VERTICES_PER_PRIMITIVE);
        IndexBuffer ().Resize (sprites_count * INDICES_PER_PRIMITIVE);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::StandaloneBillboardSpriteList::UpdateBuffersCore");
        throw;
      }
    }

  private: 
    SpriteMode  mode;
    math::vec3f view_up;
};

/*
    Batching billboard Sprite list
*/

class BatchingBillboardSpriteList: public PrimitiveListStorage<Sprite, BatchingStateBlockHolder>
{
  typedef PrimitiveListStorage<Sprite, BatchingStateBlockHolder> Base;
  public:
    enum { VERTICES_PER_PRIMITIVE = BillboardSpriteGenerator::VERTICES_PER_PRIMITIVE, INDICES_PER_PRIMITIVE = BillboardSpriteGenerator::INDICES_PER_PRIMITIVE };

/// Экземпляр списка примитивов
    class InstanceBase: public BatchingInstance
    {
      public:
/// Конструктор
        InstanceBase (const PrototypePtr& prototype)
          : BatchingInstance (prototype, BillboardSpriteGenerator::PRIMITIVE_TYPE, DynamicPrimitiveFlag_FrameDependent) 
          , indices ()
        { }

      private:
///Обновление
        void UpdateOnPrerenderCore (EntityImpl& entity)
        {
          try
          {
            BatchingBillboardSpriteList& prototype = static_cast<BatchingBillboardSpriteList&> (Prototype ());

            size_t items_count = prototype.BatchingBillboardSpriteList::Size (),
                   verts_count = items_count * VERTICES_PER_PRIMITIVE,
                   inds_count  = items_count * INDICES_PER_PRIMITIVE;

            UpdatePrimitive (verts_count, inds_count, 0, &indices, 0);
          }
          catch (xtl::exception& e)
          {
            e.touch ("render::manager::BatchingLineAndOrientedSpriteList::InstanceBase::UpdateOnPrerenderCore");
            throw;
          }
        }

      protected:
        DynamicPrimitiveIndex* indices;
    };

    template <class Generator>
    class Instance: public InstanceBase
    {
      public:
/// Конструктор
        Instance (const PrototypePtr& prototype) : InstanceBase (prototype) { }

      private:
        void UpdateOnRenderCore (FrameImpl& frame, EntityImpl& entity, RenderingContext& context, const math::mat4f& mvp_matrix)
        {
          try
          {
            BatchingBillboardSpriteList& prototype = static_cast<BatchingBillboardSpriteList&> (Prototype ());

            size_t items_count = prototype.BatchingBillboardSpriteList::Size (), verts_count = items_count * VERTICES_PER_PRIMITIVE, base_vertex = 0;

            DynamicPrimitiveVertex* vertices = BatchingManager ().AllocateDynamicVertices (verts_count, &base_vertex);

              //формирование вершин

            Generator generator (entity, prototype.view_up, context.InverseViewProjectionMatrix ());

            generate (generator, items_count, prototype.Items (), vertices);

              //формирование индексов

            generate<SpriteGenerator> (items_count, base_vertex, indices);
          }
          catch (xtl::exception& e)
          {
            e.touch ("render::manager::BatchingLineAndOrientedSpriteList::Instance<T>::UpdateOnRenderCore");
            throw;
          }
        }
    };

/// Конструктор
    BatchingBillboardSpriteList (const BatchingManagerPtr& batching_manager, const MaterialManagerPtr& material_manager, SpriteMode in_mode, const math::vec3f& in_view_up)
      : Base (material_manager, batching_manager)
      , mode (in_mode)
      , view_up (in_view_up)
    {
    }

///Статический примитив рендеринга
    render::manager::RendererPrimitive* StandaloneRendererPrimitive () { return 0; }

///Создание экземпляра
    DynamicPrimitive* CreateDynamicPrimitiveInstanceCore ()
    {
      try
      {
        if (mode == SpriteMode_Billboard) return new Instance<BillboardSpriteGenerator> (this);
        else                              return new Instance<OrientedBillboardSpriteGenerator> (this);
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::manager::BatchingBillboardSpriteList::CreateDynamicPrimitiveInstanceCore");
        throw;
      }
    }

  private:
/// Обновление буферов
    void UpdateBuffersCore () { }

  private:    
    SpriteMode  mode;
    math::vec3f view_up;
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
  return new StandaloneLineAndOrientedSpriteList<Line, LineGenerator> (material_manager, vb_usage, ib_usage, LineGenerator ());
}

LineListImpl* create_batching_line_list (const BatchingManagerPtr& batching_manager, const MaterialManagerPtr& material_manager)
{
  return new BatchingLineAndOrientedSpriteList<Line, LineGenerator> (batching_manager, material_manager, LineGenerator ());
}

SpriteListImpl* create_standalone_sprite_list (const MaterialManagerPtr& material_manager, SpriteMode mode, const math::vec3f& up, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage)
{
  switch (mode)
  {
    case SpriteMode_Oriented:          return new StandaloneLineAndOrientedSpriteList<Sprite, OrientedSpriteGenerator> (material_manager, vb_usage, ib_usage, OrientedSpriteGenerator (up));
    case SpriteMode_Billboard:         
    case SpriteMode_OrientedBillboard: return new StandaloneBillboardSpriteList (material_manager, mode, up, vb_usage, ib_usage);
    default:                           throw xtl::make_argument_exception ("render::manager::create_standalone_sprite_list", "mode", mode);
  }
}

SpriteListImpl* create_batching_sprite_list (const BatchingManagerPtr& batching_manager, const MaterialManagerPtr& material_manager, SpriteMode mode, const math::vec3f& up)
{
  switch (mode)
  {
    case SpriteMode_Oriented:          return new BatchingLineAndOrientedSpriteList<Sprite, OrientedSpriteGenerator> (batching_manager, material_manager, OrientedSpriteGenerator (up));
    case SpriteMode_Billboard:         
    case SpriteMode_OrientedBillboard: return new BatchingBillboardSpriteList (batching_manager, material_manager, mode, up);
    default:                           throw xtl::make_argument_exception ("render::manager::create_batching_sprite_list", "mode", mode);
  }
}

}

}
