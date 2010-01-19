#ifndef RENDER_GL_DRIVER_COMPLEX_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_COMPLEX_TESTS_SHARED_HEADER

#include <cstdio>
#include <cmath>
#include <exception>

#include <float.h>

#include <math/matrix.h>
#include <math/utility.h>

#include <syslib/window.h>
#include <syslib/application.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

#include <stl/string>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/visitor.h>

#include <common/file.h>
#include <common/hash.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/singleton.h>
#include <common/string.h>
#include <common/strlib.h>
#include <common/time.h>

#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/visual_model.h>

#include <media/image.h>
#include <media/mesh.h>
#include <media/rfx/material_library.h>

using namespace render::low_level;
using namespace scene_graph;

/*
    ��������� �������� ���� � ��������� (������ offsetof, ������������� ��� �������������� �� gcc)
*/

#ifdef _MSC_VER
  #define TEST_OFFSETOF(X,Y) offsetof(X,Y)
#else
  #define TEST_OFFSETOF(X,Y) (reinterpret_cast<size_t> (&(static_cast<X*> (0)->*(&X::Y))))
#endif

typedef xtl::com_ptr<IDriver>                  DriverPtr;
typedef xtl::com_ptr<ISwapChain>               SwapChainPtr;
typedef xtl::com_ptr<IDevice>                  DevicePtr;
typedef xtl::com_ptr<ITexture>                 TexturePtr;
typedef xtl::com_ptr<ISamplerState>            SamplerStatePtr;
typedef xtl::com_ptr<IView>                    ViewPtr;
typedef xtl::com_ptr<IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<IBuffer>                  BufferPtr;
typedef xtl::com_ptr<IRasterizerState>         RasterizerStatePtr;
typedef xtl::com_ptr<IProgram>                 ProgramPtr;
typedef xtl::com_ptr<IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<IPredicate>               PredicatePtr;

struct Test;

//�������� ����
class SceneManager
{
  public:
    scene_graph::Scene& Scene () { return scene; }

    void LoadScene (const char* file_name);
    
  private:
    void ReadNodeInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent);
    void ReadMeshInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent);

  private:
    scene_graph::Scene scene;
};

//������ ��������� ������ ������� � ������
stl::string read_shader (const char* file_name);

#ifdef _MSC_VER
  #pragma pack(push)
#endif

#pragma pack (1)

enum ConstantBufferSemantic
{
  ConstantBufferSemantic_Common,
  ConstantBufferSemantic_Transformations,
  ConstantBufferSemantic_Material,
};

struct CommonShaderParams
{
  math::vec3f light_pos;
  math::vec3f light_dir;
  int         diffuse_sampler;
  int         specular_sampler;
  int         bump_sampler;
  int         ambient_sampler;
  int         emission_sampler;
  int         reflection_sampler;
};

struct TransformationsShaderParams
{
  math::mat4f object_tm;
  math::mat4f model_view_tm;
  math::mat4f model_view_proj_tm;
};

//���������� �����
enum SamplerChannel
{
  SamplerChannel_Diffuse,
  SamplerChannel_Specular,
  SamplerChannel_Bump,
  SamplerChannel_Ambient,
  SamplerChannel_Emission,
  SamplerChannel_Reflection,
  
  SamplerChannel_Num
};

enum ShaderType
{
  ShaderType_Phong,
  ShaderType_Lambert,
  ShaderType_Constant
};

struct MaterialShaderParams
{
  ShaderType  shader_type;
  float       reflectivity;
  float       transparency;
  float       shininess;
  float       bump_amount;
  int         bump_texture_channel;
  math::vec4f diffuse_color;
  int         diffuse_texture_channel;
  math::vec4f ambient_color;
  int         ambient_texture_channel;  
  math::vec4f specular_color;
  int         specular_texture_channel;
  math::vec4f emission_color;
  int         emission_texture_channel;
};

struct ModelTexmap
{
  TexturePtr      texture;
  SamplerStatePtr sampler;
};

struct ModelMaterial
{
  ModelTexmap texmaps [SamplerChannel_Num];
  BufferPtr   constant_buffer;
};

typedef xtl::shared_ptr<ModelMaterial>  ModelMaterialPtr;
typedef stl::hash_map<stl::hash_key<const char*>, ModelMaterialPtr> ModelMaterialMap;

typedef stl::vector<BufferPtr> BufferArray;

struct ModelVertexBuffer
{
  BufferArray    vertex_streams;
  InputLayoutPtr input_layout;
  size_t         id;
};

typedef xtl::shared_ptr<ModelVertexBuffer>     VertexBufferPtr;
typedef stl::hash_map<size_t, VertexBufferPtr> VertexBufferMap;
typedef stl::hash_map<size_t, BufferPtr>       BufferMap;
typedef stl::vector<VertexBufferPtr>           VertexBufferArray;

struct ModelPrimitive
{
  PrimitiveType    type;          //��� ���������
  ModelMaterialPtr material;      //��������
  VertexBufferPtr  vertex_buffer; //��������� �����
  size_t           first;         //������ ������ �������/�������
  size_t           count;         //���������� ����������
};

typedef stl::vector<ModelPrimitive> PrimitiveArray;

struct ModelMesh
{
  VertexBufferArray vertex_buffers;
  BufferPtr         index_buffer;
  PrimitiveArray    primitives;
  stl::string       name;

  ModelMesh (const char* in_name)
    : name (in_name)
    {}
};

typedef xtl::shared_ptr<ModelMesh> ModelMeshPtr;

///�������� ����������
class MaterialManager
{
  public:
    MaterialManager (Test& test);
    
///��������� �������� ���������
    void SetReflectionTexture (const char* name);
  
///�������� ����������  
    void LoadMaterials (const char* file_name);

///����� ���������
    ModelMaterialPtr FindMaterial (const char* name);

  private:
    void            LoadMaterial (const char* id, const media::rfx::Material& src_mtl);
    SamplerStatePtr CreateSampler ();
    TexturePtr      LoadTexture (const char* name);
    PixelFormat     GetPixelFormat (media::Image& image);
    
    typedef stl::hash_map<stl::hash_key<const char*>, TexturePtr> TextureMap;
  
  private:
    Test&            test;
    TextureMap       textures;
    SamplerStatePtr  default_sampler;
    ModelMaterialMap materials;
    TexturePtr       default_reflection_texture;
};

typedef stl::vector<ModelMeshPtr> ModelMeshArray;  

///�������� �����
class MeshManager
{
  public:
    MeshManager (Test& test);
  
///�������� �����
    void LoadMeshes (const char* file_name);
    
///����� ����
    ModelMeshPtr FindMesh (const char* name);
    
  private:  
    Test&          test;
    ModelMeshArray meshes;  
};

//�������� �����
ModelMeshPtr create_sphere (size_t parallels, size_t meridians);

void draw (IDevice&, ModelMesh&);

///������������ �����
class SceneRenderer: public xtl::visitor<void, scene_graph::VisualModel>
{
  public:
    SceneRenderer (Test&);
    
///��������� �����
    void Draw (scene_graph::Camera&);
        
  protected:
///��������� ��������  
    void visit (scene_graph::VisualModel&);
    
  private:  
    Test&       test;
    math::mat4f view_projection_tm;
    math::mat4f view_tm;
};

///�������� ����������
struct Test
{
  public:
    typedef xtl::function<void (Test&)> CallbackFn;

    syslib::Window             window;
    SwapChainPtr               swap_chain;
    DevicePtr                  device;
    CallbackFn                 redraw;
    CallbackFn                 reload;
    ProgramPtr                 shader;
    SceneManager               scene_manager;
    SceneRenderer              scene_renderer;
    MeshManager                mesh_manager;
    MaterialManager            material_manager;
    PerspectiveCamera::Pointer camera;
    float                      x_camera_speed;
    float                      y_camera_speed;
    float                      x_camera_rotation_speed;
    float                      y_camera_rotation_speed;
    bool                       pressed_keys [syslib::Key_Num];

    Test (const wchar_t* title, const CallbackFn& in_redraw, const CallbackFn& in_reload, const char* adapter_mask="*", const char* init_string="");

  private:
    void OnKeyPressed  (const syslib::WindowEventContext& context);
    void OnKeyReleased (const syslib::WindowEventContext& context);
    void OnResize ();
    void OnRedraw ();
    void OnClose ();
    void OnCameraUpdate ();
};

#ifdef _MSC_VER
  #pragma pack (pop)
#endif

#endif
