#ifndef RENDER_GL_DRIVER_RENDER_TARGET_MANAGER_SHARED_HEADER
#define RENDER_GL_DRIVER_RENDER_TARGET_MANAGER_SHARED_HEADER

#include <stl/hash_map>

#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/trackable_ptr.h>
#include <xtl/uninitialized_storage.h>

#include <common/strlib.h>

#include <render/low_level/utils.h>

#include <shared/context_object.h>
#include <shared/render_target_manager.h>
#include <shared/texture_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Элементы таблицы локальных данных контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RenderTargetManagerCacheEntry
{
  CacheEntry_FrameBufferId = CacheEntry_RenderTargetManagerPrivateFirst, //идентификатор текущего буфера кадра
  CacheEntry_BufferAttachment,  //тип буфера
  CacheEntry_ClearDepthHash,    //хэш значения очистки буфера глубины
  CacheEntry_ClearStencilValue, //значение очистки буфера трафарета
  CacheEntry_ClearColorHash0,   //хэш цвета очистки буфера цвета 0
  CacheEntry_ClearColorHash1,   //хэш цвета очистки буфера цвета 1
  CacheEntry_ClearColorHash2,   //хэш цвета очистки буфера цвета 2
  CacheEntry_ClearColorHash3,   //хэш цвета очистки буфера цвета 3
  CacheEntry_ClearColorHash4,   //хэш цвета очистки буфера цвета 4
  CacheEntry_ClearColorHash5,   //хэш цвета очистки буфера цвета 5
  CacheEntry_ClearColorHash6,   //хэш цвета очистки буфера цвета 6
  CacheEntry_ClearColorHash7,   //хэш цвета очистки буфера цвета 7
  CacheEntry_ViewportHash0,     //хэш состояния области вывода 0
  CacheEntry_ViewportHash1,     //хэш состояния области вывода 1
  CacheEntry_ViewportHash2,     //хэш состояния области вывода 2
  CacheEntry_ViewportHash3,     //хэш состояния области вывода 3
  CacheEntry_ViewportHash4,     //хэш состояния области вывода 4
  CacheEntry_ViewportHash5,     //хэш состояния области вывода 5
  CacheEntry_ViewportHash6,     //хэш состояния области вывода 6
  CacheEntry_ViewportHash7,     //хэш состояния области вывода 7
  CacheEntry_ScissorHash0,      //хэш состояния области отсечения 0
  CacheEntry_ScissorHash1,      //хэш состояния области отсечения 1
  CacheEntry_ScissorHash2,      //хэш состояния области отсечения 2
  CacheEntry_ScissorHash3,      //хэш состояния области отсечения 3
  CacheEntry_ScissorHash4,      //хэш состояния области отсечения 4
  CacheEntry_ScissorHash5,      //хэш состояния области отсечения 5
  CacheEntry_ScissorHash6,      //хэш состояния области отсечения 6
  CacheEntry_ScissorHash7,      //хэш состояния области отсечения 7
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип цели рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RenderTargetType
{
  RenderTargetType_Color,        //буфер цвета
  RenderTargetType_DepthStencil, //буфер глубина-трафарет
  
  RenderTargetType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderBuffer: virtual public ITexture, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               GetDesc (TextureDesc&);
    const TextureDesc& GetDesc () const { return desc; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение типа буфера
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetType GetTargetType () const { return target_type; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с данными
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat source_format, const void* buffer, IDeviceContext*);
    void GetData (unsigned int layer, unsigned int mip_level, unsigned int x, unsigned int y, unsigned int width, unsigned int height, PixelFormat target_format, void* buffer, IDeviceContext*);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderBuffer (const ContextManager& context_manager, RenderTargetType target_type);
    RenderBuffer (const ContextManager& context_manager, const TextureDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение размеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetSize (unsigned int width, unsigned int height);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка буфера в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;

  private:
    RenderTargetType target_type;  //тип буфера
    TextureDesc      desc;         //дескриптор текстуры
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отображение
///////////////////////////////////////////////////////////////////////////////////////////////////
class View: virtual public IView, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    View  (const ContextManager&, ITexture* texture, const ViewDesc& desc);
    ~View ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение целевой текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* GetTexture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение дескриптора / флагов биндинга текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         GetDesc      (ViewDesc&);
    unsigned int GetBindFlags () const { return bind_flags; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение строкового имени типа текстуры
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetTextureTypeName () { return type.name (); }

  private:
    typedef xtl::com_ptr<ITexture> TexturePtr;

  private:
    TexturePtr            texture;    //указатель на текстуру
    const std::type_info& type;       //тип отображения
    ViewDesc              desc;       //дескриптор отображения
    unsigned int          bind_flags; //флаги биндинга
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс буфера кадра
///////////////////////////////////////////////////////////////////////////////////////////////////
class IFrameBuffer: public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////  
    virtual ~IFrameBuffer () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка буфера в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оповещение об отрисовке в целевые буферы
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void InvalidateRenderTargets (unsigned int render_target_slot, const Rect& update_rect) = 0;
    virtual void InvalidateRenderTargets (unsigned int render_target_slot) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление целевых буферов отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateRenderTargets () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Функторы создания буфера кадра / буфера рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::function<IFrameBuffer* (View* render_target_view, View* depth_stencil_view)> FrameBufferCreater;
typedef xtl::function<bool (View* render_target_view, View* depth_stencil_view)>          FrameBufferChecker;
typedef xtl::function<ITexture* (const TextureDesc&)>                                     RenderBufferCreater;
typedef xtl::function<ITexture* (ISwapChain*, unsigned int)>                              ColorBufferCreater;
typedef xtl::function<ITexture* (ISwapChain*)>                                            DepthStencilBufferCreater;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс реестра буферов кадра
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetRegistry
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetRegistry  ();
    ~RenderTargetRegistry ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация обработчиков создания буферов кадра и отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterCreater (const FrameBufferChecker& frame_buffer_checker, const FrameBufferCreater& frame_buffer_creater);
    void RegisterCreater (const RenderBufferCreater&);
    void RegisterCreater (const ColorBufferCreater&);
    void RegisterCreater (const DepthStencilBufferCreater&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание буферов рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* CreateRenderBuffer       (const TextureDesc& desc);
    ITexture* CreateColorBuffer        (ISwapChain* swap_chain, unsigned int index);
    ITexture* CreateDepthStencilBuffer (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение буфера кадра по двум отображениям
///////////////////////////////////////////////////////////////////////////////////////////////////
    IFrameBuffer& GetFrameBuffer (View* render_target_view, View* depth_stencil_view);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Освобождение ресурсов, связанных с отображением
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ReleaseResources (View*);

  private:
    RenderTargetRegistry (const RenderTargetRegistry&); //no impl
    RenderTargetRegistry& operator = (const RenderTargetRegistry&); //no impl
    
    IFrameBuffer* CreateFrameBuffer (View* render_target_view, View* depth_stencil_view);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация фабрик буферов кадра / рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
void register_swap_chain_manager (RenderTargetRegistry& registry, const ContextManager& manager, ISwapChain* default_swap_chain);
void register_fbo_manager        (RenderTargetRegistry& registry, const ContextManager& manager, ISwapChain* default_swap_chain);

}

}

}

#endif
