#include "shared.h"

using namespace render::manager;

#ifdef _MSC_VER
  #define VERTEX_OFFSETOF(X,Y) offsetof(X,Y)
#else
  #define VERTEX_OFFSETOF(X,Y) (reinterpret_cast<size_t> (&(static_cast<X*> (0)->*(&X::Y))))
#endif

namespace media
{

namespace geometry
{

inline size_t hash (const VertexFormat& vf)
{
  return vf.Hash ();
}

}

}

/*
    Описание реализации менеджера лэйаутов геометрии
*/

typedef stl::hash_map<size_t, LowLevelInputLayoutPtr> InputLayoutMap;
typedef stl::hash_set<media::geometry::VertexFormat>  VertexFormatSet;

struct InputLayoutManager::Impl
{
  LowLevelDevicePtr      device;                    //устройство отрисовки
  InputLayoutMap         layouts;                   //закэшированные лэйауты
  VertexFormatSet        vertex_formats;            //вершинные форматы
  Log                    log;                       //протокол отладочных сообщений
  SettingsPtr            settings;                  //настройки менеджера рендеринга
  LowLevelInputLayoutPtr dynamic_primitives_layout; //лэйаут для спрайтов
  
  Impl (const LowLevelDevicePtr& in_device, const SettingsPtr& in_settings)
    : device (in_device)
    , settings (in_settings)
  {
    static const char* METHOD_NAME = "render::manager::InputLayoutManager::Impl";
    
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
{
  try
  {
    impl.reset (new Impl (device, settings));

    InitDynamicPrimitivesLayout ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::InputLayoutManager::InputLayoutManager");
    throw;
  }
}

InputLayoutManager::~InputLayoutManager ()
{
}

/*
    Получение константной копии
*/

media::geometry::VertexFormat InputLayoutManager::Clone (const media::geometry::VertexFormat& format) const
{
  try
  {
    VertexFormatSet::iterator iter = impl->vertex_formats.find (format);

    if (iter != impl->vertex_formats.end ())
      return *iter;

    if (impl->settings->HasDebugLog ())
      impl->log.Printf ("Create vertex format clone with hash %08x", format.Hash ());

    media::geometry::VertexFormat clone = format.Clone ();

    impl->vertex_formats.insert (clone);

    return clone;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::InputLayoutManager::Clone");
    throw;
  }
}

/*
    Создание лэйаута
*/

LowLevelInputLayoutPtr InputLayoutManager::CreateInputLayout (size_t hash, const render::low_level::InputLayoutDesc& desc)
{
  try
  {
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

        impl->log.Printf ("......semantic=%s, type=%s%u, slot=%u, offset=%u, stride=%u", va.semantic, type, rank, va.slot, va.offset, va.stride);
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
    e.touch ("render::manager::InputLayoutManager::CreateInputLayout");
    throw;
  }
}

/*
    Лэйауты для спрайтов и линий
*/

void InputLayoutManager::InitDynamicPrimitivesLayout ()
{
  try
  {
    static const render::low_level::VertexAttribute attributes [] = {
      {impl->device->GetVertexAttributeSemanticName (render::low_level::VertexAttributeSemantic_Position),  render::low_level::InputDataFormat_Vector3, render::low_level::InputDataType_Float, 0, VERTEX_OFFSETOF (DynamicPrimitiveVertex, position),  sizeof (DynamicPrimitiveVertex)},
      {impl->device->GetVertexAttributeSemanticName (render::low_level::VertexAttributeSemantic_Normal),    render::low_level::InputDataFormat_Vector3, render::low_level::InputDataType_Float, 0, VERTEX_OFFSETOF (DynamicPrimitiveVertex, normal),    sizeof (DynamicPrimitiveVertex)},
      {impl->device->GetVertexAttributeSemanticName (render::low_level::VertexAttributeSemantic_Color),     render::low_level::InputDataFormat_Vector4, render::low_level::InputDataType_Float, 0, VERTEX_OFFSETOF (DynamicPrimitiveVertex, color),     sizeof (DynamicPrimitiveVertex)},
      {impl->device->GetVertexAttributeSemanticName (render::low_level::VertexAttributeSemantic_TexCoord0), render::low_level::InputDataFormat_Vector2, render::low_level::InputDataType_Float, 0, VERTEX_OFFSETOF (DynamicPrimitiveVertex, tex_coord), sizeof (DynamicPrimitiveVertex)},
    };

    static const size_t attributes_count = sizeof (attributes) / sizeof (*attributes);

    size_t attributes_hash = GetVertexAttributesHash (attributes_count, attributes);

    render::low_level::InputLayoutDesc desc;

    memset (&desc, 0, sizeof desc);

    (void)xtl::compile_time_assert<xtl::type_traits::is_same<unsigned short, DynamicPrimitiveIndex>::value>::value;

    desc.vertex_attributes_count = attributes_count;
    desc.vertex_attributes       = &attributes [0];
    desc.index_type              = render::low_level::InputDataType_UShort;
    desc.index_buffer_offset     = 0;

    impl->dynamic_primitives_layout = CreateInputLayout (attributes_hash, desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::InputLayoutManager::InitDynamicPrimitivesLayout");
    throw;
  }
}

const LowLevelInputLayoutPtr& InputLayoutManager::DynamicPrimitivesInputLayout () const
{
  return impl->dynamic_primitives_layout;
}

/*
    Получение хэша атрибутов
*/

size_t InputLayoutManager::GetVertexAttributesHash (size_t count, const render::low_level::VertexAttribute* attributes)
{
  size_t attributes_hash = 0xFFFFFFFF;

  const render::low_level::VertexAttribute* attr = attributes;

  for (size_t i=0; i<count; i++, attr++)
    attributes_hash = common::crc32 ((char*)&attr->format, (char*)&attr [1] - (char*)&attr->format, attributes_hash);

  return attributes_hash;
}
