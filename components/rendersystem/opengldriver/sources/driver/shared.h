#ifndef RENDER_GL_DRIVER_DRIVER_SHARED_HEADER
#define RENDER_GL_DRIVER_DRIVER_SHARED_HEADER

#include <render/low_level/device.h>
#include <render/low_level/opengl_driver.h>

#include <shared/platform/output_manager.h>
#include <shared/platform/swap_chain_manager.h>
#include <shared/object.h>
#include <shared/context_manager.h>
#include <shared/property_list.h>
#include <shared/output_stage.h>

#include <common/exception.h>
#include <common/singleton.h>

#include <stl/string>

#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �ࠩ��� ��� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver  ();
    ~Driver ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///���ᠭ�� �ࠩ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����᫥��� ����㯭�� ���ன�� �뢮��
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   GetOutputsCount ();
    IOutput* GetOutput       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� 楯�窨 ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (const SwapChainDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���ன�⢠ ���ᮢ��
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (ISwapChain* swap_chain, const char* init_string);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��⠭���� �㭪樨 �⫠��筮�� ��⮪���஢���� �ࠩ���
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetDebugLog (const LogFunction&);
    const LogFunction& GetDebugLog ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��⮪���஢����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogMessage (const char* message) const;

  private:
    OutputManager output_manager; //�������� ���ன�� �뢮��
    LogFunction   log_fn;         //�㭪�� �⫠��筮�� ��⮪���஢����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���ன�⢮ ���ᮢ��
///////////////////////////////////////////////////////////////////////////////////////////////////
class Device: virtual public IDevice, public Object
{
  public:  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Device  (Driver* driver, ISwapChain* swap_chain, const char* init_string);
    ~Device ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����⥫� �� ���ன�⢮ ���ᮢ��, ���஬� �ਭ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* GetDevice () { return this; }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���ன�⢠
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����ᮢ
///////////////////////////////////////////////////////////////////////////////////////////////////
    IInputLayoutState*  CreateInputLayoutState  ();
    ILightingState*     CreateLightingState     ();
    IViewerState*       CreateViewerState       ();
    ITransformState*    CreateTransformState    ();
    IMaterialState*     CreateMaterialState     ();
    IRasterizerState*   CreateRasterizerState   ();
    IBlendState*        CreateBlendState        (const BlendDesc&);
    IDepthStencilState* CreateDepthStencilState (const DepthStencilDesc&);
    ISamplerState*      CreateSamplerState      ();
    IVertexBuffer*      CreateVertexBuffer      (const BufferDesc&);
    IIndexBuffer*       CreateIndexBuffer       (const BufferDesc&);
    ITexture*           CreateTexture           (const TextureDesc&);
    IFrameBuffer*       CreateFrameBuffer       (const FrameBufferDesc&);
    IFrameBuffer*       CreateFrameBuffer       (ISwapChain*);
    IFrameBuffer*       CreateFrameBuffer       (ITexture* render_target);
    IPredicate*         CreatePredicate         ();
    IStatisticsQuery*   CreateStatisticsQuery   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��ࠢ����� �室�� �஢��� (input-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               ISSetInputLayout  (IInputLayoutState* state);
    void               ISSetVertexBuffer (size_t vertex_buffer_slot,  IVertexBuffer* buffer);
    void               ISSetIndexBuffer  (IIndexBuffer* buffer);
    IInputLayoutState* ISGetInputLayout  ();
    IVertexBuffer*     ISGetVertexBuffer (size_t vertex_buffer_slot);
    IIndexBuffer*      ISGetIndexBuffer  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��ࠢ����� 襩���묨 �஢�ﬨ (shader-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void             SSSetMode       (ShaderMode mode);
    void             SSSetViewer     (IViewerState* state);
    void             SSSetTransform  (ITransformState* state);
    void             SSSetLighting   (ILightingState* state);
    void             SSSetSampler    (size_t sampler_slot, ISamplerState* state);
    void             SSSetTexture    (size_t sampler_slot, ITexture* texture);
    void             SSSetMaterial   (IMaterialState* state);
    ShaderMode       SSGetMode       ();
    IViewerState*    SSGetViewer     ();
    ITransformState* SSGetTransform  ();
    ILightingState*  SSGetLighting   ();
    ISamplerState*   SSGetSampler    (size_t sampler_slot);
    ITexture*        SSGetTexture    (size_t sampler_slot);
    IMaterialState*  SSGetMaterial   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����祭�� ���ଠ樨 � ����㯭�� ०���� 襩����� ���ன�⢠
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool SSIsSupported (ShaderMode mode);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��ࠢ����� ���ਧ��஬ (rasterizer-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              RSSetState    (IRasterizerState* state);
    void              RSSetViewport (const Viewport& viewport);
    void              RSSetScissor  (const Rect& scissor_rect);
    IRasterizerState* RSGetState    ();
    const Viewport&   RSGetViewport ();
    const Rect&       RSGetScissor  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��ࠢ����� ��室�� �஢��� (output-stage)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                OSSetBlendState        (IBlendState* state);
    void                OSSetDepthStencil      (IDepthStencilState* state);
    void                OSSetStencilReference  (size_t reference);    
    void                OSSetFrameBuffer       (IFrameBuffer* frame_buffer);
    IBlendState*        OSGetBlendState        ();
    IDepthStencilState* OSGetDepthStencilState ();
    size_t              OSGetStencilReference  ();
    IFrameBuffer*       OSGetFrameBuffer       ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���⪠
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ClearRenderTargetSurface (IRenderTargetSurface* surface, const Color4f& color);
    void ClearDepthSurface        (IDepthStencilSurface* surface, float depth);
    void ClearStencilSurface      (IDepthStencilSurface* surface, unsigned char value);
    void Clear                    (IFrameBuffer* buffer, size_t clear_flags, const Color4f& color, float depth, unsigned char stencil);
    void Clear                    (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��ࠢ����� �।���⠬� ���ᮢ��
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPredication    (IPredicate* predicate, bool predicate_value);
    IPredicate* GetPredicate      ();
    bool        GetPredicateValue ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��ᮢ���� �ਬ�⨢��
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw        (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count);
    void DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����襭�� �믮������ ���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();

//////////////////////////////////////////////////////////////////////////////////////////////////
///���᮪ ᢮��� ���ன�⢠ ���ᮢ��
///////////////////////////////////////////////////////////////////////////////////////////////////
    IPropertyList* GetProperties ();
    
  private:
    typedef xtl::com_ptr<Driver> DriverPtr;
    
  private:
    DriverPtr      driver;          //�ࠩ��� OpenGL
    ContextManager context_manager; //�������� ���⥪�⮢ OpenGL
    PropertyList   properties;      //᢮��⢠ ���ன�⢠
    OutputStage    output_stage;    //��室��� �஢���
};

}

}

}

#endif
