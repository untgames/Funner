#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    Константы
*/

const size_t FPP_DYNAMIC_PARAMETER_FIELDS_RESERVE_SIZE = 8; //резервируемое количество смещений динамических параметров

/*
    Пара: тэг - значение
*/

struct Tag2Value
{
  const char* tag;
  int         value;
};

/*
    Парсер FPP-шейдеров
*/

class FppShaderParser
{
  public:
    FppShaderParser (FppState& in_base_state, FppDynamicParameterMap& in_dynamic_parameters, const ShaderDesc& shader_desc, const LogFunction& error_log)
     : base_state (in_base_state),
       dynamic_parameters (in_dynamic_parameters),
       parser (parse_log, shader_desc.name, shader_desc.source_code, shader_desc.source_code_size, "wxf")
    {
        //разбор шейдера

      ParseShader ();

        //проверка ошибок разбора

      for (size_t i=0, count=parse_log.MessagesCount (); i<count; i++)
        error_log (parse_log.Message (i));
        
      if (parse_log.HasErrors ())
        Raise<ArgumentException> ("render::low_level::opengl::FppShaderParser::FppShaderParser", "Errors in shader '%s'", shader_desc.name);
    }
    
  private:
      //добавление динамического параметра
    void AddDynamicParameter (Parser::Iterator line_iter, const char* name, FppDynamicParameterType type, size_t count)
    {
      FppDynamicParameterMap::iterator iter = dynamic_parameters.find (name);

      if (iter != dynamic_parameters.end ())
      {
        parse_log.Error (line_iter, "Parameter '%s' has been already defined", name);
        return;
      }            

      FppDynamicParameter& param = dynamic_parameters.insert_pair (name, FppDynamicParameter ()).first->second;

      param.type   = type;
      param.count  = count;      

      param.field_offsets.reserve (FPP_DYNAMIC_PARAMETER_FIELDS_RESERVE_SIZE);
    }
    
      //добавление смещения на динамический параметр
    void AddDynamicParameterField (Parser::Iterator line_iter, const char* name, size_t offset, FppDynamicParameterType type, size_t count)
    {
      FppDynamicParameterMap::iterator iter = dynamic_parameters.find (name);

      if (iter == dynamic_parameters.end ())      
      {
        parse_log.Error (line_iter, "Undefined parameter '%s'", name);
        return;
      }
      
      FppDynamicParameter& param = iter->second;
      
      if (param.type != type)
      {
        parse_log.Error (line_iter, "Type of parameter '%s' mismatch", name);
        return;
      }
      
      if (param.count < count)      
      {
        parse_log.Error (line_iter, "Parameter '%s' attributes count %u is less than expected attributes count %u",
                         name, param.count, count);
        return;
      }
      
      param.field_offsets.push_back (offset);
    }  
    
      //разбор значений
    template <class T, FppDynamicParameterType Type>
    void ParseValues (Parser::Iterator iter, const char* node_name, size_t offset, size_t count)
    {
      iter = iter->First (node_name);
      
      if (!iter)
        return;        

      size_t       attributes_count = iter->AttributesCount ();
      const char** attributes       = iter->Attributes ();      

      if (attributes_count == 1 && isalpha (*attributes [0]))
      {
          //добавление динамического параметра

        AddDynamicParameterField (iter, attributes [0], offset, Type, count);

        return;
      }

        //проверка корректности числа атрибутов

      if (attributes_count < count)
      {
        parse_log.Error (iter, "Too few attributes (%u attributes expected)", count);

        return;
      }

        //разбор атрибутов

      size_t parsed_attributes_count = read_range (xtl::io::make_token_iterator (attributes, attributes + count),
                                                   (T*)((char*)&base_state + offset));

      if (parsed_attributes_count != count)
        parse_log.Error (iter, "Error at read attribute #%u", parsed_attributes_count);
    }

      //разбор целочисленных значений
    void ParseIntegerValues (Parser::Iterator iter, const char* node_name, size_t offset, size_t count)
    {
      ParseValues<int, FppDynamicParameterType_Int> (iter, node_name, offset, count);
    }

      //разбор вещественных значений
    void ParseFloatValues (Parser::Iterator iter, const char* node_name, size_t offset, size_t count)
    {
      ParseValues<float, FppDynamicParameterType_Float> (iter, node_name, offset, count);
    }
    
      //разбор векторов
    void ParseVector4f (Parser::Iterator iter, const char* node_name, size_t offset)
    {
      ParseFloatValues (iter, node_name, offset, 4);
    }

    void ParseVector3f (Parser::Iterator iter, const char* node_name, size_t offset)
    {
      ParseFloatValues (iter, node_name, offset, 3);
    }

      //разбор матрицы
    void ParseMatrix4f (Parser::Iterator iter, const char* node_name, size_t offset)
    {
      ParseFloatValues (iter, node_name, offset, 16);
    }
    
      //разбор объявления параметров
    void ParseParameterDeclarations (Parser::Iterator params_iter, const char* name, FppDynamicParameterType type, size_t count)
    {
      for (Parser::NamesakeIterator iter=params_iter->First (name); iter; ++iter)
      {
        if (!iter->AttributesCount ())
        {
          parse_log.Error (iter, "Parameter names expected");
          continue;
        }

        for (size_t i=0; i<iter->AttributesCount (); i++)
          AddDynamicParameter (iter, iter->Attribute (i), type, count);
      }
    }
    
      //разбор строкового свойства
    void ParseEnum (Parser::Iterator iter, const char* node_name, size_t offset, const Tag2Value* pairs)
    {
      iter = iter->First (node_name);

      if (!iter)
        return;
        
      if (!iter->AttributesCount ())
      {
        parse_log.Error (iter, "Too few attributes (1 attribute expected)");
        return;
      }

        //поиск значения, соответствующего прочитанной строке

      const char* value = iter->Attribute (0);

      for (const Tag2Value* i=pairs; i->tag; i++)
        if (!string_wrappers::stricmp (i->tag, value))
        {
          *(int*)((char*)&base_state + offset) = i->value;

          return;
        }

        //если соответствие не найдено - сообщаем об ошибке

      parse_log.Error (iter, "Unknown value '%s'", value);
    }
    
      //разбор параметров источника освещения
    void ParseLight (Parser::Iterator light_iter, size_t base_offset)
    {
      static const Tag2Value light_types [] = {
        {"Point",  LightType_Point},
        {"Remote", LightType_Remote},
        {"Spot",   LightType_Spot},
        {0, 0}
      };
      
      ParseEnum          (light_iter, "Type",                 base_offset + offsetof (LightDesc, type), light_types);
      ParseIntegerValues (light_iter, "Enable",               base_offset + offsetof (LightDesc, enable), 1);
      ParseVector3f      (light_iter, "Position",             base_offset + offsetof (LightDesc, position));
      ParseVector3f      (light_iter, "Direction",            base_offset + offsetof (LightDesc, direction));
      ParseVector4f      (light_iter, "AmbientColor",         base_offset + offsetof (LightDesc, ambient_color));
      ParseVector4f      (light_iter, "DiffuseColor",         base_offset + offsetof (LightDesc, diffuse_color));
      ParseVector4f      (light_iter, "SpecularColor",        base_offset + offsetof (LightDesc, specular_color));
      ParseFloatValues   (light_iter, "Angle",                base_offset + offsetof (LightDesc, angle), 1);
      ParseFloatValues   (light_iter, "Exponent",             base_offset + offsetof (LightDesc, exponent), 1);
      ParseFloatValues   (light_iter, "ContantAttenuation",   base_offset + offsetof (LightDesc, constant_attenuation), 1);
      ParseFloatValues   (light_iter, "LinearAttenuation",    base_offset + offsetof (LightDesc, linear_attenuation), 1);
      ParseFloatValues   (light_iter, "QuadraticAttenuation", base_offset + offsetof (LightDesc, quadratic_attenuation), 1);
    }    
  
      //разбор параметров текстрирования
    void ParseTexmap (Parser::Iterator texmap_iter, size_t base_offset)
    {
      ParseMatrix4f (texmap_iter, "Transform", base_offset + offsetof (TexmapDesc, transform));
      ParseMatrix4f (texmap_iter, "Texgen",    base_offset + offsetof (TexmapDesc, transform));
      ParseVector4f (texmap_iter, "TexgenU",   base_offset + offsetof (TexmapDesc, transform [0]));
      ParseVector4f (texmap_iter, "TexgenV",   base_offset + offsetof (TexmapDesc, transform [1]));
      ParseVector4f (texmap_iter, "TexgenW",   base_offset + offsetof (TexmapDesc, transform [2]));
      
      static const Tag2Value texcoord_sources [] = {
        {"Explicit",      TexcoordSource_Explicit},
        {"SphereMap",     TexcoordSource_SphereMap},
        {"ReflectionMap", TexcoordSource_ReflectionMap},
        {"ObjectSpace",   TexcoordSource_ObjectSpace},
        {"ViewerSpace",   TexcoordSource_ViewerSpace},
        {0, 0}
      };

      ParseEnum (texmap_iter, "TexcoordU", base_offset + offsetof (TexmapDesc, source_u), texcoord_sources);
      ParseEnum (texmap_iter, "TexcoordV", base_offset + offsetof (TexmapDesc, source_v), texcoord_sources);
      ParseEnum (texmap_iter, "TexcoordW", base_offset + offsetof (TexmapDesc, source_w), texcoord_sources);
      
      static const Tag2Value texture_blend_modes [] = {
        {"Replace",  TextureBlend_Replace},
        {"Modulate", TextureBlend_Modulate},
        {"Blend",    TextureBlend_Blend},
        {0, 0}
      };
      
      ParseEnum (texmap_iter, "Blend", base_offset + offsetof (TexmapDesc, blend), texture_blend_modes);
    }
  
      //разбор шейдера
    void ParseShader ()
    {
      Parser::Iterator program_iter = parser.Root ();

      if (!program_iter)
        return;
        
        //разбор объявлений параметров
        
      Parser::Iterator params_iter = program_iter->First ("Parameters");
        
      if (params_iter)
      {        
        ParseParameterDeclarations (params_iter, "float",    FppDynamicParameterType_Float, 1);
        ParseParameterDeclarations (params_iter, "int",      FppDynamicParameterType_Int, 1);
        ParseParameterDeclarations (params_iter, "float3",   FppDynamicParameterType_Float, 3);
        ParseParameterDeclarations (params_iter, "int3",     FppDynamicParameterType_Int, 3);
        ParseParameterDeclarations (params_iter, "float4",   FppDynamicParameterType_Float, 4);
        ParseParameterDeclarations (params_iter, "int4",     FppDynamicParameterType_Int, 4);
        ParseParameterDeclarations (params_iter, "float4x4", FppDynamicParameterType_Float, 16);
        ParseParameterDeclarations (params_iter, "int4x4",   FppDynamicParameterType_Int, 16);

        if (params_iter->NextNamesake ())
          parse_log.Warning (params_iter->NextNamesake (), "Second (and others) 'Parameters' block(s) ignored");
      }

        //разбор параметров трансформации

      ParseMatrix4f (program_iter, "ProjectionMatrix", offsetof (FppState, projection_matrix));
      ParseMatrix4f (program_iter, "ViewMatrix", offsetof (FppState, view_matrix));
      ParseMatrix4f (program_iter, "ObjectMatrix", offsetof (FppState, object_matrix));
      
        //разбор параметров материала
      
      ParseVector4f    (program_iter, "EmissionColor", offsetof (FppState, emission_color));
      ParseVector4f    (program_iter, "AmbientColor",  offsetof (FppState, ambient_color));
      ParseVector4f    (program_iter, "DiffuseColor",  offsetof (FppState, diffuse_color));
      ParseVector4f    (program_iter, "SpecularColor", offsetof (FppState, specular_color));
      ParseFloatValues (program_iter, "Shininess",     offsetof (FppState, shininess), 1);
      
      static const Tag2Value color_material_modes [] = {
        {"Explicit",          ColorMaterial_Explicit},
        {"Emission",          ColorMaterial_Emission},
        {"Ambient",           ColorMaterial_Ambient},
        {"Diffuse",           ColorMaterial_Diffuse},
        {"Specular",          ColorMaterial_Specular},
        {"AmbientAndDiffuse", ColorMaterial_AmbientAndDiffuse},
        {0, 0}
      };

      ParseEnum (program_iter, "ColorMaterial", offsetof (FppState, color_material), color_material_modes);

      static const Tag2Value compare_modes [] = {
        {"AlwaysFail",   CompareMode_AlwaysFail},
        {"AlwaysPass",   CompareMode_AlwaysPass},
        {"Equal",        CompareMode_Equal},
        {"NotEqual",     CompareMode_NotEqual},
        {"Less",         CompareMode_Less},
        {"LessEqual",    CompareMode_LessEqual},
        {"Greater",      CompareMode_Greater},
        {"GreaterEqual", CompareMode_GreaterEqual},
        {0, 0}
      };

      ParseEnum        (program_iter, "AlphaCompareMode", offsetof (FppState, alpha_compare_mode), compare_modes);
      ParseFloatValues (program_iter, "AlphaReference",   offsetof (FppState, alpha_reference), 1);

        //разбор параметров освещения
      
      for (size_t i=0; i<FPP_MAX_LIGHTS_COUNT; i++)
      {
        char light_name [32];
        
        _snprintf (light_name, sizeof light_name, "Light%u", i);
        
        Parser::Iterator light_iter = program_iter->First (light_name);

        if (light_iter)
          ParseLight (light_iter, offsetof (FppState, lights [i]));
      }
      
        //разбор параметров текстурирования
        
      for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
      {
        char texmap_name [32];
        
        _snprintf (texmap_name, sizeof texmap_name, "Texmap%u", i);
        
        Parser::Iterator texmap_iter = program_iter->First (texmap_name);
        
        if (texmap_iter)
          ParseTexmap (texmap_iter, offsetof (FppState, maps [i]));
      }
    }    

  private:
    FppState&               base_state;         //базовое состояние fpp-шейдера
    FppDynamicParameterMap& dynamic_parameters; //динамические параметры
    ParseLog                parse_log;          //протокол парсера
    Parser                  parser;             //парсер исходного текста fpp-шейдера
};

}

/*
    Конструктор / деструктор
*/

namespace
{

void identity_matrix (Matrix4f& m)
{
  for (size_t i=0; i<4; i++)
    for (size_t j=0; j<4; j++)
      m [i][j] = i == j ? 1.0f : 0.0f;
}

}

FppShader::FppShader (const ShaderDesc& shader_desc, const LogFunction& error_log)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FppShader::FppShader";

    //инициализация состояния fpp-шейдера
    
  memset (&base_state, 0, sizeof base_state);
  
  identity_matrix (base_state.projection_matrix);
  identity_matrix (base_state.view_matrix);
  identity_matrix (base_state.object_matrix);
  
  for (size_t i=0; i<FPP_MAX_LIGHTS_COUNT; i++)
  {
    LightDesc& light = base_state.lights [i];
    
    light.constant_attenuation = 1.0f;
  }
  
  for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
  {
    TexmapDesc& texmap = base_state.maps [i];

    identity_matrix (texmap.transform);
    identity_matrix (texmap.texgen);
  }

    //разбор fpp-шейдера

  if (!shader_desc.profile)
    RaiseNullArgument (METHOD_NAME, "shader_desc.profile");

  if (!shader_desc.source_code)
    RaiseNullArgument (METHOD_NAME, "shader_desc.source_code");

  if (!shader_desc.name)
    RaiseNullArgument (METHOD_NAME, "shader_desc.name");    

  error_log (format ("Compiling %s...", shader_desc.name).c_str ());

  FppShaderParser (base_state, dynamic_parameters, shader_desc, error_log);
}

FppShader::~FppShader ()
{
}

/*
    Работа с динамическими параметрами
*/

const FppDynamicParameter* FppShader::FindDynamicParameter (const char* name) const
{
  if (!name)
    return 0;
    
  FppDynamicParameterMap::const_iterator iter = dynamic_parameters.find (name);
  
  if (iter == dynamic_parameters.end ())
    return 0;
    
  return &iter->second;
}
