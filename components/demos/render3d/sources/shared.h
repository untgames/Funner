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
#include <stl/map>

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
#include <sg/light.h>
#include <sg/scene.h>
#include <sg/visual_model.h>

#include <media/image.h>
#include <media/mesh.h>
#include <media/rfx/material_library.h>

#include <physics/low_level/driver.h>
#include <physics/low_level/material.h>
#include <physics/low_level/scene.h>
#include <physics/low_level/shape.h>
#include <physics/low_level/rigid_body.h>

#include <input/low_level/device.h>
#include <input/low_level/driver.h>

using namespace render::low_level;
using namespace scene_graph;

const size_t MAX_SAMPLERS = 4;

/*
    Получение смещения поля в структуре (аналог offsetof, компилируемый без предупреждений на gcc)
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

//менеджер сцен
class SceneManager
{
  public:
    scene_graph::Scene& Scene () { return scene; }

    Node::Pointer LoadScene (const char* file_name);
    
    void AddShattle  (Node::Pointer shattle);
    void AddMainShip (Node::Pointer ship);

    void SetDrawShattles  (bool draw);
    void SetDrawMainShips (bool draw);
    
    typedef stl::vector<Node::Pointer> NodesArray;    
    
    NodesArray& Shattles () { return shattles; }
    NodesArray& MainShips () { return main_ships; }

  private:
    void ReadNodeInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent);
    void ReadMeshInfo (common::Parser::Iterator node, scene_graph::Node::Pointer parent);

  private:
    scene_graph::Scene scene;
    NodesArray         shattles;
    NodesArray         main_ships;
};

#ifdef _MSC_VER
  #pragma pack(push)
#endif

#pragma pack (1)

struct ModelShader
{
  stl::string name;
  ProgramPtr  program;
};

typedef xtl::shared_ptr<ModelShader> ModelShaderPtr;

struct ModelTexmap
{
  TexturePtr      texture;
  SamplerStatePtr sampler;
};

struct MaterialShaderParams
{
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

//построение сферы
enum SamplerChannel
{
  SamplerChannel_Diffuse,
  SamplerChannel_Specular,
  SamplerChannel_Reflection,
  SamplerChannel_Bump,
  SamplerChannel_Ambient,
  SamplerChannel_Emission,
  
  SamplerChannel_Num
};

struct ModelMaterial
{
  ModelTexmap    texmaps [SamplerChannel_Num];
  BufferPtr      constant_buffer;
  ModelShaderPtr shader;
};

typedef xtl::shared_ptr<ModelMaterial>  ModelMaterialPtr;
typedef stl::hash_map<stl::hash_key<const char*>, ModelMaterialPtr> ModelMaterialMap;

typedef stl::vector<BufferPtr> BufferArray;

struct ModelVertexBuffer
{
  BufferArray                   vertex_streams;
  InputLayoutPtr                input_layout;
  media::geometry::VertexBuffer source_vertex_buffer;
  size_t                        id;
};

typedef xtl::shared_ptr<ModelVertexBuffer>     VertexBufferPtr;
typedef stl::hash_map<size_t, VertexBufferPtr> VertexBufferMap;
typedef stl::hash_map<size_t, BufferPtr>       BufferMap;
typedef stl::vector<VertexBufferPtr>           VertexBufferArray;

struct ModelPrimitive
{
  PrimitiveType    type;          //тип примитива
  ModelMaterialPtr material;      //материал
  VertexBufferPtr  vertex_buffer; //вершинный буфер
  size_t           first;         //индекс первой вершины/индекса
  size_t           count;         //количество примитивов
};

typedef stl::vector<ModelPrimitive> PrimitiveArray;
typedef stl::vector<math::vec3f>    VertexArray;

struct ModelMesh
{
  VertexBufferArray vertex_buffers;
  BufferPtr         index_buffer;
  PrimitiveArray    primitives;
  VertexArray       vertices;
  stl::string       name;

  ModelMesh (const char* in_name)
    : name (in_name)
    {}    
};

void build_vertices (Node& root, Test& test, VertexArray& verts);

typedef xtl::shared_ptr<ModelMesh> ModelMeshPtr;

///Менеджер шейдеров
class ShaderManager
{
  public:
    ShaderManager (Test&);
    
///Установка каталога шейдеров
    void SetShadersDir (const char* name);
    const char* ShadersDir ();
  
///Загрузка шейдера
    void LoadShader (const char* name);
    
///Поиск шейдера
    ModelShaderPtr FindShader (const char* name);
    ModelShaderPtr GetShader  (const char* name);
    
///Перезагрузка шейдеров
    void ReloadShaders ();
    
  private:
    ProgramPtr CreateProgram (const char* name);
  
  private:
    typedef stl::vector<ModelShaderPtr> ShaderArray;
    
  private:
    Test&       test;
    ShaderArray shaders;
    stl::string shaders_dir;
};

///Менеджер материалов
class MaterialManager
{
  public:
    MaterialManager (Test& test);
    
///Установка текстуры отражений
    void SetReflectionTexture (const char* name);
  
///Загрузка материалов
    void LoadMaterials (const char* file_name);

///Поиск материала
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

///Менеджер мешей
class MeshManager
{
  public:
    MeshManager (Test& test);
  
///Загрузка мешей
    void LoadMeshes (const char* file_name);
    
///Регистрация меша
    void RegisterMesh (const ModelMeshPtr&);
    
///Поиск меша
    ModelMeshPtr FindMesh (const char* name);
    
  private:  
    Test&          test;
    ModelMeshArray meshes;  
};

//создание сферы
ModelMeshPtr create_sphere (const char* name, IDevice& device, 
size_t parallels, size_t meridians, const ModelMaterialPtr& material);

void draw (IDevice&, ModelMesh&);

///Визуализатор сцены
class SceneRenderer: public xtl::visitor<void, scene_graph::VisualModel>
{
  public:
    SceneRenderer (Test&);
    
///Инициализация ресурсов
    void InitializeResources ();
    
///Рисование сцены
    void Draw (scene_graph::Camera&);
        
  protected:
///Обновление источников света
    void UpdateLights ();
  
///Рисование объектов
    void visit (scene_graph::VisualModel&);
    
  private:  
    Test&                      test;
    math::mat4f                view_projection_tm;
    math::mat4f                view_tm;
    RasterizerStatePtr         rasterizer;
    ProgramParametersLayoutPtr program_parameters_layout;
    BufferPtr                  common_cb;
    BufferPtr                  transformations_cb;
};

typedef xtl::com_ptr<physics::low_level::IRigidBody> RigidBodyPtr;

///Тестовое приложение
struct Test
{
  public:
    typedef xtl::function<void (Test&)>                  CallbackFn;
    typedef xtl::com_ptr<physics::low_level::IDriver>    PhysicsDriverPtr;
    typedef xtl::com_ptr<physics::low_level::IScene>     PhysicsScenePtr;
    typedef stl::map<Node::Pointer, RigidBodyPtr>        RigidBodiesMap;
    typedef xtl::com_ptr<input::low_level::IDriver>      InputDriverPtr;
    typedef xtl::com_ptr<input::low_level::IDevice>      InputDevicePtr;
    typedef stl::vector<InputDevicePtr>                  InputDevices;

    syslib::Window             window;
    SwapChainPtr               swap_chain;
    DevicePtr                  device;
    CallbackFn                 redraw;
    CallbackFn                 reload;
    ShaderManager              shader_manager;
    SceneManager               scene_manager;
    SceneRenderer              scene_renderer;
    MeshManager                mesh_manager;
    MaterialManager            material_manager;
    PerspectiveCamera::Pointer camera;
    DirectLight::Pointer       light;
    PhysicsDriverPtr           physics_driver;
    PhysicsScenePtr            physics_scene;
    RigidBodiesMap             rigid_bodies;
    RigidBodyPtr               camera_body;
    InputDriverPtr             input_driver;
    InputDevices               input_devices;
    float                      x_camera_speed;
    float                      y_camera_speed;
    float                      x_camera_rotation_speed;
    float                      y_camera_rotation_speed;
    float                      z_camera_rotation_speed;
    bool                       physics_enabled;
    bool                       draw_shattles;
    bool                       draw_main_ships;
    bool                       pressed_keys [syslib::Key_Num];

    Test (const wchar_t* title, const CallbackFn& in_redraw, const CallbackFn& in_reload, const char* adapter_mask="*", const char* init_string="");

  private:
    void OnKeyPressed  (const syslib::WindowEventContext& context);
    void OnKeyReleased (const syslib::WindowEventContext& context);
    void OnResize ();
    void OnRedraw ();
    void OnClose ();
    void OnCameraUpdate ();
    void OnInputEvent (const char* event);
};

#ifdef _MSC_VER
  #pragma pack (pop)
#endif

#endif
