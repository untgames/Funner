#include "shared.h"
#include <shared/object.h>
#include <render/low_level/utils.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

const size_t MAX_VERTEX_BUFFER_SLOTS = 8;

class InputLayoutState: public IInputLayoutState, public Object
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
    for (int i = 0; i < MAX_VERTEX_BUFFER_SLOTS; i++)
      vertex_buffer_slots[i] = NULL;
    
    input_state   = NULL;
    index_buffer  = NULL;
  }
  
  //////////////////////////////////////////
  /// создание ресурсов
  //////////////////////////////////////////

  IInputLayoutState* CreateInputLayoutState(const InputLayoutDesc& desc)
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
  
  IInputLayoutState* GetInputLayoutState()
  {
    return input_state;
  }
  
  void SetInputLayoutState (IInputLayoutState* state)
  {
    if (!state)
    {
      input_state = NULL;
      return;
      //RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetInputLayoutState (IInputLayoutState* state)", "state");
    }
    
    input_state = state;
  }
  
  //////////////////////////////////////////
  /// Операции с вершинными буферами
  //////////////////////////////////////////
  
  IBuffer* GetVertexBuffer(size_t vertex_buffer_slot)
  {
    if (vertex_buffer_slot >= MAX_VERTEX_BUFFER_SLOTS)
      RaiseOutOfRange("render::low_level::opengl::InputStage::Impl::GetVertexBuffer (size_t vertex_buffer_slot)",
                      "vertex_buffer_slot", vertex_buffer_slot, MAX_VERTEX_BUFFER_SLOTS);
  
    return vertex_buffer_slots[vertex_buffer_slot];
  }
  
  void SetVertexBuffer(size_t vertex_buffer_slot, IBuffer* buffer)
  {
    if (vertex_buffer_slot >= MAX_VERTEX_BUFFER_SLOTS)
      RaiseOutOfRange("render::low_level::opengl::InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)",
                      "vertex_buffer_slot", vertex_buffer_slot, MAX_VERTEX_BUFFER_SLOTS);
                      
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

  
    vertex_buffer_slots[vertex_buffer_slot] = buffer;
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
    
    index_buffer = buffer;
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

    if (!input_state)
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::GetIndices()", "InputLayoutState is not set, use SetInputLayoutState (IInputLayoutState* state) first!");
    
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
        RaiseInvalidArgument( "render::low_level::opengl::InputStage::Impl::GetIndices()",
                              "InputLayoutDesc::index_type",
                              get_name((InputDataType)desc.index_type),
                              "Index Data type must be a 1, 2 or 4 byte unsigned number");
        break;
    }
        
    Buffer* buffer = cast_object<Buffer> (GetContextManager(), index_buffer, "render::low_level::opengl::InputStage::Impl::GetIndices()", "index_buffer");

    return ((char*)buffer->GetDataPointer()) + index_size * desc.index_buffer_offset;
    //RaiseNotImplemented ("render::low_level::opengl::InputStage::Impl::GetIndices()");
    //return 0;
  }

  GLenum GetIndexType()
  {
    if (!input_state)
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::GetIndexType()", "InputLayoutState is not set, use SetInputLayoutState (IInputLayoutState* state) first!");
    
    InputLayoutDesc idesc;
    input_state->GetDesc(idesc);
    return idesc.index_type;
      
    //RaiseNotImplemented ("render::low_level::opengl::InputStage::Impl::GetIndexType()");
    //return 0;
  }
    
  void Bind(size_t base_vertex, size_t base_index)
  {
    ///////////////////////////////////////////
    /// Биндинг индексного буфера
    ///////////////////////////////////////////
    if (!input_state)
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::Bind()", "InputLayoutState is not set, use SetInputLayoutState (IInputLayoutState* state) first!");
    
    InputLayoutDesc desc;
    input_state->GetDesc(desc);
    
    if (!index_buffer)
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::Bind()", "IndexBuffer is not set, use SetIndexBuffer (IBuffer* buffer) first!");
    
    desc.index_buffer_offset = base_index;
    input_state->SetDesc(desc);
    
    ///////////////////////////////////////////
    /// Биндинг вершинных буферов
    ///////////////////////////////////////////

    MakeContextCurrent();
    
    VertexAttribute*  attrib_to_semantics_table[VertexAttributeSemantic_Num];
    
    for (VertexAttribute **ptr = attrib_to_semantics_table, **end = ptr + VertexAttributeSemantic_Num; ptr < end; ptr++)
      *ptr = NULL;
    
    for (int i = 0; i < desc.vertex_attributes_count; i++)
      attrib_to_semantics_table[ desc.vertex_attributes[i].semantic ] = &desc.vertex_attributes[i];
    
    for (int i = VertexAttributeSemantic_Num - 1; i >=0; i--)
    {
      if (attrib_to_semantics_table[i])
      {
        if (vertex_buffer_slots[ attrib_to_semantics_table[i]->slot ])
          RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::Bind()",
                                "VertexBuffer in slot %d is not set! Check slot index in VertexAttributes list or set buffer with SetVertexBuffer(...)!",
                                attrib_to_semantics_table[i]->slot);
        
        Buffer* current_buffer = cast_object<Buffer> (GetContextManager(), vertex_buffer_slots[ attrib_to_semantics_table[i]->slot ],
                                                      "render::low_level::opengl::InputStage::Impl::Bind(size_t base_vertex, size_t base_index)",
                                                      "vertex_buffer_slots[ attrib_to_semantics_table[i]->slot ]");
        
        current_buffer->Bind();
        BindBufferBySemantics(*(attrib_to_semantics_table[i]), base_vertex, current_buffer->GetDataPointer());
      }
    }
  }
    
private:
  IInputLayoutState*  input_state;
  IBuffer*            vertex_buffer_slots[MAX_VERTEX_BUFFER_SLOTS];
  IBuffer*            index_buffer;

  void BindBufferBySemantics (VertexAttribute attribute,
                              size_t base_vertex,
                              const void* base_data)
  {
    GLint size;
    GLenum type;
    
    static Extension  ARB_multitexture = "GL_ARB_multitexture",
                      GL_version_1_3   = "GL_VERSION_1_3";
    size_t            texunits_num = 1;
    
    bool has_ARB_multitexture =    GetContextManager().IsSupported(ARB_multitexture)
                                || GetContextManager().IsSupported(GL_version_1_3);
    
    if (has_ARB_multitexture)
    {
      glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, (GLint*)&texunits_num);
    }    
    
    size_t type_size;
    
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
    
    const void* data = ( (const char*)base_data ) + attribute.offset + base_vertex * (type_size + attribute.stride);

    switch (attribute.semantic)
    {
      case VertexAttributeSemantic_Position:
        switch (attribute.format)
        {
          case InputDataFormat_Vector1:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.format", get_name(attribute.format),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_Position semantic!");
        }
        
        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_Position semantic!");
        }

        glVertexPointer(size, type, attribute.stride, data);
        break;

      case VertexAttributeSemantic_Normal:
        switch (attribute.type)
        {
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_Normal semantic!");
        }

        glNormalPointer(type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_Color:
        switch (attribute.format)
        {
          case InputDataFormat_Vector1:
          case InputDataFormat_Vector2:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.format", get_name(attribute.format),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_Color semantic!");
        }
        
        glColorPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_Tangent:
        //break;
      case VertexAttributeSemantic_Binormal:
        RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                          "The semantic %s support is not implemented yet", get_name(attribute.semantic));
        break;
      case VertexAttributeSemantic_TexCoord0:
        if (has_ARB_multitexture)
          glActiveTexture(GL_TEXTURE0);

        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord0 semantic!");
        }
        
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_TexCoord1:
        if (!has_ARB_multitexture)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing is not supported");

        if (texunits_num < 2)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing supports only %d texture units instead of %d requested", texunits_num, 2);
          
        glActiveTexture(GL_TEXTURE1);
        
        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord0 semantic!");
        }
        
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_TexCoord2:
        if (!has_ARB_multitexture)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing is not supported");

        if (texunits_num < 3)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing supports only %d texture units instead of %d requested", texunits_num, 3);
          
        glActiveTexture(GL_TEXTURE2);
        
        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord0 semantic!");
        }
        
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_TexCoord3:
        if (!has_ARB_multitexture)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing is not supported");

        if (texunits_num < 4)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing supports only %d texture units instead of %d requested", texunits_num, 4);
          
        glActiveTexture(GL_TEXTURE3);
        
        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord0 semantic!");
        }
        
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_TexCoord4:
        if (!has_ARB_multitexture)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing is not supported");

        if (texunits_num < 5)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing supports only %d texture units instead of %d requested", texunits_num, 5);
          
        glActiveTexture(GL_TEXTURE4);
        
        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord0 semantic!");
        }
        
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_TexCoord5:
        if (!has_ARB_multitexture)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing is not supported");

        if (texunits_num < 6)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing supports only %d texture units instead of %d requested", texunits_num, 6);
          
        glActiveTexture(GL_TEXTURE5);
        
        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord0 semantic!");
        }
        
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_TexCoord6:
        if (!has_ARB_multitexture)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing is not supported");

        if (texunits_num < 7)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing supports only %d texture units instead of %d requested", texunits_num, 7);
          
        glActiveTexture(GL_TEXTURE6);
        
        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord0 semantic!");
        }
        
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_TexCoord7:
        if (!has_ARB_multitexture)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing is not supported");

        if (texunits_num < 8)
          RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                            "Multitexturing supports only %d texture units instead of %d requested", texunits_num, 8);
          
        glActiveTexture(GL_TEXTURE7);
        
        switch (attribute.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_UShort:
          case InputDataType_UInt:
            RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                                 "attribute.type", get_name(attribute.type),
                                 "This InputDataType is not supported by the VertexAttributeSemantic_TexCoord0 semantic!");
        }
        
        glTexCoordPointer(size, type, attribute.stride, data);
        break;
      case VertexAttributeSemantic_Influence:
        RaiseNotSupported("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                          "The semantic %s support is not implemented yet", get_name(attribute.semantic));
        break;
      default:
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::BindBufferBySemantics (...)",
                             "attribute.semantic", attribute.semantic, "Unknown VertexAttributeSemantic supplied!"); 
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

IInputLayoutState* InputStage::CreateInputLayoutState (const InputLayoutDesc& desc)
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

void InputStage::SetInputLayout (IInputLayoutState* state)
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

IInputLayoutState* InputStage::GetInputLayout () const
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
