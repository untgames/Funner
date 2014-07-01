#include "shared.h"

using namespace render::manager;

/*
    Описание реализации
*/

struct DynamicTextureImpl::Impl
{
  stl::string      name;            //имя динамической текстуры
  IDynamicTexture* dynamic_texture; //ссылка на динамическую текстуру
  TexturePtr       texture;         //текстура

  Impl (const char* in_name, EntityImpl& entity, const RenderManager::DynamicTextureCreator& creator)
    : name (in_name ? in_name : "")
    , dynamic_texture (0)
  {
    try
    {
      if (!in_name)
        throw xtl::make_null_argument_exception ("", "name");
        
      Entity entity_wrapper = Wrappers::Wrap<Entity> (&entity);
        
      dynamic_texture = creator (name.c_str (), entity_wrapper);
      
      if (!dynamic_texture)
        throw xtl::format_operation_exception ("", "Dynamic texture creator failed for texture '%s'", name.c_str ());
      
      try
      {
        texture = Wrappers::Unwrap<TextureImpl> (dynamic_texture->Texture ());
        
        if (!texture)
          throw xtl::format_operation_exception ("", "IDynamicTexture::Texture failed for texture '%s'", name.c_str ());
      }
      catch (...)
      {
        dynamic_texture->Release ();
        throw;
      }
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::DynamicTextureImpl::Impl::Impl");
      throw;
    }
  }
  
  ~Impl ()
  {
    dynamic_texture->Release ();
  }
};

/*
    Конструкторы / деструктор
*/

DynamicTextureImpl::DynamicTextureImpl (const char* name, EntityImpl& entity, const RenderManager::DynamicTextureCreator& creator)
  : impl (new Impl (name, entity, creator))
{
}

DynamicTextureImpl::~DynamicTextureImpl ()
{
}

/*
    Имя
*/

const char* DynamicTextureImpl::Name ()
{
  return impl->name.c_str ();
}

/*
    Текстура
*/

TexturePtr DynamicTextureImpl::Texture ()
{
  return impl->texture;
}

/*
    Обновление
*/

void DynamicTextureImpl::Update (const FramePtr& frame)
{
  try
  {
    if (!frame)
      throw xtl::make_null_argument_exception ("", "frame");
      
    Frame frame_wrapper = Wrappers::Wrap<Frame> (frame);
    
    impl->dynamic_texture->Update (frame_wrapper);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::DynamicTextureImpl::Update");
    throw;
  }
}
