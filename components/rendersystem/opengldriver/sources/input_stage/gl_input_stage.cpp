#include "shared.h"
#include <shared/object.h>
#include <render/low_level/utils.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;


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
    InputLayoutDesc input_desc;
};


/*
================================================================================
    Описание реализации входного уровня конвейера OpenGL
================================================================================
*/

const size_t MAX_VERTEX_BUFFER_SLOTS = 8;

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
     
      if (!(desc.bind_flags & BindFlag_VertexBuffer))
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::CreateVertexBuffer (const BufferDesc& desc)",
                             "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                             "Buffer descriptor must include VertexBuffer binding support");
    
      if (GLEW_ARB_vertex_buffer_object)
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

      if (!(desc.bind_flags & BindFlag_IndexBuffer))
        RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::CreateIndexBuffer (const BufferDesc& desc)",
                             "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                             "Buffer descriptor must include IndexBuffer binding support");

      if (GLEW_ARB_vertex_buffer_object)
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
      RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetInputLayoutState (IInputLayoutState* state)", "state");
    
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
    if (!buffer)
      RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)",
                        "buffer");
    
    BufferDesc desc;
    buffer->GetDesc(desc);
    if (!(desc.bind_flags & BindFlag_VertexBuffer))
      RaiseInvalidArgument("render::low_level::opengl::InputStage::Impl::CreateVertexBuffer (const BufferDesc& desc)",
                           "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                           "Buffer descriptor must include VertexBuffer binding support");

    if (vertex_buffer_slot >= MAX_VERTEX_BUFFER_SLOTS)
      RaiseOutOfRange("render::low_level::opengl::InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)",
                      "vertex_buffer_slot", vertex_buffer_slot, MAX_VERTEX_BUFFER_SLOTS);
  
    vertex_buffer_slots[vertex_buffer_slot] = buffer;
  }
  
  //////////////////////////////////////////
  /// Операции с индексным буфером
  //////////////////////////////////////////

  void SetIndexBuffer(IBuffer* buffer)
  {
    if (!buffer)
      RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetIndexBuffer (IBuffer* buffer)", "buffer");
      
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
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::GetIndices()", "Index buffer is not set, use SetIndexBuffer(size_t, IBuffer*) first!");
    
    Buffer* buffer = cast_object<Buffer> (GetContextManager(), index_buffer, "render::low_level::opengl::InputStage::Impl::GetIndices()", "index_buffer");
    return buffer->GetDataPointer();
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
  }
    
private:
  IInputLayoutState*  input_state;
  IBuffer*            vertex_buffer_slots[MAX_VERTEX_BUFFER_SLOTS];
  IBuffer*            index_buffer;
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
//  RaiseNotImplemented ("render::low_level::opengl::InputStage::Bind (size_t base_vertex, size_t base_index)");
}
