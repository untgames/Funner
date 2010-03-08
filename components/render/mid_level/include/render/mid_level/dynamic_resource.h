#ifndef RENDER_MID_LEVEL_DYNAMIC_RESOURCE_HEADER
#define RENDER_MID_LEVEL_DYNAMIC_RESOURCE_HEADER

#include <xtl/functional_fwd>

#include <render/frame.h>

namespace render
{

namespace mid_level
{

//forward declaration
class RenderManager;
class Texture;
class Primitive;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс динамического ресурса
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Resource> class IDynamicResource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение ресурса
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Resource GetResource (Frame& frame, Entity& entity) = 0;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчёт ссылок
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IDynamicResource () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер динамических ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Resource> class DynamicResourceManager
{
  public:
    typedef IDynamicResource<Resource>                                                 DynamicResource;
    typedef xtl::function<DynamicResource* (RenderManager& manager, const char* name)> DynamicResourceCreator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Register      (const char* name, const DynamicResourceCreator& creator);
    static void Unregister    (const char* name);
    static void UnregisterAll ();
};

typedef IDynamicResource<Texture>         DynamicTexture;
typedef IDynamicResource<Primitive>       DynamicPrimitive;
typedef DynamicResourceManager<Texture>   DynamicTextureManager;
typedef DynamicResourceManager<Primitive> DynamicPrimitiveManager;

template DynamicTextureManager;
template DynamicPrimitiveManager;

}

}

#endif
