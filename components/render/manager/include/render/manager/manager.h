#ifndef RENDER_MANAGER_MANAGER_HEADER
#define RENDER_MANAGER_MANAGER_HEADER

#include <xtl/functional_fwd>

#include <render/manager/dynamic_texture.h>
#include <render/manager/entity.h>
#include <render/manager/frame.h>
#include <render/manager/material.h>
#include <render/manager/texture.h>
#include <render/manager/window.h>

namespace syslib
{

//forward declaration
class Window;

}

namespace common
{

//forward declaration
class ParseNode;

}

namespace render
{

namespace manager
{

//implementation forwards
class RenderManagerImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Уровень протоколирования
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LogLevel
{
  LogLevel_Info,  //информационные сообщения
  LogLevel_Debug, //отладочные сообщения
  
  LogLevel_Default = LogLevel_Debug
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///События менеджера рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RenderManagerEvent
{
  RenderManagerEvent_OnConfigurationChanged, //конфигурация рендеринга изменена

  RenderManagerEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Оконные события менеджера рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RenderManagerWindowEvent
{
  RenderManagerWindowEvent_OnAdded,   //добавлено новое окно рендеринга
  RenderManagerWindowEvent_OnRemoved, //удалено окно рендеринга
  
  RenderManagerWindowEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderManager
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderManager  ();
    RenderManager  (const RenderManager&);
    ~RenderManager ();
    
    RenderManager& operator = (const RenderManager&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Строка описания устройства рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание окна рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    manager::Window CreateWindow (syslib::Window& window, common::PropertyMap& properties);
    manager::Window CreateWindow (INativeWindow* window, common::PropertyMap& properties);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор окон рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                WindowsCount () const;
    const manager::Window Window       (size_t index) const;
          manager::Window Window       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание целей рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget CreateRenderBuffer       (size_t width, size_t height, PixelFormat format);
    RenderTarget CreateDepthStencilBuffer (size_t width, size_t height);
                                                         
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание примитивов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Primitive        CreatePrimitive        ();
    Primitive        CreatePrimitive        (const PrimitiveBuffers&);
    PrimitiveBuffers CreatePrimitiveBuffers (MeshBufferUsage lines_usage = MeshBufferUsage_Stream, MeshBufferUsage sprites_usage = MeshBufferUsage_Stream);    
    Frame            CreateFrame            ();
    Entity           CreateEntity           ();
    Texture          CreateTexture          (const char* name);
    Texture          CreateTexture          (const media::Image& image, bool generate_mipmaps = true);
    Texture          CreateTexture          (const media::Image& image, TextureDimension dimension, bool generate_mipmaps = true);
    Texture          CreateTexture          (const media::CompressedImage& image);
    Texture          CreateTexture          (const media::CompressedImage& image, TextureDimension dimension);
    Texture          CreateTexture2D        (size_t width, size_t height, PixelFormat format, bool generate_mipmaps = true);
    Texture          CreateTexture3D        (size_t width, size_t height, size_t depth, PixelFormat format, bool generate_mipmaps = true);
    Texture          CreateTextureCubemap   (size_t size, PixelFormat format, bool generate_mipmaps = true);
    Material         CreateMaterial         ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание/поиск совместно используемых ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture   CreateSharedTexture   (const char* name);
    Material  CreateSharedMaterial  (const char* name);
    Primitive CreateSharedPrimitive (const char* name);
    bool      HasSharedTexture      (const char* name);
    bool      HasSharedMaterial     (const char* name);
    bool      HasSharedPrimitive    (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация совместно используемых ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ShareTexture     (const char* name, const Texture& texture);
    void ShareMaterial    (const char* name, const Material& material);
    void SharePrimitive   (const char* name, const Primitive& primitive);
    void UnshareTexture   (const char* name);
    void UnshareMaterial  (const char* name);
    void UnsharePrimitive (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Применение настроек / получение конфигурации ренедринга эффектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              ChangeSettings (const common::PropertyMap& settings);
    common::ParseNode Configuration  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource   (const char* resource_name);
    void LoadResource   (const media::rfx::MaterialLibrary&);
    void LoadResource   (const media::geometry::MeshLibrary&);
    void UnloadResource (const char* resource_name);
    void UnloadResource (const media::rfx::MaterialLibrary&);
    void UnloadResource (const media::geometry::MeshLibrary&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация динамических текстур
///  Примечание: передаваемый при создании текстуры параметр Entity нужно хранить по ссылке и не
///  захватывать, это может привести к циклической ссылке и к утечке ресурсов. Текстура всегда 
///  освобождается при удалении объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<IDynamicTexture* (const char* name, Entity& entity)> DynamicTextureCreator;

    void RegisterDynamicTexture       (const char* name_mask, const DynamicTextureCreator& creator);
    void UnregisterDynamicTexture     (const char* name_mask);
    void UnregisterAllDynamicTextures ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшированием
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройка параметров кэширования временных объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetCacheTimeDelay  (size_t milliseconds);
    void   SetCacheFrameDelay (size_t frames_count);
    size_t CacheTimeDelay     () const;
    size_t CacheFrameDelay    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация на события
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (RenderManager& manager, manager::Window& window)> WindowEventHandler;
    typedef xtl::function<void (RenderManager& manager)>                         EventHandler;

    xtl::connection RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const;
    xtl::connection RegisterEventHandler       (RenderManagerEvent event, const EventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Уровень протоколирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetLogLevel (manager::LogLevel level);
    manager::LogLevel LogLevel    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение объекта оповещения об удалении
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::trackable& GetTrackable () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RenderManager&);
    
  private:
    RenderManager (RenderManagerImpl*);
    
  private:
    RenderManagerImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RenderManager&, RenderManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение объекта оповещения об удалении
///////////////////////////////////////////////////////////////////////////////////////////////////
xtl::trackable& get_trackable (const RenderManager&);

}

}

#endif
