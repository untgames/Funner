#include "shared.h"

using namespace render;
using namespace common;

/*
    Описание реализации буфера свойств
*/

struct PropertyBuffer::Impl
{
  DeviceManagerPtr     device_manager;    //менеджер устройства отрисовки
  LowLevelBufferPtr    buffer;            //константный буфер
  common::PropertyMap  properties;        //исходные свойства
  xtl::auto_connection update_connection; //соединение обновления свойств
  bool                 need_update;       //флаг необходимости обновления буфера

///Конструктор
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)
    , need_update (true)
  {
  }
  
};

/*
    Конструктор / деструктор
*/

PropertyBuffer::PropertyBuffer (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
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
  
  Invalidate ();
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

      //создание нового буфера
      
    render::low_level::BufferDesc desc;

    memset (&desc, 0, sizeof (desc));
    
    desc.size         = impl->properties.Layout ().BufferSize ();
    desc.usage_mode   = render::low_level::UsageMode_Default;
    desc.bind_flags   = render::low_level::BindFlag_ConstantBuffer;
    desc.access_flags = render::low_level::AccessFlag_ReadWrite;
    
    impl->buffer      = LowLevelBufferPtr (impl->device_manager->Device ().CreateBuffer (desc), false);
    impl->need_update = false;
    
    return impl->buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PropertyBuffer::Buffer");
    throw;
  }
}
