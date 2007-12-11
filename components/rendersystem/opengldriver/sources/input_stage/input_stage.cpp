#include <common/exception.h>
#include <shared/input_stage.h>

namespace render
{

namespace low_level
{

namespace opengl
{

/// struct InputStage::Impl

struct InputStage::Impl
{
  Impl();
  Impl(const Impl&);
  ~Impl();
};

InputStage::Impl::Impl()
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::Impl::Impl()");
}

InputStage::Impl::Impl(const Impl&)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::Impl::Impl(const Impl&)");
}

InputStage::Impl::~Impl()
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::Impl::~Impl()");
}

/// class InputStage

InputStage::InputStage(ContextManager&)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::InputStage(ContextManager&)");
}

InputStage::~InputStage()
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::~InputStage()");
}

IInputLayoutState* InputStage::CreateInputLayoutState (const InputLayoutDesc& desc)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::CreateInputLayoutState(const InputLayoutDesc&)");
  return NULL;
}

IVertexBuffer* InputStage::CreateVertexBuffer (const BufferDesc& desc)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::CreateVertexBuffer(const BufferDesc&)");
  return NULL;
}

IIndexBuffer* InputStage::CreateIndexBuffer (const BufferDesc& desc)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::CreateIndexBuffer(const BufferDesc&)");
  return NULL;
}

void InputStage::SetInputLayout (IInputLayoutState* state)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::SetInputLayout(IInputLayoutState*)");
}

IInputLayoutState* InputStage::GetInputLayout () const
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::GetInputLayout()");
  return NULL;
}

void InputStage::SetVertexBuffer (size_t vertex_buffer_slot, IVertexBuffer* buffer)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::SetVertexBuffer(size_t, IVertexBuffer*)");
}

IVertexBuffer* InputStage::GetVertexBuffer (size_t vertex_buffer_slot) const
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::GetVertexBuffer(size_t)");
  return NULL;
}

void SetIndexBuffer (IIndexBuffer* buffer)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::SetIndexBuffer(IIndexBuffer*)");
}

IIndexBuffer* GetIndexBuffer () const
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::GetIndexBuffer()");
  return NULL;
}

InputStage::InputStage (const InputStage&)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::InputStage(const InputStage&)");  
}

InputStage::InputStage& operator = (const InputStage&)
{
  common::RaiseNotImplemented("render::low_level::opengl::InputStage::operator = (const InputStage&)");
  return *this;
}

} ///opengl namespace

} ///low_level namespace

} ///render namespace