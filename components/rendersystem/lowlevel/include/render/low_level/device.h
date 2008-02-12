#ifndef RENDER_LOW_LEVEL_DEVICE_HEADER
#define RENDER_LOW_LEVEL_DEVICE_HEADER

#include <exception>
#include <render/low_level/buffer.h>
#include <render/low_level/state.h>
#include <render/low_level/view.h>
#include <render/low_level/shader.h>
#include <render/low_level/query.h>

namespace render
{

namespace low_level
{

//forward declarations
class ISwapChain;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t DEVICE_VERTEX_BUFFER_SLOTS_COUNT   = 8; //���������� ������ ��� ���������� ��������� �������
const size_t DEVICE_SAMPLER_SLOTS_COUNT         = 8; //���������� ������ ��� ���������� ���������
const size_t DEVICE_CONSTANT_BUFFER_SLOTS_COUNT = 8; //���������� ������ ��� ���������� ����������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ClearFlag
{
  ClearFlag_RenderTarget = 1, //������� ������ ����� (render-target)
  ClearFlag_Depth        = 2, //������� ����� �������
  ClearFlag_Stencil      = 4, //������� ����� ���������

  ClearFlag_DepthStencil = ClearFlag_Depth | ClearFlag_Stencil,
  ClearFlag_All          = ClearFlag_RenderTarget | ClearFlag_DepthStencil
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PrimitiveType
{
  PrimitiveType_PointList,     //������ �����
  PrimitiveType_LineList,      //������ ��������
  PrimitiveType_LineStrip,     //������� ��������
  PrimitiveType_TriangleList,  //������ �������������
  PrimitiveType_TriangleStrip, //������� �������������
  PrimitiveType_TriangleFan,   //����� �������������

  PrimitiveType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IDevice: virtual public IObject
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* GetName () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual IInputLayout*            CreateInputLayout            (const InputLayoutDesc&) = 0;
    virtual IShaderParametersLayout* CreateShaderParametersLayout (const ShaderParametersLayoutDesc&) = 0;
    virtual IRasterizerState*        CreateRasterizerState        (const RasterizerDesc&) = 0;
    virtual IBlendState*             CreateBlendState             (const BlendDesc&) = 0;
    virtual IDepthStencilState*      CreateDepthStencilState      (const DepthStencilDesc&) = 0;
    virtual ISamplerState*           CreateSamplerState           (const SamplerDesc&) = 0;
    virtual IBuffer*                 CreateBuffer                 (const BufferDesc&) = 0;
    virtual IShader*                 CreateShader                 (const ShaderDesc& desc, const LogFunction& error_log) = 0;
    virtual ITexture*                CreateTexture                (const TextureDesc&) = 0;
    virtual ITexture*                CreateRenderTargetTexture    (ISwapChain* swap_chain, size_t buffer_index) = 0;
    virtual ITexture*                CreateDepthStencilTexture    (ISwapChain* swap_chain) = 0;
    virtual IView*                   CreateView                   (ITexture* texture, const ViewDesc& desc) = 0;
    virtual IPredicate*              CreatePredicate              () = 0;
    virtual IStatisticsQuery*        CreateStatisticsQuery        () = 0;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� (input-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void          ISSetInputLayout  (IInputLayout* state) = 0;
    virtual void          ISSetVertexBuffer (size_t vertex_buffer_slot,  IBuffer* buffer) = 0;
    virtual void          ISSetIndexBuffer  (IBuffer* buffer) = 0;
    virtual IInputLayout* ISGetInputLayout  () = 0;
    virtual IBuffer*      ISGetVertexBuffer (size_t vertex_buffer_slot) = 0;
    virtual IBuffer*      ISGetIndexBuffer  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �������� (shader-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                     SSSetShader                 (IShader* shader) = 0;
    virtual void                     SSSetShaderParametersLayout (IShaderParametersLayout* parameters_layout) = 0;
    virtual void                     SSSetSampler                (size_t sampler_slot, ISamplerState* state) = 0;
    virtual void                     SSSetTexture                (size_t sampler_slot, ITexture* texture) = 0;
    virtual void                     SSSetConstantBuffer         (size_t buffer_slot, IBuffer* buffer) = 0;
    virtual IShaderParametersLayout* SSGetShaderParametersLayout () = 0;
    virtual IShader*                 SSGetShader                 () = 0;
    virtual ISamplerState*           SSGetSampler                (size_t sampler_slot) = 0;
    virtual ITexture*                SSGetTexture                (size_t sampler_slot) = 0;
    virtual IBuffer*                 SSGetConstantBuffer         (size_t buffer_slot) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� (rasterizer-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void              RSSetState    (IRasterizerState* state) = 0;
    virtual void              RSSetViewport (const Viewport& viewport) = 0;
    virtual void              RSSetScissor  (const Rect& scissor_rect) = 0;
    virtual IRasterizerState* RSGetState    () = 0;
    virtual const Viewport&   RSGetViewport () = 0;
    virtual const Rect&       RSGetScissor  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ������� (output-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void                OSSetBlendState        (IBlendState* state) = 0;
    virtual void                OSSetDepthStencilState (IDepthStencilState* state) = 0;
    virtual void                OSSetStencilReference  (size_t reference) = 0;
    virtual void                OSSetRenderTargets     (IView* render_target_view, IView* depth_stencil_view) = 0;
    virtual IBlendState*        OSGetBlendState        () = 0;
    virtual IDepthStencilState* OSGetDepthStencilState () = 0;
    virtual size_t              OSGetStencilReference  () = 0;
    virtual IView*              OSGetRenderTargetView  () = 0;
    virtual IView*              OSGetDepthStencilView  () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void ClearRenderTargetView (const Color4f& color) = 0;
    virtual void ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil) = 0;
    virtual void ClearViews            (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void        SetPredication    (IPredicate* predicate, bool predicate_value) = 0;
    virtual IPredicate* GetPredicate      () = 0;
    virtual bool        GetPredicateValue () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Draw        (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count) = 0;
    virtual void DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Flush () = 0;
};

}

}

#endif
