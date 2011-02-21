#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
===================================================================================================
    EffectLoaderLibrary::Library<T>
===================================================================================================
*/

template <class Ptr>
struct EffectLoaderLibrary::Library<Ptr>::Impl
{
  typedef stl::hash_map<stl::hash_key<const char*>, Ptr> ItemMap;
  
  ItemMap items; //элементы библиотеки
};

/*
    Конструктор / деструктор
*/

template <class Ptr>
EffectLoaderLibrary::Library<Ptr>::Library ()
  : impl (new Impl)
{
}

template <class Ptr>
EffectLoaderLibrary::Library<Ptr>::~Library ()
{
}

/*
    Количество элементов
*/

template <class Ptr>
size_t EffectLoaderLibrary::Library<Ptr>::Size ()
{
  return impl->items.size ();
}

/*
    Добавление / удаление элементов
*/

template <class Ptr>
void EffectLoaderLibrary::Library<Ptr>::Add (const char* id, const Item& item)
{
  static const char* METHOD_NAME = "render::mid_level::EffectLoaderLibrary::Library<T>";

  if (!id)
    throw xtl::make_null_argument_exception (METHOD_NAME, "id");
    
  if (impl->items.find (id) != impl->items.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "id", id, "Item has already registered");
    
  if (!item)
    throw xtl::make_null_argument_exception (METHOD_NAME, "item");

  impl->items [id] = item;
}

template <class Ptr>
void EffectLoaderLibrary::Library<Ptr>::Remove (const char* id)
{
  if (!id)
    return;
    
  impl->items.erase (id);
}

template <class Ptr>
void EffectLoaderLibrary::Library<Ptr>::Clear ()
{
  impl->items.clear ();
}

/*
    Поиск элемента
*/

template <class Ptr>
typename EffectLoaderLibrary::Library<Ptr>::Item EffectLoaderLibrary::Library<Ptr>::Find (const char* id)
{
  if (!id)
    return Item ();
    
  typename ItemMap::iterator iter = impl->items.find (id);
  
  if (iter == impl->items.end ())
    return Item ();
    
  return iter->second;
}

/*
===================================================================================================
    EffectLoaderLibrary
===================================================================================================
*/

struct EffectLoaderLibrary::Impl
{
  RasterizerStateLibrary   rasterizer_states;     //библиотека состояний растеризатора
  BlendStateLibrary        blend_states;          //библиотека состояний уровня смешивания цветов
  DepthStencilStateLibrary depth_stencil_states;  //библиотека состояний уровня отсечения
  SamplerStateLibrary      sampler_states;        //библиотека состояний текстурных сэмплеров
  ProgramLibrary           programs;              //библиотека программ
};

/*
    Конструктор / деструктор
*/

EffectLoaderLibrary::EffectLoaderLibrary ()
  : impl (new Impl)
{
}

EffectLoaderLibrary::~EffectLoaderLibrary ()
{
}

/*
    Доступные библиотеки
*/

EffectLoaderLibrary::RasterizerStateLibrary& EffectLoaderLibrary::RasterizerStates ()
{
  return impl->rasterizer_states;
}

EffectLoaderLibrary::BlendStateLibrary& EffectLoaderLibrary::BlendStates ()
{
  return impl->blend_states;
}

EffectLoaderLibrary::DepthStencilStateLibrary& EffectLoaderLibrary::DepthStencilStates ()
{
  return impl->depth_stencil_states;
}

EffectLoaderLibrary::SamplerStateLibrary& EffectLoaderLibrary::SamplerStates ()
{
  return impl->sampler_states;
}

EffectLoaderLibrary::ProgramLibrary& EffectLoaderLibrary::Programs ()
{
  return impl->programs;
}
