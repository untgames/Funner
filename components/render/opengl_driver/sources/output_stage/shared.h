#ifndef RENDER_GL_DRIVER_OUTPUT_STAGE_SHARED_HEADER
#define RENDER_GL_DRIVER_OUTPUT_STAGE_SHARED_HEADER

#include <memory.h>

#include <stl/list>

#include <xtl/intrusive_ptr.h>
#include <xtl/trackable_ptr.h>
#include <xtl/bind.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/common_exceptions.h>

#include <common/strlib.h>
#include <common/hash.h>

#include <render/low_level/utils.h>

#include <shared/output_stage.h>
#include <shared/context_object.h>
#include <shared/texture_manager.h>
#include <shared/platform/swap_chain_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ��������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum OutputStageCache
{
  OutputStageCache_FrameBufferId,         //������������� �������� ������ �����
  OutputStageCache_ReadBufferAttachment,  //��� ������ ������
  OutputStageCache_DrawBufferAttachment,  //��� ������ ���������
  OutputStageCache_ClearColorHash,        //��� ����� ������� ������ �����
  OutputStageCache_ClearDepthHash,        //��� �������� ������� ������ �������
  OutputStageCache_ClearStencilValue,     //�������� ������� ������ ���������
  OutputStageCache_BlendStateHash,        //��� ��������� ��������� ���������� ������
  OutputStageCache_DepthStencilStateHash, //��� ��������� ��������� ������������� ���������
  OutputStageCache_StencilReference,      //��������� �������� ����� ���������
  OutputStageCache_ViewportHash,          //��� ��������� ������� ������
  OutputStageCache_ScissorHash,           //��� ��������� ������� ���������
  OutputStageCache_RasterizerStateHash,   //��� ��������� ��������� ������������
  OutputStageCache_ColorWriteMask,        //����� ������ � ����� �����
  OutputStageCache_DepthWriteEnable,      //�������� �� ������ � ����� �������
  OutputStageCache_StencilWriteMask,      //����� ������ � ����� ���������
  OutputStageCache_ScissorEnable,         //������� �� ���� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RenderTargetType
{
  RenderTargetType_Color,        //����� �����
  RenderTargetType_DepthStencil, //����� �������-��������
  
  RenderTargetType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class View: virtual public IView, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    View (const ContextManager&, ITexture* texture, const ViewDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* GetTexture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� / ������ �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   GetDesc (ViewDesc&);
    size_t GetBindFlags () const { return bind_flags; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ����� ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetTextureTypeName () { return type.name (); }

  private:
    typedef xtl::com_ptr<ITexture> TexturePtr;

  private:
    TexturePtr            texture;    //��������� �� ��������
    const std::type_info& type;       //��� �����������
    ViewDesc              desc;       //���������� �����������
    size_t                bind_flags; //����� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IFrameBuffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    virtual ~IFrameBuffer () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ��������� � ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void InvalidateRenderTargets (const Rect& update_rect) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateRenderTargets     () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������� ������ ����� / ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::function<IFrameBuffer* (View* color_view, View* depth_stencil_view)> FrameBufferCreater;
typedef xtl::function<bool (View* color_view, View* depth_stencil_view)>          FrameBufferChecker;
typedef xtl::function<ITexture* (const TextureDesc&)>                             RenderBufferCreater;
typedef xtl::function<ITexture* (ISwapChain*, size_t)>                            ColorBufferCreater;
typedef xtl::function<ITexture* (ISwapChain*)>                                    DepthStencilBufferCreater;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class FrameBufferManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FrameBufferManager  (const ContextManager&, ISwapChain* default_swap_chain);
    FrameBufferManager  (const FrameBufferManager&);
    ~FrameBufferManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ContextManager& GetContextManager () const;
          ContextManager& GetContextManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* GetDefaultSwapChain () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������ �������� ������� ����� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterCreater       (const FrameBufferChecker& frame_buffer_checker,const FrameBufferCreater& frame_buffer_creater);
    void RegisterCreater       (const RenderBufferCreater&);
    void RegisterCreater       (const ColorBufferCreater&);
    void RegisterCreater       (const DepthStencilBufferCreater&);
    void UnregisterAllCreaters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ����� / ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IFrameBuffer*  CreateFrameBuffer        (View* color_view, View* depth_stencil_view);
    ITexture*      CreateRenderBuffer       (const TextureDesc&);
    ITexture*      CreateColorBuffer        (ISwapChain* swap_chain, size_t buffer_index);
    ITexture*      CreateDepthStencilBuffer (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFrameBuffer (size_t context_id, ISwapChain* draw_swap_chain, GLenum draw_buffer_type, ISwapChain* read_swap_chain,
                         GLenum read_buffer_type);
    void SetFrameBuffer (size_t context_id, ISwapChain* swap_chain, GLenum buffer_type);
    void SetFrameBuffer (size_t fbo_id, size_t cache_id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ������� ����� � ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetFrameBufferActivity     (bool color_buffer_state, bool depth_stencil_buffer_state);
    bool IsActiveColorBuffer        () const;
    bool IsActiveDepthStencilBuffer () const;

  private:
    FrameBufferManager& operator = (const FrameBufferManager&); //no impl

  private:
    struct Impl;
    xtl::com_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ ������� ����� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
void register_swap_chain_manager (FrameBufferManager&);
void register_fbo_manager        (FrameBufferManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderBuffer: virtual public ITexture, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (TextureDesc&);
    const TextureDesc& GetDesc () const { return desc; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetType GetTargetType () const { return target_type; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderBuffer (const ContextManager& context_manager, RenderTargetType target_type);
    RenderBuffer (const ContextManager& context_manager, const TextureDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetSize (size_t width, size_t height);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;

  private:
    RenderTargetType target_type;  //��� ������
    TextureDesc      desc;         //���������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class BlendState: virtual public IBlendState, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BlendState  (const ContextManager&, const BlendDesc&);
    ~BlendState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDesc (const BlendDesc&);
    void GetDesc (BlendDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

  private:
    BlendDesc desc;         //���������� ���������
    size_t    desc_hash;    //��� ����������� ���������
    int       display_list; //����� ������ ������ ������������ OpenGL
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DepthStencilState: virtual public IDepthStencilState, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DepthStencilState  (const ContextManager&, const DepthStencilDesc&);
    ~DepthStencilState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDesc (const DepthStencilDesc&);
    void GetDesc (DepthStencilDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (size_t stencil_reference);

  private:
    DepthStencilDesc desc;                           //���������� ���������
    size_t           desc_hash;                      //��� ����������� ���������
    int              display_list;                   //����� ������ ������ ������������ OpenGL
    GLenum           gl_stencil_func [FaceMode_Num]; //������� ��������� ���������
    bool             need_two_side_stencil;          //��������� ������� ������������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RasterizerState : virtual public IRasterizerState, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RasterizerState  (const ContextManager& manager, const RasterizerDesc& desc);
    ~RasterizerState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDesc (const RasterizerDesc&);
    void GetDesc (RasterizerDesc&);

  private:
    RasterizerDesc desc;         //���������� ���������
    size_t         desc_hash;    //��� ����������� ���������
    int            display_list; //����� ������ ������ ������������ ��������� � OpenGL
};

}

}

}

#endif
