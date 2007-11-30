#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ����������� / ����������
*/

Device::Device (ISwapChain* swap_chain, const char*)
  : current_state (swap_chain)
{  
  ContextLock lock (current_state);
  
    //��������� ���������� �� ���������� ���������
    
  properties.AddProperty ("vendor",     reinterpret_cast<const char*> (glGetString (GL_VENDOR)));
  properties.AddProperty ("renderer",   reinterpret_cast<const char*> (glGetString (GL_RENDERER)));
  properties.AddProperty ("gl_version", reinterpret_cast<const char*> (glGetString (GL_VERSION)));
  
    //��������� ������ ����������
    
  const char* swap_chain_extension_string = current_state.GetContext ().GetSwapChainExtensionString ();
  
  stl::string extension_string = reinterpret_cast<const char*> (glGetString (GL_EXTENSIONS));
  
  if (*swap_chain_extension_string)
  {
    extension_string += ' ';
    extension_string += swap_chain_extension_string;
  }
  
  properties.AddProperty ("gl_extensions", extension_string.c_str ());

    //�������� ��������� ��������� OpenGL

  current_state.Check ("render::low_level::opengl::Device::Device");
}

Device::~Device ()
{
}

/*
    ��� ����������
*/

const char* Device::GetName ()
{
  return "render::low_level::opengl::Device";
}

/*
    �������� ��������
*/

IInputLayoutState* Device::CreateInputLayoutState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateInputLayoutState");
  return 0;
}

ILightingState* Device::CreateLightingState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateLightingState");
  return 0;
}

IViewerState* Device::CreateViewerState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateViewerState");
  return 0;
}

ITransformState* Device::CreateTransformState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateTransformState");
  return 0;
}

IMaterialState* Device::CreateMaterialState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateMaterialState");
  return 0;
}

IRasterizerState* Device::CreateRasterizerState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateRasterizerState");
  return 0;
}

IBlendState* Device::CreateBlendState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateBlendState");
  return 0;
}

IDepthStencilState* Device::CreateDepthStencilState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateDepthStencilState");
  return 0;
}

ISamplerState* Device::CreateSamplerState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateSamplerState");
  return 0;
}

IVertexBuffer* Device::CreateVertexBuffer (const BufferDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateVertexBuffer");
  return 0;
}

IIndexBuffer* Device::CreateIndexBuffer (const BufferDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateIndexBuffer");
  return 0;
}

ITexture* Device::CreateTexture (const TextureDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateTexture");
  return 0;
}

IFrameBuffer* Device::CreateFrameBuffer (const FrameBufferDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateFrameBuffer(const FrameBufferDesc&)");
  return 0;
}

IFrameBuffer* Device::CreateFrameBuffer (ISwapChain*)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateFrameBuffer(ISwapChain*)");
  return 0;
}

IFrameBuffer* Device::CreateFrameBuffer (ITexture* render_target)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateFrameBuffer(ITexture*)");
  return 0;
}

IPredicate* Device::CreatePredicate ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreatePredicate");
  return 0;
}

IStatisticsQuery* Device::CreateStatisticsQuery ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateStatisticsQuery");
  return 0;
}

/*
    ���������� ������� ������� (input-stage)
*/

void Device::ISSetInputLayout (IInputLayoutState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ISSetInputLayout");  
}

void Device::ISSetVertexBuffer (size_t vertex_buffer_slot, IVertexBuffer* buffer)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ISSetVertexBuffer");
}

void Device::ISSetIndexBuffer (IIndexBuffer* buffer)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ISSetIndexBuffer");
}

IInputLayoutState* Device::ISGetInputLayout ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ISGetInputLayout");
  return 0;
}

IVertexBuffer* Device::ISGetVertexBuffer (size_t vertex_buffer_slot)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ISGetVertexBuffer");
  return 0;
}

IIndexBuffer* Device::ISGetIndexBuffer ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ISGetIndexBuffer");
  return 0;
}

/*
    ���������� ���������� �������� (shader-stage)
*/

void Device::SSSetMode (ShaderMode mode)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetMode");
}

void Device::SSSetViewer (IViewerState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetViewer");
}

void Device::SSSetTransform (ITransformState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetTransform");
}

void Device::SSSetLighting (ILightingState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetLighting");
}

void Device::SSSetSampler (size_t sampler_slot, ISamplerState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetSampler");
}

void Device::SSSetTexture (size_t sampler_slot, ITexture* texture)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetTexture");
}

void Device::SSSetMaterial (IMaterialState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetMaterial");
}

ShaderMode Device::SSGetMode ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetMode");
  return ShaderMode_Flat;
}

IViewerState* Device::SSGetViewer ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetViewer");
  return 0;
}

ITransformState* Device::SSGetTransform ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetTransform");
  return 0;
}

ILightingState* Device::SSGetLighting ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetLighting");
  return 0;
}

ISamplerState* Device::SSGetSampler (size_t sampler_slot)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetSampler");
  return 0;
}

ITexture* Device::SSGetTexture (size_t sampler_slot)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetTexture");
  return 0;
}

IMaterialState* Device::SSGetMaterial ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetMaterial");
  return 0;
}

/*
    ��������� ���������� � ��������� ������� �������� ����������
*/

bool Device::SSIsSupported (ShaderMode mode)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSIsSupported");
  return false;
}

/*
    ���������� �������������� (rasterizer-stage)
*/

void Device::RSSetState (IRasterizerState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSSetState");
}

void Device::RSSetViewport (const Viewport& viewport)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSSetViewport");
}

void Device::RSSetScissor (const Rect& scissor_rect)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSSetScissor");
}

IRasterizerState* Device::RSGetState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSGetState");
  return 0;
}

const Viewport& Device::RSGetViewport ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSGetViewport");
  
  static Viewport temp_viewport;
  
  return temp_viewport;
}

const Rect& Device::RSGetScissor ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSGetScissor");
  
  static Rect temp_scissor;
  
  return temp_scissor;
  
}

/*
    ���������� �������� ������� (output-stage)
*/

void Device::OSSetBlendState (IBlendState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::OSSetBlendState");
}

void Device::OSSetDepthStencil (IDepthStencilState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::OSSetDepthStencil");
}

void Device::OSSetStencilReference (size_t reference)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::OSSetStencilReference");
}

void Device::OSSetFrameBuffer (IFrameBuffer* frame_buffer)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::OSSetFrameBuffer");
}

IBlendState* Device::OSGetBlendState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::OSGetBlendState");
  return 0;
}

IDepthStencilState* Device::OSGetDepthStencilState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::OSGetDepthStencilState");
  return 0;
}

size_t Device::OSGetStencilReference ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::OSGetStencilReference");
  return 0;
}

IFrameBuffer* Device::OSGetFrameBuffer ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::OSGetFrameBuffer");
  return 0;
}

/*
    �������
*/

void Device::ClearRenderTargetSurface (IRenderTargetSurface* surface, const Color4f& color)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ClearRenderTargetSurface");
}

void Device::ClearDepthSurface (IDepthStencilSurface* surface, float depth)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ClearDepthSurface");
}

void Device::ClearStencilSurface (IDepthStencilSurface* surface, unsigned char value)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::ClearStencilSurface");
}

void Device::Clear (IFrameBuffer* buffer, size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::Clear");
}

void Device::Clear (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::Clear");
}

/*
    ���������� ����������� ���������
*/

void Device::SetPredication (IPredicate* predicate, bool predicate_value)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SetPredication");
}

IPredicate* Device::GetPredicate ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::GetPredicate");
  return 0;
}

bool Device::GetPredicateValue ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::GetPredicateValue");
  return false;
}

/*
    ��������� ����������
*/

void Device::Draw (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::Draw");
}

void Device::DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::DrawIndexed");
}

/*
    �������� ���������� ���������� ������ ������
*/

void Device::Flush ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::Flush");
}

/*
    ������ ������� ���������� ���������
*/

IPropertyList* Device::GetProperties ()
{
  return &properties;
}
