#ifndef RENDER_GL_DRIVER_OUTPUT_STAGE_SHARED_HEADER
#define RENDER_GL_DRIVER_OUTPUT_STAGE_SHARED_HEADER

#include <memory.h>

#include <render/low_level/utils.h>

#include <shared/output_stage.h>
#include <shared/context_object.h>
#include <shared/object.h>
#include <shared/trackable.h>
#include <shared/texture_manager.h>
#include <shared/platform/swap_chain_manager.h>

#include <stl/list>

#include <xtl/intrusive_ptr.h>
#include <xtl/bind.h>
#include <xtl/uninitialized_storage.h>

#include <common/exception.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ColorBuffer: virtual public ITexture, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (TextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, void* buffer);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ColorBuffer (const ContextManager&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetSize (size_t& width, size_t& height) = 0;

  private:
    TextureDesc desc; //���������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������-��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class DepthStencilBuffer: virtual public ITexture, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (TextureDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, const void* buffer);
    void GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, void* buffer);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DepthStencilBuffer (const ContextManager&);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void GetSize (size_t& width, size_t& height) = 0;    

  private:
    TextureDesc desc; //���������� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainColorBuffer: public ColorBuffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainColorBuffer (const ContextManager& manager, ISwapChain* swap_chain, size_t buffer_index);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ � gl-��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* GetSwapChain   () const { return swap_chain.get (); }
    size_t      GetBufferIndex () const { return buffer_index; }
    GLenum      GetBufferType  () const;

  private:
    void Bind ();
    void GetSize (size_t& width, size_t& height);

  private:
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

  private:
    SwapChainPtr swap_chain;   //������� ������
    size_t       buffer_index; //������ ������ ������ � ������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������-��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainDepthStencilBuffer: public DepthStencilBuffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainDepthStencilBuffer  (const ContextManager& manager, ISwapChain* swap_chain);
    ~SwapChainDepthStencilBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetContextId () const { return context_id; }

  private:
    void Bind ();
    void GetSize (size_t& width, size_t& height);

  private:
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

  private:
    SwapChainPtr swap_chain; //������� ������
    size_t       context_id; //�������������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���, ������������ ��� �������� ���������� ����������� ��� �������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct NullView {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ViewType
{
  ViewType_Null,                        //����������� �����������
  ViewType_SwapChainColorBuffer,        //����������� �� ����� ����� ������� ������
  ViewType_SwapChainDepthStencilBuffer, //����������� �� ����� �������-�������� ������� ������
  ViewType_Texture,                     //����������� �� ��������
//  ViewType_RenderBuffer  //����������� ��������� �� ����� ��������� ��� ����� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class View: virtual public IView, public Object, public Trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    View (ITexture* texture, const ViewDesc& desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ViewType GetType () { return type; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture*                    GetTexture                     () { return base_texture.get (); }
    IBindableTexture*            GetBindableTexture             () { return bindable_texture.get (); }
    SwapChainColorBuffer*        GetSwapChainColorBuffer        () { return color_buffer.get (); }
    SwapChainDepthStencilBuffer* GetSwapChainDepthStencilBuffer () { return depth_stencil_buffer.get (); }
//    RenderBuffer*      GetRenderBuffer    () { return render_buffer.get (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetDesc (ViewDesc&);

  private:
    typedef xtl::com_ptr<ITexture>                    TexturePtr;
    typedef xtl::com_ptr<IBindableTexture>            BindableTexturePtr;
    typedef xtl::com_ptr<SwapChainColorBuffer>        ColorBufferPtr;
    typedef xtl::com_ptr<SwapChainDepthStencilBuffer> DepthStencilBufferPtr;

  private:
    ViewType              type;                 //��� �����������
    TexturePtr            base_texture;         //��������� �� ������� ��������
    BindableTexturePtr    bindable_texture;     //��������
    ColorBufferPtr        color_buffer;         //������ ����� ������� ������
    DepthStencilBufferPtr depth_stencil_buffer; //����� �������-�������� ������� ������
    ViewDesc              desc;                 //���������� �����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class FrameBuffer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    virtual ~FrameBuffer () {}
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Bind () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateRenderTargets () = 0;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� �������� ��������� ��������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputStageResourceFactory
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~OutputStageResourceFactory () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ITexture* CreateTexture             (const TextureDesc&) = 0;
    virtual ITexture* CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index) = 0;
    virtual ITexture* CreateDepthStencilTexture (ISwapChain* swap_chain) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual FrameBuffer* CreateFrameBuffer (NullView, NullView) = 0;
    virtual FrameBuffer* CreateFrameBuffer (NullView, SwapChainDepthStencilBuffer*) = 0;
    virtual FrameBuffer* CreateFrameBuffer (NullView, IBindableTexture*) = 0;
    virtual FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, NullView) = 0;
    virtual FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, SwapChainDepthStencilBuffer*) = 0;
    virtual FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, IBindableTexture*) = 0;
    virtual FrameBuffer* CreateFrameBuffer (IBindableTexture*, NullView) = 0;
    virtual FrameBuffer* CreateFrameBuffer (IBindableTexture*, SwapChainDepthStencilBuffer*) = 0;
    virtual FrameBuffer* CreateFrameBuffer (IBindableTexture*, IBindableTexture*) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static OutputStageResourceFactory* CreateDefaultFactory (const ContextManager&);
    static OutputStageResourceFactory* CreateFboFactory     (const ContextManager&, OutputStageResourceFactory* default_factory);
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
    int       display_list; //����� ������ ������ ������������ OpenGL
};

}

}

}

#endif
