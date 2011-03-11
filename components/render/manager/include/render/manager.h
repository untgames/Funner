#ifndef RENDER_MANAGER_MANAGER_HEADER
#define RENDER_MANAGER_MANAGER_HEADER

///???экспортируемые свойства рендеринга из материалов через entity

#include <render/dynamic_texture.h>
#include <render/entity.h>
#include <render/frame.h>
#include <render/material.h>
#include <render/texture.h>
#include <render/window.h>

namespace syslib
{

//forward declaration
class Window;

}

namespace render
{

//implementation forwards
class RenderManagerImpl;
class Wrappers;

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
    render::Window CreateWindow (syslib::Window& window, common::PropertyMap& properties);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор окон рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t               WindowsCount () const;
    const render::Window Window       (size_t index) const;
          render::Window Window       (size_t index);

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
    Texture          CreateTexture          (const media::Image& image, bool generate_mipmaps = true);
    Texture          CreateTexture          (const media::Image& image, TextureDimension dimension, bool generate_mipmaps = true);
    Texture          CreateTexture2D        (size_t width, size_t height, PixelFormat format, bool generate_mipmaps = true);
    Texture          CreateTexture3D        (size_t width, size_t height, size_t depth, PixelFormat format, bool generate_mipmaps = true);
    Texture          CreateTextureCubemap   (size_t size, PixelFormat format, bool generate_mipmaps = true);
    Material         CreateMaterial         ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание совместно используемых ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture   CreateSharedTexture   (const char* name);
    Material  CreateSharedMaterial  (const char* name);
    Primitive CreateSharedPrimitive (const char* name);

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
/// освобождается при удалении объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<IDynamicTexture* (size_t args_count, const char** args, Entity& entity)> DynamicTextureCreator;

    void RegisterDynamicTexture       (const char* name_mask, const DynamicTextureCreator& creator);
    void UnregisterDynamicTexture     (const char* name_mask);
    void UnregisterAllDynamicTextures ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация на события
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (RenderManager& manager, render::Window& window)> WindowEventHandler;

    xtl::connection RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const;

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

}

#endif
