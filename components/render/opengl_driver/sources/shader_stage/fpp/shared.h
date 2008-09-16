#ifndef RENDER_GL_DRIVER_FPP_SHADER_MANAGER_SHARED_HEADER
#define RENDER_GL_DRIVER_FPP_SHADER_MANAGER_SHARED_HEADER

#include "../shared.h"

#include <stl/string>
#include <stl/hash_map>

#include <xtl/string.h>

#include <common/parser.h>
#include <common/strlib.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const size_t FPP_MAX_LIGHTS_COUNT = 8; //������������ ���������� ���������� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LightType
{
  LightType_Point,  //�������� �������� �����
  LightType_Remote, //�������� ������������ �������� �����
  LightType_Spot,   //�������������� ������������ �������� �����
  
  LightType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct LightDesc
{
  LightType type;                  //��� ��������� �����
  int       enable;                //��������� ��������� �����: �������/��������
  float     position [3];          //��������� ��������� �����
  float     direction [3];         //����������� ��������� �����
  Color4f   ambient_color;         //���� ����������� ������������ �����
  Color4f   diffuse_color;         //���� ������������ ������������ �����
  Color4f   specular_color;        //���� ��������� ������������ �����
  float     angle;                 //���� ���������
  float     exponent;              //���������� ��������� ��������� �� ����
  float     constant_attenuation;  //����������� ��������� �� ����������
  float     linear_attenuation;    //�������� ��������� �� ����������
  float     quadratic_attenuation; //������������ ��������� �� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TexcoordSource
{
  TexcoordSource_Explicit,      //���������� ���������� �������� ����
  TexcoordSource_SphereMap,     //��������� ���������� ���������: ����������� �����
  TexcoordSource_ReflectionMap, //��������� ���������� ���������: ����� ���������
  TexcoordSource_ObjectSpace,   //��������� ���������� ���������: texmap.transform * position
  TexcoordSource_ViewerSpace,   //��������� ���������� ���������: texmap.transform * modelviewprojection * position
  
  TexcoordSource_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ��� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextureBlend
{
  TextureBlend_Replace,  //������ ������������ ����� ������ ��������
  TextureBlend_Modulate, //��������� ������������ ����� �� ���� ��������
  TextureBlend_Blend,    //����������

  TextureBlend_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct TexmapDesc
{
  TextureBlend   blend;     //���������� ������
  Matrix4f       transform; //������� �������������� ���������� ���������
  Matrix4f       texgen;    //������� ���������� ��������� ���������� ���������
  TexcoordSource source_u;  //�������� ���������� ��������� ��� U-����������
  TexcoordSource source_v;  //�������� ���������� ��������� ��� V-����������
  TexcoordSource source_w;  //�������� ���������� ��������� ��� W-����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ����� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ColorMaterial
{
  ColorMaterial_Explicit,          //���� ��������� ������� ����������� ���������
  ColorMaterial_Emission,          //���� ��������� ����� emission-������������ ���������
  ColorMaterial_Ambient,           //���� ��������� ����� ambient-������������ ���������
  ColorMaterial_Diffuse,           //���� ��������� ����� diffuse-������������ ���������
  ColorMaterial_Specular,          //���� ��������� ����� specular-������������ ���������
  ColorMaterial_AmbientAndDiffuse, //���� ��������� ����� ambient � diffuse ������������ ���������  
  
  ColorMaterial_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialDesc
{
  Color4f       emission_color;     //���� ���������
  Color4f       ambient_color;      //���� ����������
  Color4f       diffuse_color;      //���� �����������
  Color4f       specular_color;     //���� ���������
  float         shininess;          //"�������������"
  ColorMaterial color_material;     //��������� �������� ����� ���������
  CompareMode   alpha_compare_mode; //����� ����� �����
  float         alpha_reference;    //��������� ����� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ViewerDesc
{
  Matrix4f projection_matrix; //������� �������������
  Matrix4f view_matrix;       //������� ����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ObjectDesc
{
  Matrix4f matrix; //������� �������������� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ModesDesc
{
  bool normalize; //���������� �� ��������������� �������  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RasterizationDesc
{
  float point_size;           //������ �����
  float line_width;           //������ �����
  int   line_stipple_factor;  //���������� ���������� ������� � ����� �����
  int   line_stipple_pattern; //����� �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FppState
{
  ModesDesc         modes;                              //������ ������������
  ViewerDesc        viewer;                             //��������� �����������
  ObjectDesc        object;                             //��������� �������
  RasterizationDesc rasterization;                      //��������� ������������
  LightDesc         lights [FPP_MAX_LIGHTS_COUNT];      //��������� ���������� ���������
  MaterialDesc      material;                           //�������� ���������
  TexmapDesc        maps [DEVICE_SAMPLER_SLOTS_COUNT];  //��������� ���������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum FppDynamicParameterType
{
  FppDynamicParameterType_Int,   //����� �����
  FppDynamicParameterType_Float, //������������ �����
  
  FppDynamicParameterType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� ����� ��������� FppState, ����������� ������ � ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef stl::vector<size_t> FppFieldOffsetArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������� fpp-������� (������ uniform-var � glsl)
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FppDynamicParameter
{
  FppDynamicParameterType type;          //��� ���������
  size_t                  count;         //���������� ���������
  FppFieldOffsetArray     field_offsets; //�������� ����� � ��������� FppState, ����������� ��� ��������� ���������
};

typedef stl::hash_map<stl::hash_key<const char*>, FppDynamicParameter> FppDynamicParameterMap;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Fixed-pipeline-program
///////////////////////////////////////////////////////////////////////////////////////////////////
class FppProgram: virtual public ICompiledProgram, virtual public IShader, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FppProgram  (const ContextManager&, const ShaderDesc& shader_desc, const LogFunction& error_log);
    ~FppProgram ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������� ������������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const FppState& GetBaseState () const { return base_state; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const FppDynamicParameter* FindDynamicParameter (const char* name) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������, ��������������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    IBindableProgram* CreateBindableProgram (ProgramParametersLayout* layout);

  private:
    FppState               base_state;         //������� ��������� ������������� ��������� ��������
    FppDynamicParameterMap dynamic_parameters; //������ ������������ ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Fixed-pipeline-program ��������������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
class FppBindableProgram: virtual public IBindableProgram, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FppBindableProgram (const ContextManager& manager, FppProgram& compiled_program, ProgramParametersLayout* layout);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� � �������� OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (ConstantBufferPtr* constant_buffers);  
    
  private:
    void UpdateHashes ();
    
  private:
    struct Parameter;
    struct Group;

    typedef stl::vector<Parameter>  ParameterArray;
    typedef stl::vector<Group>      GroupArray;

  private:
    ParameterArray  parameters;         //��������� ���������
    GroupArray      groups;             //������ ����������
    FppState        fpp_state;          //��������� ������������� ��������� ��������
    Matrix4f        view_object_matrix; //���������� �������������� ���� � �������
    size_t          viewer_hash;        //��� ���������� �����������
    size_t          object_hash;        //��� ���������� �������
    size_t          rasterization_hash; //��� ���������� ������������
    size_t          material_hash;      //��� ���������� ���������
    size_t          lighting_hash;      //��� ���������� ���������
    size_t          texmaps_hash;       //��� ���������� ���������������
    size_t          modes_hash;         //��� ������� ������������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� fixed-pipeline-program ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FppShaderManager : virtual public IShaderManager, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FppShaderManager (const ContextManager& manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetProfilesCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetProfile (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IShader*          CreateShader  (const ShaderDesc& shader_desc, const LogFunction& error_log);
    ICompiledProgram* CreateProgram (size_t shaders_count, IShader** shaders, const LogFunction& error_log);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
void identity_matrix  (Matrix4f); //������������� ��������� �������
void mult_matrix      (const Matrix4f a, const Matrix4f b, Matrix4f result); //result = a * b
void transpose_matrix (const Matrix4f source, Matrix4f result); //���������������� �������

}

}

}

#endif
