#ifndef RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_SHARED_HEADER
#define RENDER_MID_LEVEL_LOW_LEVEL_DRIVER_SHARED_HEADER

#include <climits>

#include <stl/algorithm>
#include <stl/string>
#include <stl/vector>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/string.h>

#include <common/log.h>
#include <common/singleton.h>

#include <media/image.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>
#include <render/low_level/utils.h>

#include <render/mid_level/driver.h>
#include <render/mid_level/low_level_driver.h>
#include <render/mid_level/renderer2d.h>

#ifdef _MSC_VER
  #pragma warning (disable : 4250) //'class1' : inherits 'class2::member' via dominance
#endif

namespace render
{

namespace mid_level
{

namespace low_level_driver
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Object: virtual public IObject, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();    
    void Release ();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RenderTargetType
{
  RenderTargetType_Color,        //����� �����
  RenderTargetType_DepthStencil, //����� ������������� ���������
  
  RenderTargetType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTarget: virtual public IRenderTarget, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget (render::low_level::IView* render_target_view, RenderTargetType type);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetType GetType () { return type; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetWidth  ();
    size_t GetHeight ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CaptureImage (media::Image&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� View
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::IView* GetView () { return render_target_view.get (); } 
  
  private:
    typedef xtl::com_ptr<render::low_level::IView> IViewPtr;

  private:
    IViewPtr         render_target_view;
    RenderTargetType type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class BasicFrame: virtual public IFrame, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BasicFrame ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void           SetRenderTargets      (IRenderTarget* render_target, IRenderTarget* depth_stencil_target);
    IRenderTarget* GetRenderTarget       (); //������� ����� �����
    IRenderTarget* GetDepthStencilTarget (); //������� ����� ������������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Draw (render::low_level::IDevice* device, const low_level::Rect& device_viewport);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetDeviceViewportCore (render::low_level::IDevice* device, const low_level::Rect& device_viewport) = 0;
    virtual void DrawCore              (render::low_level::IDevice* device) = 0;

  private:
    typedef xtl::com_ptr<RenderTarget> RenderTargetPtr;

  private:
    RenderTargetPtr             render_target;
    RenderTargetPtr             depth_stencil_target;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� � �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ViewportFrame: virtual public IViewportFrame, public BasicFrame
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ViewportFrame ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetRenderTargets (IRenderTarget* render_target, IRenderTarget* depth_stencil_target);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetViewport (const render::mid_level::Viewport&);
    void GetViewport (render::mid_level::Viewport&);
   
  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDeviceViewportCore (render::low_level::IDevice* device, const low_level::Rect& device_viewport);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateNormalizedViewport ();

  private:
    render::low_level::Viewport viewport;
    float                       normalized_viewport_left;
    float                       normalized_viewport_top;
    float                       normalized_viewport_width;
    float                       normalized_viewport_height;
    bool                        need_update_normalized_viewport;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClearFrame: virtual public IClearFrame, public BasicFrame
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ClearFrame ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetFlags (size_t clear_flags);
    size_t GetFlags () { return clear_flags; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetColor (const math::vec4f& color);
    void GetColor (math::vec4f& color);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void          SetDepthValue   (float depth_value);
    void          SetStencilIndex (unsigned char stencil_index);
    float         GetDepthValue   () { return clear_depth_value; }
    unsigned char GetStencilIndex () { return clear_stencil_index; }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetDeviceViewportCore (render::low_level::IDevice* device, const low_level::Rect& device_viewport) {}
    void DrawCore (render::low_level::IDevice*);

  private:
    size_t                     clear_flags;
    render::low_level::Color4f clear_color;
    float                      clear_depth_value;
    unsigned char              clear_stencil_index;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class BasicRenderer: virtual public IRenderer, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BasicRenderer (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain);
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ���� ��������
///  (��������� ��� ����������� ������������� ��������, ��������� �� ������ IRenderer)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetResourcePoolId ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ����� � ������ ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderTarget* GetColorBuffer ();
    IRenderTarget* GetDepthStencilBuffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderTarget* CreateDepthStencilBuffer ();
    IRenderTarget* CreateRenderBuffer       ();
    IClearFrame*   CreateClearFrame         ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� � ������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddFrame (IFrame*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� / ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawFrames   ();
    void CancelFrames ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ��� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                   SetViewport (const low_level::Rect& viewport);
    const low_level::Rect& GetViewport () const { return viewport; }
    
  private:
    typedef xtl::com_ptr<IRenderTarget>                 RenderTargetPtr;
    typedef xtl::com_ptr<render::low_level::IDevice>    DevicePtr;
    typedef xtl::com_ptr<render::low_level::ISwapChain> SwapChainPtr;
    typedef xtl::com_ptr<BasicFrame>                    FramePtr;
    typedef stl::vector<FramePtr>                       FrameArray;

  protected:
    DevicePtr device;

  private:
    RenderTargetPtr color_buffer;
    RenderTargetPtr depth_stencil_buffer;
    SwapChainPtr    swap_chain;
    FrameArray      frames;
    size_t          resource_pool_id;  
    low_level::Rect viewport;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Driver: virtual public IDriver
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Driver  ();
    ~Driver ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������� ������ ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      GetRenderersCount ();    
    const char* GetRendererName   (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRenderer* CreateRenderer (const char* name);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ��� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetViewport (const char* name, const low_level::Rect& viewport);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterRenderer       (const char* name, render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain);
    void UnregisterRenderer     (const char* name);
    void UnregisterAllRenderers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  () {}
    void Release () {}

  private:
    void UnregisterDriver ();

  private:
    struct RendererEntry;

    typedef xtl::shared_ptr<RendererEntry> RendererEntryPtr;
    typedef stl::vector<RendererEntryPtr>  RendererEntries;

  private:
    RendererEntries renderer_entries;
};

namespace renderer2d
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ImageTexture: virtual public mid_level::renderer2d::ITexture, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImageTexture  (render::low_level::ITexture*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetWidth  ();
    size_t GetHeight ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    media::PixelFormat GetFormat ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CaptureImage (media::Image&);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::ITexture* GetTexture ();

  private:
    typedef xtl::com_ptr<render::low_level::ITexture> ITexturePtr;
    
  private:
    ITexturePtr texture;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� � ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderTargetTexture: virtual public mid_level::renderer2d::ITexture, public RenderTarget
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetTexture (render::low_level::IView* render_target_view);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������, ����������� ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SpriteVertexData
{
  struct SpriteVertex
  {
    math::vec3f position;
    math::vec2f texcoord;
  };

  SpriteVertex                     vertices[4];
  math::vec4f                      color;
  render::low_level::ITexture*     texture;
  mid_level::renderer2d::BlendMode blend_mode;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Primitive: virtual public mid_level::renderer2d::IPrimitive, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Primitive  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetTransform (const math::mat4f&);
    void GetTransform (math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                             SetTexture (mid_level::renderer2d::ITexture*);
    mid_level::renderer2d::ITexture* GetTexture ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::ITexture* GetLowLevelTexture () { return low_level_texture; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                             SetBlendMode (mid_level::renderer2d::BlendMode blend_mode);
    mid_level::renderer2d::BlendMode GetBlendMode ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetSpritesCount  ();
    void   GetSprite        (size_t index, mid_level::renderer2d::Sprite& sprite);
    size_t AddSprites       (size_t sprites_count, const mid_level::renderer2d::Sprite* sprites);
    void   RemoveSprites    (size_t first_sprite, size_t sprites_count);
    void   RemoveAllSprites ();
    void   ReserveSprites   (size_t sprites_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ��� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SpriteVertexData* GetSpriteVertexBuffer ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ������ ��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BuildSpriteVertexData (size_t sprite_index);
    void ComputeSpriteTransorm (size_t sprite_index);

  private:
    typedef stl::vector<mid_level::renderer2d::Sprite>    SpriteArray;
    typedef xtl::uninitialized_storage<SpriteVertexData>  SpriteVertexArray;
    typedef xtl::com_ptr<mid_level::renderer2d::ITexture> TexturePtr;

  private:
    math::mat4f                      transform;
    TexturePtr                       texture;
    render::low_level::ITexture      *low_level_texture;
    mid_level::renderer2d::BlendMode blend_mode;
    SpriteArray                      sprites;
    SpriteVertexArray                sprite_vertex_buffer;
    bool                             dirty_transform;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ProgramParameters
{
  math::mat4f view_matrix;
  math::mat4f projection_matrix;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class CommonResources : public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    CommonResources (render::low_level::IDevice* device);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::IBlendState*        GetBlendState (render::mid_level::renderer2d::BlendMode blend_mode) { return blend_states[blend_mode].get (); }
    render::low_level::IBuffer*            GetConstantBuffer () { return constant_buffer.get (); }
    render::low_level::IDepthStencilState* GetDepthStencilState (bool depth_write_enabled);

  private:
    typedef xtl::com_ptr<render::low_level::IBlendState>        BlendStatePtr;
    typedef xtl::com_ptr<render::low_level::IBuffer>            BufferPtr;
    typedef xtl::com_ptr<render::low_level::IDepthStencilState> DepthStencilStatePtr;
  
  private:
    BlendStatePtr        blend_states[render::mid_level::renderer2d::BlendMode_Num];
    BufferPtr            constant_buffer;
    DepthStencilStatePtr depth_stencil_states[2];
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������, ����������� ��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderedSpriteVertex
{
  math::vec3f position;
  math::vec2f texcoord;
  math::vec4f color;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� 2D ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Frame: virtual public mid_level::renderer2d::IFrame, public ViewportFrame
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Frame (CommonResources* in_common_resources, render::low_level::IDevice* device);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� / ������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetView       (const math::mat4f&);
    void SetProjection (const math::mat4f&);
    void GetView       (math::mat4f&);
    void GetProjection (math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t PrimitivesCount ();
    void   AddPrimitive    (mid_level::renderer2d::IPrimitive*);
    void   Clear           ();

  private:
    typedef xtl::com_ptr<Primitive>                  PrimitivePtr;
    typedef stl::vector<PrimitivePtr>                PrimitiveArray;
    typedef stl::vector<SpriteVertexData*>           SpriteVertexArray;
    typedef xtl::com_ptr<render::low_level::IBuffer> BufferPtr;
    typedef xtl::com_ptr<CommonResources>            CommonResourcesPtr;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����� ��� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ReserveNotBlendedSpritesVertexBuffer (render::low_level::IDevice* device, size_t sprites_count);
    void ReserveBlendedSpritesVertexBuffer    (render::low_level::IDevice* device, size_t sprites_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MakeVertexBuffer (SpriteVertexArray& vertex_data_array, BufferPtr& buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawCore (render::low_level::IDevice* device);

  private:
    math::mat4f                   view_tm, proj_tm;
    PrimitiveArray                primitives;
    CommonResourcesPtr            common_resources;
    BufferPtr                     not_blended_sprites_vertex_buffer;              //��������� ������ �������� ��� ���������
    BufferPtr                     blended_sprites_vertex_buffer;                  //��������� ������ �������� � ����������
    SpriteVertexArray             not_blended_sprites_vertex_data_buffer;         //����������� ������, �� �������� ����������� ������ ��� ���������
    SpriteVertexArray             blended_sprites_vertex_data_buffer;             //����������� ������, �� �������� ����������� ������ ��� ���������
    size_t                        current_not_blended_sprites_vertex_buffer_size; //������� ������ ���������� ������� �������� ��� ���������
    size_t                        current_blended_sprites_vertex_buffer_size;     //������� ������ ���������� ������� �������� � ����������
    render::low_level::BufferDesc vertex_buffer_desc;
    
    typedef xtl::uninitialized_storage<RenderedSpriteVertex> RenderedSpriteVertexArray;
    
    RenderedSpriteVertexArray vertex_buffer_cache;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� 2D-����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Renderer: virtual public mid_level::renderer2d::IRenderer, public BasicRenderer
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Renderer  (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    mid_level::renderer2d::ITexture*   CreateTexture   (const media::Image& image);
    mid_level::renderer2d::ITexture*   CreateTexture   (size_t width, size_t height, media::PixelFormat pixel_format);
    mid_level::renderer2d::IPrimitive* CreatePrimitive ();
    mid_level::renderer2d::IFrame*     CreateFrame     ();

  private:
    typedef xtl::com_ptr<render::low_level::IProgram>                 ProgramPtr;
    typedef xtl::com_ptr<render::low_level::IProgramParametersLayout> ProgramParametersLayoutPtr;
    typedef xtl::com_ptr<render::low_level::ISamplerState>            SamplerStatePtr;
    typedef xtl::com_ptr<render::low_level::IInputLayout>             InputLayoutPtr;
    typedef xtl::com_ptr<CommonResources>                             CommonResourcesPtr;

  private:
    ProgramPtr                 program;
    ProgramParametersLayoutPtr program_parameters_layout;
    SamplerStatePtr            sampler;
    InputLayoutPtr             input_layout;
    CommonResourcesPtr         common_resources;
};

}

}

}

}

#endif

