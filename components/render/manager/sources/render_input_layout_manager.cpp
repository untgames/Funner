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
  DebugLog          debug_log; //протокол отладочных сообщений
  
  Impl (const LowLevelDevicePtr& in_device) : device (in_device) {}
};

/*
    Конструкторы / деструктор / присваивание
*/

InputLayoutManager::InputLayoutManager (const LowLevelDevicePtr& in_device)
  : impl (new Impl (in_device))
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
    size_t hash = common::crc32 (&desc, sizeof (desc));
    
    InputLayoutMap::iterator iter = impl->layouts.find (hash);
    
    if (iter != impl->layouts.end ())
      return iter->second;
      
    impl->debug_log.Printf ("Create input layout:");
    impl->debug_log.Printf ("...index type: %s", get_name (desc.index_type));
    impl->debug_log.Printf ("...index buffer offset: %u", desc.index_buffer_offset);
    impl->debug_log.Printf ("...%u vertex attributes:", desc.vertex_attributes_count);
    
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
      
      impl->debug_log.Printf ("......semantic=%s, type=%s%u, slot=%u, offset=%u, stride=%u", get_name (va.semantic), type, rank,
        va.slot, va.offset, va.stride);
    }
      
    LowLevelInputLayoutPtr layout (impl->device->CreateInputLayout (desc), false);
    
    impl->debug_log.Printf ("...layout created (hash=%08x)", hash);
    
    impl->layouts [hash] = layout;
    
    return layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::InputLayoutManager::CreateInputLayout");
    throw;
  }
}
