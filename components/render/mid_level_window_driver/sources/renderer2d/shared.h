#ifndef RENDER_MID_WINDOW_DRIVER_RENDERER2D_SHARED_HEADER
#define RENDER_MID_WINDOW_DRIVER_RENDERER2D_SHARED_HEADER

#include <float.h>

#include <stl/algorithm>

#include <xtl/common_exceptions.h>
#include <xtl/uninitialized_storage.h>

#include <math/utility.h>

#include <common/log.h>

#include <media/compressed_image.h>
#include <media/image.h>

#include <render/low_level/utils.h>

#include <shared/basic_renderer.h>

namespace render
{

namespace mid_level
{

namespace window_driver
{

namespace renderer2d
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t SPRITE_VERTICES_COUNT = 4; //���������� ������ � ����� �������
const size_t SPRITE_INDICES_COUNT  = 6; //���������� �������� � ����� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::com_ptr<render::low_level::IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<render::low_level::IBuffer>                  BufferPtr;
typedef xtl::com_ptr<render::low_level::IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<render::low_level::IProgram>                 ProgramPtr;
typedef xtl::com_ptr<render::low_level::IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<render::low_level::ISamplerState>            SamplerStatePtr;
typedef xtl::com_ptr<render::low_level::IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<render::low_level::IRasterizerState>         RasterizerStatePtr;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct CommonProgramParameters
{
  math::mat4f view_matrix;       //������� ����
  math::mat4f projection_matrix; //������� ��������
};

struct DynamicProgramParameters
{
  float alpha_reference; //�������� ��� ������ �����-�����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ��� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderableVertex
{
  math::vec3f  position; //��������� ������� � ������������
  math::vec3f  normal;   //������� ������� � ������������
  math::vec2f  texcoord; //���������� ����������
  math::vec4ub color;    //���� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����������� ��� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderablePrimitive
{
  float                             alpha_reference; //�������� ��� �����-�����
  render::low_level::ITexture*      texture;         //��������
  render::low_level::Rect*          scissor;         //������� ���������
  mid_level::renderer2d::BlendMode  blend_mode;      //����� ���������� ������
  mid_level::renderer2d::ShaderMode shader_mode;     //����� ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������� ��� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderableSprite
{
  RenderablePrimitive* primitive;                        //������ �� ��������, �������� ����������� ������
  RenderableVertex     vertices [SPRITE_VERTICES_COUNT]; //�������, ����������� ������
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
    render::low_level::IBlendState*              GetBlendState        (render::mid_level::renderer2d::BlendMode blend_mode) { return blend_states[blend_mode].get (); }
    render::low_level::IDepthStencilState*       GetDepthStencilState (bool depth_write_enabled);
    render::low_level::IProgram*                 GetDefaultProgram    () { return default_program.get (); }
    render::low_level::IProgram*                 GetAlphaClampProgram () { return alpha_clamp_program.get (); }
    render::low_level::IProgram*                 GetReflectionProgram () { return reflection_program.get (); }
    render::low_level::IProgramParametersLayout* GetProgramParametersLayout () { return program_parameters_layout.get (); }
    render::low_level::ISamplerState*            GetSamplerState      () { return sampler.get (); }
    render::low_level::IInputLayout*             GetInputLayout       () { return input_layout.get (); }
    render::low_level::IRasterizerState*         GetRasterizerState   (bool scissor_enabled);

  private:
    BlendStatePtr              blend_states [render::mid_level::renderer2d::BlendMode_Num];
    DepthStencilStatePtr       depth_stencil_states [2];
    ProgramPtr                 default_program;
    ProgramPtr                 alpha_clamp_program;
    ProgramPtr                 reflection_program;
    ProgramParametersLayoutPtr program_parameters_layout;
    SamplerStatePtr            sampler;
    InputLayoutPtr             input_layout;
    RasterizerStatePtr         rasterizer_states [2];
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ImageTexture: virtual public mid_level::renderer2d::ITexture, public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ImageTexture (render::low_level::IDevice& device, const media::Image& image);
    ImageTexture (render::low_level::IDevice& device, const media::CompressedImage& image);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetWidth  ();
    size_t GetHeight ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CaptureImage (media::Image&);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::ITexture* GetTexture ();

  private:
    typedef xtl::com_ptr<render::low_level::ITexture> TexturePtr;
    
  private:
    TexturePtr texture;
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
    RenderTargetTexture (render::low_level::IDevice& device, size_t width, size_t height, PixelFormat format);
    
  private:
    typedef xtl::com_ptr<render::low_level::IView> ViewPtr;
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ViewPtr CreateView (render::low_level::IDevice& device, size_t width, size_t height, PixelFormat format);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderableSpriteList
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderableSpriteList  ();
    ~RenderableSpriteList ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size () { return data_buffer.size (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ������ / ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::low_level::IBuffer* GetVertexBuffer () { return vertex_buffer.get (); }
    render::low_level::IBuffer* GetIndexBuffer  () { return index_buffer.get (); }
    const RenderableSprite**    GetSprites      () { return data_buffer.data (); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add (size_t sprites_count, const RenderableSprite* sprites);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����� ��� ���������� ���������� ����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reserve (size_t sprites_count);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Predicate> void Sort (Predicate pred)
    {
      stl::sort (data_buffer.data (), data_buffer.data () + data_buffer.size (), pred);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void UpdateVertexBuffer (render::low_level::IDevice&);

  private:
    void ResizeBuffers (render::low_level::IDevice&, size_t new_sprites_count);
    
  private:
    typedef xtl::uninitialized_storage<const RenderableSprite*> SpriteArray;

  private:
    size_t      buffers_sprites_count; //������� ������ ���������� �������� � ��������� � ��������� �������
    BufferPtr   vertex_buffer;         //��������� �����
    BufferPtr   index_buffer;          //��������� �����
    SpriteArray data_buffer;           //��������� ����� � ������� ���������
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
    render::low_level::ITexture* GetLowLevelTexture () { return renderable_primitive.texture; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                             SetBlendMode (mid_level::renderer2d::BlendMode blend_mode);
    mid_level::renderer2d::BlendMode GetBlendMode ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                              SetShaderMode (mid_level::renderer2d::ShaderMode mode);
    mid_level::renderer2d::ShaderMode GetShaderMode ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ��� ������ �����-�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetAlphaReference (float ref);
    float GetAlphaReference ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetScissor      (const Viewport&);
    void SetScissorState (bool state);
    void GetScissor      (Viewport&);
    bool GetScissorState ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetSpritesCount  ();
    void   AddSprites       (size_t sprites_count, const mid_level::renderer2d::Sprite* sprites);
    void   AddSprites       (size_t sprites_count, const mid_level::renderer2d::SpriteVertex* vertices);
    void   RemoveAllSprites ();
    void   ReserveSprites   (size_t sprites_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddSprites (RenderableSpriteList& dst_buffer);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateRenderableSprites ();

  private:
    typedef stl::vector<mid_level::renderer2d::SpriteVertex>  VertexArray;
    typedef xtl::uninitialized_storage<RenderableSprite>      RenderableSpriteArray;
    typedef xtl::com_ptr<mid_level::renderer2d::ITexture>     TexturePtr;

  private:
    math::mat4f             transform;                      //������� �������������� ���������
    TexturePtr              texture;                        //��������
    VertexArray             sprite_vertices;                //������ ������ ��������
    RenderablePrimitive     renderable_primitive;           //���������, ����������� ��� ������������ ���������
    RenderableSpriteArray   renderable_sprites;             //������ ��������������� ��������
    render::low_level::Rect scissor_rect;                   //������� ���������
    bool                    scissor_state;                  //��������� ���������
    bool                    need_update_transform;          //���������� �������� ��������� ������
    bool                    need_update_renderable_sprites; //���������� �������� ��������� ������������ ��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� 2D ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Frame: virtual public mid_level::renderer2d::IFrame, public BasicFrame
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Frame (CommonResources* in_common_resources, render::low_level::IDevice* device);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� / ������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetViewPoint  (const math::vec3f&);
    void SetProjection (const math::mat4f&);
    void GetViewPoint  (math::vec3f&);
    void GetProjection (math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t PrimitivesCount ();
    void   AddPrimitive    (mid_level::renderer2d::IPrimitive*);
    void   Clear           ();

  private:
    typedef xtl::com_ptr<Primitive>        PrimitivePtr;
    typedef stl::vector<PrimitivePtr>      PrimitiveArray;
    typedef xtl::com_ptr<CommonResources>  CommonResourcesPtr;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateVertexBuffers (render::low_level::IDevice&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DrawCore (render::low_level::IDevice* device);

  private:
    CommonProgramParameters  common_program_parameters;  //����� ��������� ��������� ����������
    BufferPtr                common_constant_buffer;     //����������� ����� ��� �������� ����� ���������� ��������� ����������
    BufferPtr                dynamic_constant_buffer;    //����������� ����� ��� �������� ������������ ���������� ��������� ����������
    PrimitiveArray           primitives;                 //������ ����������
    CommonResourcesPtr       common_resources;           //����� �������

       //������� ����� �������!!!
    RenderableSpriteList     not_blended_sprites;        //������� ��� ���������
    RenderableSpriteList     blended_sprites;            //������� � ����������
};

}

}

}

}

#endif
