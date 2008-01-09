#include "shared.h"
#include <shared/object.h>
#include <render/low_level/utils.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;


class InputLayoutState: public IInputLayoutState, public Object
{
  public:
    InputLayoutState(const InputLayoutDesc& desc): input_desc(desc) { }
    
    void GetDesc (InputLayoutDesc& desc)
    {
      desc = input_desc;
    }
    
    void SetDesc (const InputLayoutDesc& desc)
    {
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

const size_t Max_Vertex_Buffer_Slots = 8;

struct InputStage::Impl: public ContextObject
{
  //////////////////////////////////////////
  /// конструктор
  //////////////////////////////////////////

  Impl (const ContextManager& context_manager) : ContextObject (context_manager)
  {
    for (int i = 0; i < Max_Vertex_Buffer_Slots; i++)
      vertex_buffer_slots[i] = NULL;
    
    input_state   = NULL;
    index_buffer  = NULL;
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
    if (vertex_buffer_slot >= Max_Vertex_Buffer_Slots)
      RaiseOutOfRange("render::low_level::opengl::InputStage::Impl::GetVertexBuffer (size_t vertex_buffer_slot)",
                      "vertex_buffer_slot", vertex_buffer_slot, Max_Vertex_Buffer_Slots);
  
    return vertex_buffer_slots[vertex_buffer_slot];
  }
  
  void SetVertexBuffer(size_t vertex_buffer_slot, IBuffer* buffer)
  {
    if (!buffer)
      RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)",
                        "buffer");
    
    if (vertex_buffer_slot >= Max_Vertex_Buffer_Slots)
      RaiseOutOfRange("render::low_level::opengl::InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)",
                      "vertex_buffer_slot", vertex_buffer_slot, Max_Vertex_Buffer_Slots);
  
    vertex_buffer_slots[vertex_buffer_slot] = buffer;
  }
  
  //////////////////////////////////////////
  /// Операции с индексным буфером
  //////////////////////////////////////////

  void SetIndexBuffer(IBuffer* buffer)
  {
    if (!buffer)
      RaiseNullArgument("render::low_level::opengl::InputStage::Impl::SetIndexBuffer (IBuffer* buffer)", "buffer");
    
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
    /*
    if (!index_buffer)
      RaiseInvalidOperation("render::low_level::opengl::InputStage::Impl::GetIndices()", "Index buffer is not set, use SetIndexBuffer(size_t, IBuffer*) first!");
      
    return index_buffer->
    */
    RaiseNotImplemented ("render::low_level::opengl::InputStage::Impl::GetIndices()");
    return 0;
  }

  GLenum GetIndexType()
  {
    RaiseNotImplemented ("render::low_level::opengl::InputStage::Impl::GetIndexType()");
    return 0;
  }
  
  //////////////////////////////////////////
  /// Заглушка для контекста
  //////////////////////////////////////////  
  
  void MakeContextCurrent()
  {
    ContextObject::MakeContextCurrent();
  }
  
private:
  IInputLayoutState*  input_state;
  IBuffer*            vertex_buffer_slots[Max_Vertex_Buffer_Slots];
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
    return new InputLayoutState(desc);
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
    impl->MakeContextCurrent();
    
    if (!(desc.bind_flags & BindFlag_VertexBuffer))
      RaiseInvalidArgument("render::low_level::opengl::InputStage::CreateVertexBuffer (const BufferDesc& desc)",
                           "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                           "Buffer descriptor must include VertexBuffer binding support");
    
    if (GLEW_ARB_vertex_buffer_object)
      return new VboBuffer(impl->GetContextManager(), GL_ARRAY_BUFFER, desc);
    else
      return new SystemMemoryBuffer(desc);
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
    impl->MakeContextCurrent();

    if (!(desc.bind_flags & BindFlag_IndexBuffer))
      RaiseInvalidArgument("render::low_level::opengl::InputStage::CreateVertexBuffer (const BufferDesc& desc)",
                           "desc.bind_flags", get_name((BindFlag)desc.bind_flags),
                           "Buffer descriptor must include IndexBuffer binding support");

    if (GLEW_ARB_vertex_buffer_object)
      return new VboBuffer(impl->GetContextManager(), GL_ELEMENT_ARRAY_BUFFER, desc);
    else
      return new SystemMemoryBuffer(desc);
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
  RaiseNotImplemented ("render::low_level::opengl::InputStage::Bind (size_t base_vertex, size_t base_index)");
}
