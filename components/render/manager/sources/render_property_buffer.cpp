#include "shared.h"

using namespace render::manager;
using namespace common;

/*
    Константы
*/

const size_t MIN_BUFFER_SIZE = 16; //минимальный размер буфера констант

/*
    Описание реализации буфера свойств
*/

struct PropertyBuffer::Impl
{
  DeviceManagerPtr     device_manager;     //менеджер устройства отрисовки
  LowLevelBufferPtr    buffer;             //константный буфер
  common::PropertyMap  properties;         //исходные свойства
  xtl::auto_connection update_connection;  //соединение обновления свойств
  bool                 need_update;        //флаг необходимости обновления буфера
  size_t               cached_buffer_size; //закэшированный размер буфера

///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , need_update (true)
    , cached_buffer_size (0)
  {
  }  
};

/*
    Конструктор / деструктор
*/

PropertyBuffer::PropertyBuffer (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
  impl->update_connection = impl->properties.RegisterEventHandler (PropertyMapEvent_OnUpdate, xtl::bind (&PropertyBuffer::OnPropertiesUpdated, this));
}

PropertyBuffer::~PropertyBuffer ()
{
}

/*
    Установка свойств
*/

void PropertyBuffer::SetProperties (const common::PropertyMap& in_properties)
{    
  if (impl->properties.Id () == in_properties.Id ())
    return;

  impl->update_connection = in_properties.RegisterEventHandler (PropertyMapEvent_OnUpdate, xtl::bind (&PropertyBuffer::OnPropertiesUpdated, this));
  impl->properties        = in_properties;  
  impl->need_update       = true;
  
  InvalidateCache (false);
}                    

const common::PropertyMap& PropertyBuffer::Properties ()
{
  return impl->properties;
}

/*
    Свойства обновлены
*/

void PropertyBuffer::OnPropertiesUpdated ()
{
  if (impl->need_update)
    return;
    
  impl->need_update = true;  
    
  InvalidateCache (false);
}

/*
    Получение буфера
*/

const LowLevelBufferPtr& PropertyBuffer::Buffer ()
{
  try
  {
    if (!impl->need_update)
      return impl->buffer;

    UpdateCache ();
        
    return impl->buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PropertyBuffer::Buffer");
    throw;
  }
}

/*
    Обновление кэша
*/

void PropertyBuffer::UpdateCacheCore ()
{
  try
  {
    if (!impl->need_update)
      return;      
    
    size_t buffer_size = impl->properties.BufferSize ();    
    
    bool need_recreate_buffer = !impl->buffer || buffer_size > impl->cached_buffer_size;
    
    if (need_recreate_buffer)
    {
        //создание нового буфера
        
      render::low_level::BufferDesc desc;

      memset (&desc, 0, sizeof (desc));
      
      desc.size         = stl::max (buffer_size, MIN_BUFFER_SIZE);
      desc.usage_mode   = render::low_level::UsageMode_Default;
      desc.bind_flags   = render::low_level::BindFlag_ConstantBuffer;
      desc.access_flags = render::low_level::AccessFlag_ReadWrite;
      
      impl->buffer             = LowLevelBufferPtr (impl->device_manager->Device ().CreateBuffer (desc), false);
      impl->cached_buffer_size = desc.size;      
      
      InvalidateCacheDependencies ();
    }

    impl->buffer->SetData (0, buffer_size, impl->properties.BufferData ());

    impl->need_update = false;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PropertyBuffer::UpdateCacheCore");
    throw;
  }
}
