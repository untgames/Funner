#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Описание реализации входного уровня конвейера OpenGL
*/

struct InputStage::Impl: public ContextObject
{
    //конструктор
  Impl (const ContextManager& context_manager) : ContextObject (context_manager) {}
};

/*
    Конструкторы
*/

InputStage::InputStage (ContextManager& context_manager)
  : impl (new Impl (context_manager))
{
}

InputStage::~InputStage()
{
}

/*
    Создание ресурсов
*/

IInputLayoutState* InputStage::CreateInputLayoutState (const InputLayoutDesc& desc)
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::CreateInputLayoutState");
  return 0;
}

IBuffer* InputStage::CreateVertexBuffer (const BufferDesc& desc)
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::CreateVertexBuffer");
  return 0;
}

IBuffer* InputStage::CreateIndexBuffer (const BufferDesc& desc)
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::CreateIndexBuffer");
  return 0;
}

/*
    Управление конфигурацией входных данных    
*/

void InputStage::SetInputLayout (IInputLayoutState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::SetInputLayout");
}

IInputLayoutState* InputStage::GetInputLayout () const
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::GetInputLayout");
  return 0;
}

/*
    Управление вершинными буферами
*/

void InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::SetVertexBuffer");
}

IBuffer* InputStage::GetVertexBuffer (size_t vertex_buffer_slot) const
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::GetVertexBuffer");
  return 0;
}

/*
    Управление индексным буфером
*/

void InputStage::SetIndexBuffer (IBuffer* buffer)
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::SetIndexBuffer");
}

IBuffer* InputStage::GetIndexBuffer () const
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::GetIndexBuffer");
  return 0;
}

/*
    Получение адреса начала массива индексов и типа индексов
*/

const void* InputStage::GetIndices () const
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::GetIndices");
  return 0;
}

GLenum InputStage::GetIndexType () const
{
  RaiseNotImplemented ("render::low_level::opengl::InputStage::GetIndexType");
  return 0;
}

/*
    Установка состояния уровня в контекст OpenGL
*/

void InputStage::Bind (size_t base_vertex, size_t base_index)
{
}
