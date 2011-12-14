#include "shared.h"

using namespace render;

/*
    Описание реализации менеджера лэйаутов геометрии
*/

typedef stl::hash_map<size_t, LowLevelInputLayoutPtr> InputLayoutMap;

struct InputLayoutManager::Impl
{
  LowLevelDevicePtr device;    //устройство отрисовки
  InputLayoutMap    layouts;   //закэшированные лэйауты
  Log               log;       //протокол отладочных сообщений
  SettingsPtr       settings;  //настройки менеджера рендеринга
  
  Impl (const LowLevelDevicePtr& in_device, const SettingsPtr& in_settings)
    : device (in_device)
    , settings (in_settings)
  {
    static const char* METHOD_NAME = "render::InputLayoutManager::Impl";
    
    if (!device)
      throw xtl::make_null_argument_exception (METHOD_NAME, "device");
      
    if (!settings)
      throw xtl::make_null_argument_exception (METHOD_NAME, "settings");
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

InputLayoutManager::InputLayoutManager (const LowLevelDevicePtr& device, const SettingsPtr& settings)
  : impl (new Impl (device, settings))
{
}

InputLayoutManager::~InputLayoutManager ()
{
}

/*
    Создание лэйаута
*/

LowLevelInputLayoutPtr InputLayoutManager::CreateInputLayout (const render::low_level::InputLayoutDesc& desc)
{
  try
  {
    size_t hash = common::crc32 (&desc.vertex_attributes [0], sizeof (render::low_level::VertexAttribute) * desc.vertex_attributes_count);
    
    struct HashDesc
    {
      size_t vertex_attributes_count;
      int    index_type;
      size_t index_buffer_offset;
    };
    
    HashDesc hash_desc;
    
    memset (&hash_desc, 0, sizeof (hash_desc));
    
    hash_desc.vertex_attributes_count = desc.vertex_attributes_count;
    hash_desc.index_type              = desc.index_type;
    hash_desc.index_buffer_offset     = desc.index_buffer_offset;
    
    hash = common::crc32 (&hash_desc, sizeof (hash_desc), hash);
    
    InputLayoutMap::iterator iter = impl->layouts.find (hash);
    
    if (iter != impl->layouts.end ())
      return iter->second;
      
    if (impl->settings->HasDebugLog ())
    {
      impl->log.Printf ("Create input layout:");
      impl->log.Printf ("...index type: %s", get_name (desc.index_type));
      impl->log.Printf ("...index buffer offset: %u", desc.index_buffer_offset);
      impl->log.Printf ("...%u vertex attributes:", desc.vertex_attributes_count);
      
      for (size_t i=0; i<desc.vertex_attributes_count; i++)
      {
        const render::low_level::VertexAttribute& va = desc.vertex_attributes [i];
        const char* type = "";
        size_t      rank = 0;

        switch (va.format)
        {
          case render::low_level::InputDataFormat_Vector1:
          case render::low_level::InputDataFormat_Vector2: 
          case render::low_level::InputDataFormat_Vector3:
          case render::low_level::InputDataFormat_Vector4:
            rank = va.format - render::low_level::InputDataFormat_Vector1 + 1;
            break;
          default:
            rank = 0;
            break;
        }
              
        switch (va.type)
        {
          case render::low_level::InputDataType_Byte:   type = "byte"; break;
          case render::low_level::InputDataType_UByte:  type = "ubyte"; break;
          case render::low_level::InputDataType_Short:  type = "short"; break;
          case render::low_level::InputDataType_UShort: type = "ushort"; break;
          case render::low_level::InputDataType_Int:    type = "int"; break;
          case render::low_level::InputDataType_UInt:   type = "uint"; break;
          case render::low_level::InputDataType_Float:  type = "float"; break;
          default:
            break;
        }
        
        impl->log.Printf ("......semantic=%s, type=%s%u, slot=%u, offset=%u, stride=%u", get_name (va.semantic), type, rank,
          va.slot, va.offset, va.stride);
      }
    }
      
    LowLevelInputLayoutPtr layout (impl->device->CreateInputLayout (desc), false);
    
    if (impl->settings->HasDebugLog ())    
      impl->log.Printf ("...layout created (hash=%08x)", hash);
    
    impl->layouts [hash] = layout;
    
    return layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::InputLayoutManager::CreateInputLayout");
    throw;
  }
}
