#include "shared.h"
#include <shared/object.h>
#include <render/low_level/utils.h>
#include <stl/vector>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

class InputLayoutState: public IInputLayout, public Object
{
  public:
    InputLayoutState(const InputLayoutDesc& desc)
    {
      try
      {
        SetDesc(desc);
      }
      catch (common::Exception& exception)
      {
        exception.Touch("render::low_level::opengl::InputLayoutState::InputLayoutState(const InputLayoutDesc& desc)");
      }
    }
    
    void GetDesc (InputLayoutDesc& desc)
    {
      desc = input_desc;
    }
    
    void SetDesc (const InputLayoutDesc& desc)
    {
      if (desc.index_type >= InputDataType_Num)
        RaiseInvalidArgument("render::low_level::opengl::InputLayoutState::SetDesc (const InputLayoutDesc& desc)", "desc.index_type", desc.index_type);

      input_desc = desc;
    }
    
  private:
    InputLayoutDesc   input_desc;
};


/*
================================================================================
    Описание реализации входного уровня конвейера OpenGL
================================================================================
*/

struct InputStage::Impl: public ContextObject
{
  //////////////////////////////////////////
  /// конструктор
  //////////////////////////////////////////

  Impl (const ContextManager& context_manager) : ContextObject (context_manager)
  {
    for (int i = 0; i < DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
      vertex_buffer_slots[i] = NULL;
    
    input_state   = NULL;
    index_buffer  = NULL;
    
    MakeContextCurrent();

    if (has_ARB_multitexture())
    {
      glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, (GLint*)&max_texture_units_count);
    }    

    ///////////////////////////////////////////
    /// Константы для проверки поддержки типов и форматов данных
    ///////////////////////////////////////////
    
    position_types_supported.push_back(InputDataType_Short);
    position_types_supported.push_back(InputDataType_Int);
    position_types_supported.push_back(InputDataType_Float);
    
    position_formats_supported.push_back(InputDataFormat_Vector2);
    position_formats_supported.push_back(InputDataFormat_Vector3);
    position_formats_supported.push_back(InputDataFormat_Vector4);
    
    normal_types_supported.push_back(InputDataType_Byte);
    normal_types_supported.push_back(InputDataType_Short);
    normal_types_supported.push_back(InputDataType_Int);
    normal_types_supported.push_back(InputDataType_Float);
    
    normal_formats_supported.push_back(InputDataFormat_Vector1);
    normal_formats_supported.push_back(InputDataFormat_Vector2);
    normal_formats_supported.push_back(InputDataFormat_Vector3);
    normal_formats_supported.push_back(InputDataFormat_Vector4);

    color_types_supported.push_back(InputDataType_Byte);
    color_types_supported.push_back(InputDataType_UByte);
    color_types_supported.push_back(InputDataType_Short);
    color_types_supported.push_back(InputDataType_UShort);
    color_types_supported.push_back(InputDataType_Int);
    color_types_supported.push_back(InputDataType_UInt);
    color_types_supported.push_back(InputDataType_Float);

    color_formats_supported.push_back(InputDataFormat_Vector3);
    color_formats_supported.push_back(InputDataFormat_Vector4);

    texcoord_types_supported.push_back(InputDataType_Short);
    texcoord_types_supported.push_back(InputDataType_Int);
    texcoord_types_supported.push_back(InputDataType_Float);
    
    texcoord_formats_supported.push_back(InputDataFormat_Vector1);
    texcoord_formats_supported.push_back(InputDataFormat_Vector2);
    texcoord_formats_supported.push_back(InputDataFormat_Vector3);
    texcoord_formats_supported.push_back(InputDataFormat_Vector4);
  }
  
  //////////////////////////////////////////
  /// создание ресурсов
  //////////////////////////////////////////

  IInputLayout* CreateInputLayoutState(const InputLayoutDesc& desc)
  {
    try
    {
      return new InputLayoutState(desc);
    }
    catch (common::Exception& exception)
    {
      exception.Touch("render::low_level::opengl::InputStage::Impl::CreateInputLayoutState (const InputLayoutDesc& desc)");
      throw;
    }
  }

  IBuffer* CreateVertexBuffer (const BufferDesc& desc)
  {
    try
    {
      MakeContextCurrent();

      static Extension  ARB_vertex_buffer_object  = "GL_ARB_vertex_buffer_object",
                        GL_version_1_5            = "GL_VERSION_1_5";
    
      bool has_ARB_vertex_buffer_object =    GetContextManager().IsSupported(ARB_vertex_buffer_object)
                                          || GetContextManager().IsSupported(GL_version_1_5);
     
      if (!(desc.bind_flags & BindFlag_VertexBuffer))
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::CreateVertexBuffer (const BufferDesc& desc)",
                             "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                             "Buffer descriptor must include VertexBuffer binding support");
    
      if (has_ARB_vertex_buffer_object)
        return new VboBuffer(GetContextManager(), GL_ARRAY_BUFFER, desc);
      else
        return new SystemMemoryBuffer(GetContextManager(), desc);
    }
    catch (common::Exception& exception)
    {
      exception.Touch("render::low_level::opengl::InputStage::Impl::CreateVertexBuffer (const BufferDesc& desc)");
      throw;
    }
  }

  IBuffer* CreateIndexBuffer (const BufferDesc& desc)
  {
    try
    {
      MakeContextCurrent();

      static Extension  ARB_vertex_buffer_object  = "GL_ARB_vertex_buffer_object",
                        GL_version_1_5            = "GL_VERSION_1_5";
    
      bool has_ARB_vertex_buffer_object =    GetContextManager().IsSupported(ARB_vertex_buffer_object)
                                          || GetContextManager().IsSupported(GL_version_1_5);
      if (!(desc.bind_flags & BindFlag_IndexBuffer))
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::CreateIndexBuffer (const BufferDesc& desc)",
                             "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                             "Buffer descriptor must include IndexBuffer binding support");

      if (has_ARB_vertex_buffer_object)
        return new VboBuffer(GetContextManager(), GL_ELEMENT_ARRAY_BUFFER, desc);
      else
        return new SystemMemoryBuffer(GetContextManager(), desc);
    }
    catch (common::Exception& exception)
    {
      exception.Touch("render::low_level::opengl::InputStage::Impl::CreateIndexBuffer (const BufferDesc& desc)");
      throw;
    } 
  }

  //////////////////////////////////////////
  /// операции над InputLayoutState
  //////////////////////////////////////////
  
  IInputLayout* GetInputLayoutState()
  {
    return input_state;
  }
  
  void SetInputLayoutState (IInputLayout* state)
  {
    RaiseNotImplemented ("render::low_level::opengl::InputStage::SetInputLayoutState");
/*    if (!state)
    {
      input_state = NULL;
      return;
      //RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetInputLayoutState (IInputLayoutState* state)", "state");
    }
    
    input_state = state;
    
    InputLayoutDesc desc;
    input_state->GetDesc(desc);
    
    for (int i = 0; i < VertexAttributeSemantic_Num; i++)
      attribute_table[i] = NULL;
    
    for (size_t i = 0; i < desc.vertex_attributes_count; i++)
      attribute_table[desc.vertex_attributes[i].semantic] = &desc.vertex_attributes[i];
    
    MakeContextCurrent();

    for (int i = 0; i < VertexAttributeSemantic_Num; i++)
      SetSemanticClientState(static_cast<VertexAttributeSemantic>(i), attribute_table[i]!=NULL?true:false);*/
  }
  
  //////////////////////////////////////////
  /// Операции с вершинными буферами
  //////////////////////////////////////////
  
  IBuffer* GetVertexBuffer(size_t vertex_buffer_slot)
  {
    if (vertex_buffer_slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
      RaiseOutOfRange("render::low_level::opengl::InputStage::Impl::GetVertexBuffer (size_t vertex_buffer_slot)",
                      "vertex_buffer_slot", vertex_buffer_slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);
  
    return vertex_buffer_slots[vertex_buffer_slot];
  }
  
  void SetVertexBuffer(size_t vertex_buffer_slot, IBuffer* buffer)
  {
    if (vertex_buffer_slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
      RaiseOutOfRange("render::low_level::opengl::InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)",
                      "vertex_buffer_slot", vertex_buffer_slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);
                      
    if (!buffer)
    {
      vertex_buffer_slots[vertex_buffer_slot] = NULL;
      return;
    //  RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)",
    //                    "buffer");
    }
    
    BufferDesc desc;
    buffer->GetDesc(desc);
    if (!(desc.bind_flags & BindFlag_VertexBuffer))
      RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::CreateVertexBuffer (const BufferDesc& desc)",
                           "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                           "Buffer descriptor must include VertexBuffer binding support");
  
    vertex_buffer_slots[vertex_buffer_slot] = cast_object<Buffer>(GetContextManager(), buffer,
                                                                  "render::low_level::opengl::InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)",
                                                                  "buffer");
  }
  
  //////////////////////////////////////////
  /// Операции с индексным буфером
  //////////////////////////////////////////

  void SetIndexBuffer(IBuffer* buffer)
  {
    if (!buffer)
    {
      index_buffer = NULL;
      return;
      //RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetIndexBuffer (IBuffer* buffer)", "buffer");
    }
      
    BufferDesc desc;
    buffer->GetDesc(desc);
    if (!(desc.bind_flags & BindFlag_IndexBuffer))
      RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::CreateIndexBuffer (const BufferDesc& desc)",
                           "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                           "Buffer descriptor must include IndexBuffer binding support");
    
    index_buffer = cast_object<Buffer>(GetContextManager(), buffer,
                                       "render::low_level::opengl::InputStage::SetIndexBuffer (IBuffer* buffer)",
                                       "buffer");
  }
  
  IBuffer* GetIndexBuffer()
  {
    return index_buffer;
  }
  
  //////////////////////////////////////////
  /// Операции с индексами
  //////////////////////////////////////////
  
  const void* GetIndices()
  {
    if (!index_buffer)
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::GetIndices()", "IndexBuffer is not set, use SetIndexBuffer(size_t, IBuffer*) first!");

    return static_cast<char*>(index_buffer->GetDataPointer()) + full_index_offset;
    //RaiseNotImplemented ("render::low_level::opengl::InputStage::Impl::GetIndices()");
    //return 0;
  }

  GLenum GetIndexType()
  {
    RaiseNotImplemented ("render::low_level::opengl::InputStage::GetIndexType"); return 0;
/*    if (!input_state)
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::GetIndexType()", "InputLayoutState is not set, use SetInputLayoutState (IInputLayoutState* state) first!");
    
    InputLayoutDesc idesc;
    input_state->GetDesc(idesc);
    return idesc.index_type;
*/      
    //RaiseNotImplemented ("render::low_level::opengl::InputStage::Impl::GetIndexType()");
    //return 0;
  }
    
  void Bind(size_t base_vertex, size_t base_index)
  {
    RaiseNotImplemented ("render::low_level::opengl::InputStage::Impl::Bind");
/*    if (!input_state)
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::Bind()", "InputLayoutState is not set, use SetInputLayoutState (IInputLayoutState* state) first!");
    
    ///////////////////////////////////////////
    /// Биндинг индексного буфера, по сути настройка
    /// смещения для GetIndices()
    ///////////////////////////////////////////
    InputLayoutDesc desc;
    input_state->GetDesc(desc);
    
    size_t index_size;
    switch (desc.index_type)
    {
      case InputDataType_UByte:
        index_size = sizeof (unsigned char);
        break;
      case InputDataType_UShort:
        index_size = sizeof (unsigned short);
        break;
      case InputDataType_UInt:
        index_size = sizeof (unsigned int);
        break;
      default:
        RaiseInvalidArgument( "render::low_level::opengl::InputStage::Impl::Bind()",
                              "InputLayoutDesc::index_type",
                              get_name((InputDataType)desc.index_type),
                              "Index Data type must be a 1, 2 or 4 byte unsigned number");
    }
    
    full_index_offset = desc.index_buffer_offset + base_index*index_size;

    ///////////////////////////////////////////
    /// Биндинг вершинных буферов
    ///////////////////////////////////////////

    MakeContextCurrent();
    
    for (int i = VertexAttributeSemantic_Num - 1; i >=0; i--)
    {
      if (attribute_table[i])
      {
        if (!(vertex_buffer_slots[ attribute_table[i]->slot ]))
          RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::Bind()",
                                "VertexBuffer in slot %d is not set! Check slot index in VertexAttributes list or set buffer with SetVertexBuffer(...)!",
                                attribute_table[i]->slot);
        
        Buffer* current_buffer = vertex_buffer_slots[ attribute_table[i]->slot ];
        
        current_buffer->Bind();
        BindBufferBySemantics(*(attribute_table[i]), base_vertex, current_buffer->GetDataPointer());
      }
    }  */
  }
    
private:
  IInputLayout*  input_state;
  Buffer*        vertex_buffer_slots[DEVICE_VERTEX_BUFFER_SLOTS_COUNT];
  Buffer*        index_buffer;
  size_t         max_texture_units_count;
  size_t         full_index_offset;
  
  VertexAttribute*    attribute_table[VertexAttributeSemantic_Num];

  stl::vector<InputDataType>   position_types_supported,
                               normal_types_supported,
                               color_types_supported,
                               texcoord_types_supported;
  stl::vector<InputDataFormat> position_formats_supported,
                               normal_formats_supported,
                               color_formats_supported,
                               texcoord_formats_supported;

  void BindBufferBySemantics (VertexAttribute attribute,
                              size_t base_vertex,
                              const void* base_data)
  {    
    GLint size;
    GLenum type;
    
    size_t type_size;
    size_t texunit = 0;
    
    ///////////////////////////////////////////
    /// Преобразование формата буфера
    ///////////////////////////////////////////
    switch (attribute.format)
    {
      case InputDataFormat_Vector1:
        size = 1;
        type_size = 1;
        break;
      case InputDataFormat_Vector2:
        size = 2;
        type_size = 2;
        break;
      case InputDataFormat_Vector3:
        size = 3;
        type_size = 3;
        break;
      case InputDataFormat_Vector4:
        size = 4;
        type_size = 4;
        break;
      default:
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                             "attribute.format", attribute.format, "Unknown InputDataFormat value supplied!"); 
    }
    ///////////////////////////////////////////
    /// Преобразование типа данных буфера
    ///////////////////////////////////////////
    switch (attribute.type)
    {
      case InputDataType_Byte:
        type = GL_BYTE;
        type_size *= sizeof(char);
        break;
      case InputDataType_UByte:
        type = GL_UNSIGNED_BYTE;
        type_size *= sizeof(char);
        break;
      case InputDataType_Short:
        type = GL_SHORT;
        type_size *= sizeof(short);
        break;
      case InputDataType_UShort:
        type = GL_UNSIGNED_SHORT;
        type_size *= sizeof(short);
        break;
      case InputDataType_Int:
        type = GL_INT;
        type_size *= sizeof(int);
        break;
      case InputDataType_UInt:
        type = GL_UNSIGNED_INT;
        type_size *= sizeof(int);
        break;
      case InputDataType_Float:
        type = GL_FLOAT;
        type_size *= sizeof(float);
        break;
      default:
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                             "attribute.type", attribute.type, "Unknown InputDataType value supplied!");         
    }
    ///////////////////////////////////////////
    /// Расчёт начального адреса данных
    ///////////////////////////////////////////
    const void* data = ( (const char*)base_data ) + attribute.offset + base_vertex * (type_size + attribute.stride);
    ///////////////////////////////////////////
    /// Перебор семантик
    ///////////////////////////////////////////
    switch (attribute.semantic)
    {
      ///////////////////////////////////////////
      /// Координаты
      ///////////////////////////////////////////
      case VertexAttributeSemantic_Position:
        if (!IsParamSupported(attribute.format, position_formats_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.format", get_name(attribute.format),
                               "This InputDataType is not supported by the VertexAttributeSemantic_Position semantic!");
        
        if (!IsParamSupported(attribute.type, position_types_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.type", get_name(attribute.type),
                               "This InputDataType is not supported by the VertexAttributeSemantic_Position semantic!");

        glVertexPointer(size, type, attribute.stride, data);
        break;
      ///////////////////////////////////////////
      /// Нормали
      ///////////////////////////////////////////
      case VertexAttributeSemantic_Normal:
        if (!IsParamSupported(attribute.format, normal_formats_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.format", get_name(attribute.format),
                               "This InputDataType is not supported by the VertexAttributeSemantic_Normal semantic!");
        
        if (!IsParamSupported(attribute.type, normal_types_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.type", get_name(attribute.type),
                               "This InputDataType is not supported by the VertexAttributeSemantic_Normal semantic!");

        glNormalPointer(type, attribute.stride, data);
        break;
      ///////////////////////////////////////////
      /// Цвета
      ///////////////////////////////////////////
      case VertexAttributeSemantic_Color:
        if (!IsParamSupported(attribute.format, color_formats_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.format", get_name(attribute.format),
                               "This InputDataType is not supported by the VertexAttributeSemantic_Color semantic!");
        
        if (!IsParamSupported(attribute.type, color_types_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.type", get_name(attribute.type),
                               "This InputDataType is not supported by the VertexAttributeSemantic_Color semantic!");
        
        glColorPointer(size, type, attribute.stride, data);
        break;
      ///////////////////////////////////////////
      /// Касательные
      ///////////////////////////////////////////
      case VertexAttributeSemantic_Tangent:
        //break;
      ///////////////////////////////////////////
      /// Бинормали
      ///////////////////////////////////////////
      case VertexAttributeSemantic_Binormal:
        RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                          "The semantic %s support is not implemented yet", get_name(attribute.semantic));
        break;
      ///////////////////////////////////////////
      /// Основной буфер текстурных координат
      ///////////////////////////////////////////
      case VertexAttributeSemantic_TexCoord0:
        if (!IsParamSupported(attribute.format, texcoord_formats_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.format", get_name(attribute.format),
                               "This InputDataFormat is not supported by the VertexAttributeSemantic_TexCoord semantics!");
                               
        if (!IsParamSupported(attribute.type, texcoord_types_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.type", get_name(attribute.type),
                               "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord semantics!");
        
        if (has_ARB_multitexture())
          glActiveTexture(GL_TEXTURE0);

        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      ///////////////////////////////////////////
      /// Дополнительные буферы текстурных координат
      ///////////////////////////////////////////
      case VertexAttributeSemantic_TexCoord1:
        //break;
      case VertexAttributeSemantic_TexCoord2:
        //break;
      case VertexAttributeSemantic_TexCoord3:
        //break;
      case VertexAttributeSemantic_TexCoord4:
        //break;
      case VertexAttributeSemantic_TexCoord5:
        //break;
      case VertexAttributeSemantic_TexCoord6:
        //break;
      case VertexAttributeSemantic_TexCoord7:
        if (!has_ARB_multitexture())
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing is not supported");

        texunit = attribute.semantic - VertexAttributeSemantic_TexCoord0;
        if (texunit > max_texture_units_count)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing supports only %d texture units instead of %d requested",
                            max_texture_units_count, texunit);
          
        if (!IsParamSupported(attribute.format, texcoord_formats_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.format", get_name(attribute.format),
                               "This InputDataFormat is not supported by the VertexAttributeSemantic_TexCoord semantics!");
                               
        if (!IsParamSupported(attribute.type, texcoord_types_supported))
          RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                               "attribute.type", get_name(attribute.type),
                               "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord semantics!");
        
        glActiveTexture(GL_TEXTURE0 + texunit);
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      default:
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                             "attribute.semantic", attribute.semantic, "Unknown VertexAttributeSemantic supplied!"); 
    }
  }

  ///////////////////////////////////////////
  /// Проверка поддержки мультитекстурирования
  ///////////////////////////////////////////
  bool has_ARB_multitexture()
  {
    static Extension  ARB_multitexture = "GL_ARB_multitexture",
                      GL_version_1_3   = "GL_VERSION_1_3";
    
    return GetContextManager().IsSupported(ARB_multitexture) || GetContextManager().IsSupported(GL_version_1_3);    
  }
  
  ///////////////////////////////////////////
  /// Проверка поддержки типа данных по списку
  ///////////////////////////////////////////
  template <typename T>
  bool IsParamSupported(T param, const stl::vector<T> &pack)
  {
    for (stl::vector<T>::const_iterator i = pack.begin(); i != pack.end(); i++)
      if (*i == param)
        return true;
    
    return false;
  }  
  
  ///////////////////////////////////////////
  /// Включение/выключение клиентстэйта
  ///////////////////////////////////////////
  void SetClientState(GLenum state, bool enable)
  {
    if (enable)
      glEnableClientState(state);
    else
      glDisableClientState(state);    
  }
  
  void SetSemanticClientState(VertexAttributeSemantic semantic, bool enable)
  {
    switch (semantic)
    {
      case VertexAttributeSemantic_Position:
        SetClientState(GL_VERTEX_ARRAY, enable);
        break;
      case VertexAttributeSemantic_Normal:
        SetClientState(GL_NORMAL_ARRAY, enable);
        break;
      case VertexAttributeSemantic_Color:
        SetClientState(GL_COLOR_ARRAY, enable);
        break;
      case VertexAttributeSemantic_Tangent:
        //SetClientState(, enable);
        break;
      case VertexAttributeSemantic_Binormal:
        //SetClientState(, enable);
        break;
      case VertexAttributeSemantic_TexCoord0:
      case VertexAttributeSemantic_TexCoord1:
      case VertexAttributeSemantic_TexCoord2:
      case VertexAttributeSemantic_TexCoord3:
      case VertexAttributeSemantic_TexCoord4:
      case VertexAttributeSemantic_TexCoord5:
      case VertexAttributeSemantic_TexCoord6:
      case VertexAttributeSemantic_TexCoord7:
        SetClientState(GL_TEXTURE_COORD_ARRAY, enable);
        break;
      default:
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::SetSemanticClientState (VertexAttributeSemantic semantic, bool enable)",
                             "semantic", semantic, "Unknown VertexAttributeSemantic supplied!"); 
    }
  }
};

/*
================================================================================
    Конструкторы
================================================================================
*/

InputStage::InputStage (ContextManager& context_manager)
  : impl (new Impl (context_manager))
{
}

InputStage::~InputStage()
{
}

/*
================================================================================
    Создание ресурсов
================================================================================
*/

IInputLayout* InputStage::CreateInputLayoutState (const InputLayoutDesc& desc)
{
  try
  {
    return impl->CreateInputLayoutState(desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::CreateInputLayoutState (const InputLayoutDesc& desc)");
    throw;
  }
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::CreateInputLayoutState");
  //return 0;
}

IBuffer* InputStage::CreateVertexBuffer (const BufferDesc& desc)
{
  try
  {
    return impl->CreateVertexBuffer(desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::CreateVertexBuffer (const BufferDesc& desc)");
    throw;
  } 
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::CreateVertexBuffer");
  //return 0;
}

IBuffer* InputStage::CreateIndexBuffer (const BufferDesc& desc)
{
  try
  {
    return impl->CreateIndexBuffer(desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::CreateIndexBuffer (const BufferDesc& desc)");
    throw;
  } 
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::CreateIndexBuffer");
  //return 0;
}

/*
================================================================================
    Управление конфигурацией входных данных    
================================================================================
*/

void InputStage::SetInputLayout (IInputLayout* state)
{
  try
  {
    impl->SetInputLayoutState(state);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::SetInputLayout (IInputLayoutState* state)");
    throw;
  }
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::SetInputLayout");
}

IInputLayout* InputStage::GetInputLayout () const
{
  return impl->GetInputLayoutState();
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::GetInputLayout");
  //return 0;
}

/*
================================================================================
    Управление вершинными буферами
================================================================================
*/

void InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)
{
  try
  {
    impl->SetVertexBuffer(vertex_buffer_slot, buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)");
    throw;
  }
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::SetVertexBuffer");
}

IBuffer* InputStage::GetVertexBuffer (size_t vertex_buffer_slot) const
{
  try
  {
    return impl->GetVertexBuffer(vertex_buffer_slot);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::GetVertexBuffer (size_t vertex_buffer_slot)");
    throw;
  }
  
  return 0;
}

/*
================================================================================
    Управление индексным буфером
================================================================================
*/

void InputStage::SetIndexBuffer (IBuffer* buffer)
{
  try
  {
    impl->SetIndexBuffer(buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::SetIndexBuffer (IBuffer* buffer)");
    throw;
  }
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::SetIndexBuffer");
}

IBuffer* InputStage::GetIndexBuffer () const
{
  return impl->GetIndexBuffer();
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::GetIndexBuffer");
  //return 0;
}

/*
================================================================================
    Получение адреса начала массива индексов и типа индексов
================================================================================
*/

const void* InputStage::GetIndices () const
{
  
  try
  {
    return impl->GetIndices();
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::GetIndices()");
    throw;
  }
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::GetIndices");
  return 0;
}

GLenum InputStage::GetIndexType () const
{
  try
  {
    return impl->GetIndexType();
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::GetIndexType()");
    throw;
  }
  
  //RaiseNotImplemented ("render::low_level::opengl::InputStage::GetIndexType");
  return 0;
}

/*
================================================================================
    Установка состояния уровня в контекст OpenGL
================================================================================
*/

void InputStage::Bind (size_t base_vertex, size_t base_index)
{
  try
  {
    impl->Bind(base_vertex, base_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch("render::low_level::opengl::InputStage::Bind (size_t base_vertex, size_t base_index)");
    throw;
  }  
//  RaiseNotImplemented ("render::low_level::opengl::InputStage::Bind (size_t base_vertex, size_t base_index)");
}
