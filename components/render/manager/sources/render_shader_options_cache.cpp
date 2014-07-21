#include "shared.h"

using namespace render::manager;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) // this used in base initializer list
#endif

namespace
{

///Элемент кэша опций шейдера
struct ShaderOptionsCacheEntry: public ShaderOptions, public xtl::reference_counter
{
  size_t layout_hash;    //закэшированный хэш расположения свойств
  size_t options_count;  //закэшированное количество опций
  bool   is_valid;       //является ли данный элемент корректным
  
///Конструктор
  ShaderOptionsCacheEntry () : layout_hash (0xffffffff), options_count (0), is_valid (false) {}
  
///Обновление кэша
  void UpdateCache (const common::PropertyMap& properties, ShaderOptionsLayout& layout)
  {
    if (is_valid && layout.Hash () == layout_hash && options_count == layout.Size ())
      return;
      
    layout.GetShaderOptions (properties, *this);
    
    layout_hash   = layout.Hash ();
    options_count = layout.Size ();
    is_valid      = true;
  }
};

typedef xtl::intrusive_ptr<ShaderOptionsCacheEntry>                ShaderOptionsCacheEntryPtr;
typedef CacheMap<ShaderOptionsLayout*, ShaderOptionsCacheEntryPtr> ShaderOptionsCacheEntryMap;

}

/*
    Описание реализации кэша опций шейдера
*/

struct ShaderOptionsCache::Impl: public xtl::trackable
{
  ShaderOptionsCache&               owner;
  common::PropertyMap               properties;                //полный список опций
  common::PropertyMap::EventHandler properties_update_handler; //обработчик события обновления свойств
  xtl::auto_connection              on_properties_update;      //соединение с обработчиком обновления свойств
  ShaderOptionsCacheEntryMap        cache;                     //кэш опций шейдера
  bool                              need_invalidate_cache;     //кэш требуется обновить
  
///Конструктор
  Impl (ShaderOptionsCache& in_owner, const CacheManagerPtr& cache_manager)
    : owner (in_owner)
    , properties_update_handler (xtl::bind (&Impl::OnPropertiesUpdate, this))
    , cache (cache_manager)
    , need_invalidate_cache (false)
  {
    on_properties_update  = properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, properties_update_handler); 
  }
  
///Обработчик события обновления свойств
  void OnPropertiesUpdate ()
  {
    need_invalidate_cache = true;

    owner.InvalidateCache ();
  }
  
///Обработчик события удаления расположения опций шейдера
  void OnLayoutDeleted (ShaderOptionsLayout* layout)
  {
    cache.Remove (layout);
  }
};

/*
    Конструктор / деструктор
*/

ShaderOptionsCache::ShaderOptionsCache (const CacheManagerPtr& cache_manager)
  : impl (new Impl (*this, cache_manager))
{
}

ShaderOptionsCache::~ShaderOptionsCache ()
{
}

/*
    Количество закэшированных расположений опций шейдера
*/

size_t ShaderOptionsCache::CachedLayoutsCount ()
{
  return impl->cache.Size ();
}

/*
    Полный список опций
*/

void ShaderOptionsCache::SetProperties (const common::PropertyMap& properties)
{
  impl->on_properties_update  = properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, impl->properties_update_handler);
  impl->properties            = properties;
  impl->need_invalidate_cache = true;
}

const common::PropertyMap& ShaderOptionsCache::Properties ()
{
  return impl->properties;
}

/*
    Получение опций шейдера
*/

namespace
{

struct InvalidateCacheEntry
{
  void operator () (ShaderOptionsLayout*, const ShaderOptionsCacheEntryPtr& entry) const
  {
    entry->is_valid = false;
  }
};

}

const ShaderOptions& ShaderOptionsCache::GetShaderOptions (ShaderOptionsLayout& layout)
{
  try
  {
      //обработка частного случая - отсутствия свойств
      
    if (impl->properties.Size () == 0 || layout.Size () == 0)
      return xtl::singleton_default<ShaderOptions>::instance ();
    
      //обновление флагов кэширования
    
    if (impl->need_invalidate_cache)
    {
      impl->cache.ForEach (InvalidateCacheEntry ());
        
      impl->need_invalidate_cache = false;
    }
     
      //поиск в кэше
      
    if (ShaderOptionsCacheEntryPtr* entry = impl->cache.Find (&layout))
    {
      (*entry)->UpdateCache (impl->properties, layout);
      return **entry;
    }
    
      //добавление элемента кэша
      
    ShaderOptionsCacheEntryPtr entry (new ShaderOptionsCacheEntry, false);
    
    xtl::connection on_deleted_connection = layout.GetTrackable ().connect_tracker (xtl::bind (&Impl::OnLayoutDeleted, &*impl, &layout), *impl);
    
    try
    {
      impl->cache.Add (&layout, entry);
      
      entry->UpdateCache (impl->properties, layout);
      
      return *entry;
    }
    catch (...)
    {
      on_deleted_connection.disconnect ();
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::ShaderOptionsCache::GetShaderOptions");
    throw;
  }
}
