#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::mid_level;

//TODO: регистр: высокий или низкий?
//TODO: template programs

namespace
{

///Загрузчик эффектов
class EffectLoader
{
  public:
///Конструктор  
    EffectLoader (const DeviceManagerPtr& in_device_manager, const char* file_name)
      : parser (file_name, "wxf")
      , device_manager (in_device_manager)
    {
      try
      {
        ParseRoot (parser.Root ());
      }
      catch (...)
      {
        const common::ParseLog& parse_log = parser.Log ();

        try
        {  
          for (size_t i=0; i<parse_log.MessagesCount (); ++i)
            log.Print (parse_log.Message (i));
        }
        catch (...)
        {
          //подавление всех исключений
        }
        
        throw;
      }
    }
    
  private:
///Разбор корня
    void ParseRoot (Parser::Iterator iter)
    {
      for_each_child (*iter, "BlendState",        xtl::bind (&EffectLoader::ParseBlendState, this, _1));
      for_each_child (*iter, "DepthStencilState", xtl::bind (&EffectLoader::ParseDepthStencilState, this, _1));
      for_each_child (*iter, "RasterizerState",   xtl::bind (&EffectLoader::ParseRasterizerState, this, _1));      
      for_each_child (*iter, "SamplerState",      xtl::bind (&EffectLoader::ParseSamplerState, this, _1));
      for_each_child (*iter, "ShaderLibrary",     xtl::bind (&EffectLoader::ParseShaderLibrary, this, _1));
      for_each_child (*iter, "Program",           xtl::bind (&EffectLoader::ParseProgram, this, _1));
      for_each_child (*iter, "Template",          xtl::bind (&EffectLoader::ParseTemplateProgram, this, _1));
    }
    
///Разбор аргумента операции смешивания цветов
    static BlendArgument ParseBlendArgument (const ParseNode& node, const char* value)
    {
      struct Tag2Value
      {
        const char*   tag;
        BlendArgument value;
      };
      
      static Tag2Value values [] = {
        {"Zero",                    BlendArgument_Zero},
        {"One",                     BlendArgument_One},
        {"SourceColor",             BlendArgument_SourceColor},
        {"SourceAlpha",             BlendArgument_SourceAlpha},
        {"InverseSourceColor",      BlendArgument_InverseSourceColor},
        {"InverseSourceAlpha",      BlendArgument_InverseSourceAlpha},
        {"DestinationColor",        BlendArgument_DestinationColor},
        {"DestinationAlpha",        BlendArgument_DestinationAlpha},
        {"InverseDestinationColor", BlendArgument_InverseDestinationColor},
        {"InverseDestinationAlpha", BlendArgument_InverseDestinationAlpha},        
      };
      
      static const size_t values_count = sizeof (values) / sizeof (*values);
      
      for (size_t i=0; i<values_count; i++)
        if (!strcmp (values [i].tag, value))
          return values [i].value;
      
      raise_parser_exception (node, "Unexpected blend argument '%s'", value);
      
      return BlendArgument_Zero;
    }
    
///Разбор операции смешивания
    void ParseBlendOperation (const ParseNode& node, Parser::AttributeIterator iter, BlendOperation& operation, BlendArgument& src_arg, BlendArgument& dst_arg)
    {
      const char* operation_tag = get<const char*> (iter);
      const char* src_arg_tag   = get<const char*> (iter);
      const char* dst_arg_tag   = get<const char*> (iter);
      
      if      (!xtl::xstricmp (operation_tag, "Add"))                operation = BlendOperation_Add;
      else if (!xtl::xstricmp (operation_tag, "Subtraction"))        operation = BlendOperation_Subtraction;
      else if (!xtl::xstricmp (operation_tag, "ReverseSubtraction")) operation = BlendOperation_ReverseSubtraction;
      else if (!xtl::xstricmp (operation_tag, "Min"))                operation = BlendOperation_Min;
      else if (!xtl::xstricmp (operation_tag, "Max"))                operation = BlendOperation_Max;
      else
      {
        raise_parser_exception (node, "Bad blend operation '%s'", operation_tag);      
      }
      
      src_arg = ParseBlendArgument (node, src_arg_tag);
      dst_arg = ParseBlendArgument (node, dst_arg_tag);      
    }
    
///Разбор состояния уровня смешивания цветов
    void ParseBlendState (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      BlendDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      desc.blend_enable                     = xtl::xstrcmp (get<const char*> (*iter, "Enable",                 "true"), "true") == 0;
      desc.sample_alpha_to_coverage         = xtl::xstrcmp (get<const char*> (*iter, "SampleAlphaToCoverage",  "false"), "true") == 0;
      desc.blend_color_operation            = BlendOperation_Add;
      desc.blend_color_source_argument      = BlendArgument_One;
      desc.blend_color_destination_argument = BlendArgument_Zero;
      desc.blend_alpha_operation            = BlendOperation_Add;
      desc.blend_alpha_source_argument      = BlendArgument_One;
      desc.blend_alpha_destination_argument = BlendArgument_Zero;
      desc.color_write_mask                 = 0;
      
      const char* color_mask = get<const char*> (*iter, "ColorWriteMask", "rgba");
      
      if (strchr (color_mask, 'R')) desc.color_write_mask |= ColorWriteFlag_Red;
      if (strchr (color_mask, 'G')) desc.color_write_mask |= ColorWriteFlag_Green;
      if (strchr (color_mask, 'B')) desc.color_write_mask |= ColorWriteFlag_Blue;
      if (strchr (color_mask, 'A')) desc.color_write_mask |= ColorWriteFlag_Alpha;
      
      if (ParseNode color_node = iter->First ("BlendColor"))
      {
        Parser::AttributeIterator attr_iter = make_attribute_iterator (color_node);
        
        ParseBlendOperation (*iter, attr_iter, desc.blend_color_operation, desc.blend_color_source_argument, desc.blend_color_destination_argument);
      }
      
      if (ParseNode alpha_node = iter->First ("BlendAlpha"))
      {
        Parser::AttributeIterator attr_iter = make_attribute_iterator (alpha_node);
        
        ParseBlendOperation (*iter, attr_iter, desc.blend_alpha_operation, desc.blend_alpha_source_argument, desc.blend_alpha_destination_argument);
      }      

      try
      {
        LowLevelBlendStatePtr state (device_manager->Device ().CreateBlendState (desc), false);
      
        library.BlendStates ().Add (id, state);
      }
      catch (std::exception& e)
      {
        parser.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }
    
///Разбор режима сравнения
    static CompareMode ParseCompareMode (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "");
      
      struct Tag2Value
      {
        const char* tag;
        CompareMode value;
      };
      
      static Tag2Value values [] = {
        {"AlwaysFail",   CompareMode_AlwaysFail},
        {"AlwaysPass",   CompareMode_AlwaysPass},
        {"Equal",        CompareMode_Equal},
        {"NotEqual",     CompareMode_NotEqual},
        {"Less",         CompareMode_Less},
        {"LessEqual",    CompareMode_LessEqual},
        {"Greater",      CompareMode_Greater},
        {"GreaterEqual", CompareMode_GreaterEqual},        
      };
      
      static const size_t values_count = sizeof (values) / sizeof (*values);
      
      for (size_t i=0; i<values_count; i++)
        if (!strcmp (values [i].tag, value))
          return values [i].value;

      raise_parser_exception (node, "Unexpected compare mode '%s'", value);
      
      return CompareMode_AlwaysPass;
    }
    
///Разбор операции стенсила
    static StencilOperation ParseStencilOperation (const ParseNode& node, const char* child_name)
    {
      ParseNode child = node.First (child_name);
      
      if (!child)
        return StencilOperation_Keep;
        
      const char* value = get<const char*> (child, "");
      
      struct Tag2Value
      {
        const char*      tag;
        StencilOperation value;
      };
      
      static Tag2Value values [] = {
        {"Keep",      StencilOperation_Keep},
        {"Zero",      StencilOperation_Zero},
        {"Replace",   StencilOperation_Replace},
        {"Increment", StencilOperation_Increment},
        {"Decrement", StencilOperation_Decrement},
        {"Invert",    StencilOperation_Invert},
      };
      
      static const size_t values_count = sizeof (values) / sizeof (*values);
      
      for (size_t i=0; i<values_count; i++)
        if (!strcmp (values [i].tag, value))
          return values [i].value;

      raise_parser_exception (child, "Unexpected stencil operation '%s'", value);
      
      return StencilOperation_Keep;        
    }
    
///Разбор состояния уровня отсечения
    void ParseDepthStencilState (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      DepthStencilDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      desc.depth_test_enable   = xtl::xstrcmp (get<const char*> (*iter, "DepthTest",   "true"), "true") == 0;
      desc.depth_write_enable  = xtl::xstrcmp (get<const char*> (*iter, "DepthWrite",  "true"), "true") == 0;
      desc.stencil_test_enable = xtl::xstrcmp (get<const char*> (*iter, "StencilTest", "false"), "true") == 0;
      desc.depth_compare_mode  = iter->First ("DepthCompareMode") ? ParseCompareMode (iter->First ("DepthCompareMode")) : CompareMode_Less;
      desc.stencil_read_mask   = get<size_t> (*iter, "StencilReadMask", 0u);
      desc.stencil_write_mask  = get<size_t> (*iter, "StencilWriteMask", 0u);
      
      for (Parser::NamesakeIterator stencil_iter=iter->First ("Stencil"); stencil_iter; ++stencil_iter)
      {
        const char* face       = get<const char*> (*stencil_iter, "");        
        size_t      face_index = 0;
        
        if      (!xtl::xstrcmp (face, "Front") || !xtl::xstrcmp (face, "FrontAndBack")) face_index = FaceMode_Front;
        else if (!xtl::xstrcmp (face, "Back"))                                          face_index = FaceMode_Back;
        
        StencilDesc& stencil_desc = desc.stencil_desc [face_index];
        
          //разбор параметров стенсила
          
        stencil_desc.stencil_compare_mode   = stencil_iter->First ("StencilCompareMode") ? ParseCompareMode (stencil_iter->First ("StencilCompareMode")) : CompareMode_AlwaysPass;
        stencil_desc.stencil_fail_operation = ParseStencilOperation (*stencil_iter, "StencilFailOperation");
        stencil_desc.depth_fail_operation   = ParseStencilOperation (*stencil_iter, "DepthFailOperation");
        stencil_desc.stencil_pass_operation = ParseStencilOperation (*stencil_iter, "StencilPassOperation");        
          
          //дублирование данных
          
        if (!xtl::xstrcmp (face, "FrontAndBack"))
          desc.stencil_desc [FaceMode_Back] = desc.stencil_desc [FaceMode_Front];
      }
      
      try
      {
        LowLevelDepthStencilStatePtr state (device_manager->Device ().CreateDepthStencilState (desc), false);
      
        library.DepthStencilStates ().Add (id, state);
      }
      catch (std::exception& e)
      {
        parser.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }
    
///Разбор режима отображения граней
    static FillMode ParseFillMode (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "Solid");
      
      if (!xtl::xstrcmp (value, "Solid"))     return FillMode_Solid;
      if (!xtl::xstrcmp (value, "Wireframe")) return FillMode_Wireframe;
      
      raise_parser_exception (node, "Unexpected fill mode '%s'", value);
      
      return FillMode_Solid;
    }
    
///Разбор режима отсечения граней
    static CullMode ParseCullMode (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "None");
      
      if (!xtl::xstrcmp (value, "None"))  return CullMode_None;
      if (!xtl::xstrcmp (value, "Back"))  return CullMode_Back;
      if (!xtl::xstrcmp (value, "Front")) return CullMode_Front;
      
      raise_parser_exception (node, "Unexpected cull mode '%s'", value);
      
      return CullMode_None;
    }
    
///Разбор состояния уровня растеризации
    void ParseRasterizerState (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      RasterizerDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      desc.fill_mode               = ParseFillMode (iter->First ("FillMode"));
      desc.cull_mode               = ParseCullMode (iter->First ("CullMode"));
      desc.front_counter_clockwise = xtl::xstrcmp (get<const char*> (*iter, "FrontCounterClockwise", "true"), "true") == 0;
      desc.depth_bias              = get<int> (*iter, "DepthBias", 0);
      desc.scissor_enable          = xtl::xstrcmp (get<const char*> (*iter, "ScissorEnable", "false"), "true") == 0;
      desc.multisample_enable      = xtl::xstrcmp (get<const char*> (*iter, "MultisampleEnable", "false"), "true") == 0;
      desc.antialiased_line_enable = xtl::xstrcmp (get<const char*> (*iter, "AntialiasedLineEnable", "false"), "true") == 0;      

      try
      {
        LowLevelRasterizerStatePtr state (device_manager->Device ().CreateRasterizerState (desc), false);
      
        library.RasterizerStates ().Add (id, state);
      }
      catch (std::exception& e)
      {
        parser.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }
    
///Разбор фильтра минимизации текстур
    static TexMinFilter ParseTexMinFilter (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "Default");
      
      struct Tag2Value
      {
        const char*  tag;
        TexMinFilter value;
      };
      
      static Tag2Value values [] = {
        {"Default",          TexMinFilter_Default},
        {"Point",            TexMinFilter_Point},
        {"Linear",           TexMinFilter_Linear},
        {"PointMipPoint",    TexMinFilter_PointMipPoint},
        {"LinearMipPoint",   TexMinFilter_LinearMipPoint},
        {"PointMipLinear",   TexMinFilter_PointMipLinear},
        {"LinearMipLinear",  TexMinFilter_LinearMipLinear}
      };

      static const size_t values_count = sizeof (values) / sizeof (*values);
      
      for (size_t i=0; i<values_count; i++)
        if (!strcmp (values [i].tag, value))
          return values [i].value;

      raise_parser_exception (node, "Unexpected texture min filter '%s'", value);
      
      return TexMinFilter_Default;
    }
    
///Разбор фильтра максимизации текстур
    static TexMagFilter ParseTexMagFilter (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "Default");

      if (!xtl::xstrcmp (value, "Default")) return TexMagFilter_Default;
      if (!xtl::xstrcmp (value, "Point"))   return TexMagFilter_Point;
      if (!xtl::xstrcmp (value, "Linear"))  return TexMagFilter_Linear;

      raise_parser_exception (node, "Unexpected texture mag filter '%s'", value);

      return TexMagFilter_Default;      
    }
    
///Разбор свёртки текстурных координат
    static TexcoordWrap ParseTexcoordWrap (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "Repeat");
      
      if (!xtl::xstrcmp (value, "Repeat"))        return TexcoordWrap_Repeat;
      if (!xtl::xstrcmp (value, "Mirror"))        return TexcoordWrap_Mirror;
      if (!xtl::xstrcmp (value, "Clamp"))         return TexcoordWrap_Clamp;
      if (!xtl::xstrcmp (value, "ClampToBorder")) return TexcoordWrap_ClampToBorder;
      
      raise_parser_exception (node, "Unexpected texcoord wrap mode '%s'", value);
      
      return TexcoordWrap_Repeat;
    }

///Разбор состояния сэмплера
    void ParseSamplerState (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      SamplerDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      desc.min_filter           = ParseTexMinFilter (iter->First ("MinFilter"));
      desc.mag_filter           = ParseTexMagFilter (iter->First ("MagFilter"));
      desc.wrap_u               = ParseTexcoordWrap (iter->First ("WrapU"));
      desc.wrap_v               = ParseTexcoordWrap (iter->First ("WrapV"));
      desc.wrap_w               = ParseTexcoordWrap (iter->First ("WrapW"));
      desc.comparision_function = iter->First ("ComparisionFunction") ? ParseCompareMode (iter->First ("ComparisionFunction")) : CompareMode_AlwaysPass;
      desc.mip_lod_bias         = get<float> (*iter, "MipLodBias", 0.0f);
      desc.min_lod              = get<float> (*iter, "MinLod", 0.0f);
      desc.max_lod              = get<float> (*iter, "MaxLod", 0.0f);
      desc.max_anisotropy       = get<size_t> (*iter, "MaxAnisotropy", 1u);
      
      math::vec4f border_color = get<math::vec4f> (*iter, "BorderColor", math::vec4f (0.0f));
      
      for (size_t i=0; i<4; i++)
        desc.border_color [i] = border_color [i];

      try
      {
        LowLevelSamplerStatePtr state (device_manager->Device ().CreateSamplerState (desc), false);
      
        library.SamplerStates ().Add (id, state);
      }
      catch (std::exception& e)
      {
        parser.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }
    
///Лог-функтор загрузки шейдеров
    struct ShaderLoaderLog
    {
      ShaderLoaderLog (ParseNode& in_node, ParseLog& in_log) : node (in_node), log (in_log), node_printed (false) {}
      
      void operator () (const char* message)
      {
        if (!node_printed)
        {
          log.Error (node, "Shader library load failed:");
          
          node_printed = true;
        }
        
        log.Printf (ParseLogMessageType_Error, message);
      }
      
      ParseNode& node;
      ParseLog&  log;
      bool       node_printed;
    };
    
///Разбор библиотеки шейдеров
    void ParseShaderLibrary (Parser::Iterator iter)
    {
      const char* file_mask = get<const char*> (*iter, "");
      
      library.ShaderLibrary ().Load (file_mask, ShaderLoaderLog (*iter, parser.Log ()));
    }
    
///Разбор программы
    void ParseProgram (Parser::Iterator iter)
    {
      const char* id      = get<const char*> (*iter, "");
      const char* options = get<const char*> (*iter, "Options", "");
      
      static const size_t SHADERS_RESERVE_COUNT = 4;
      
      stl::vector<ShaderDesc> shaders;
      
      shaders.reserve (SHADERS_RESERVE_COUNT);
      
      for (Parser::NamesakeIterator shader_iter=iter->First ("Shader"); shader_iter; ++shader_iter)
      {
        ShaderDesc desc;
        
        memset (&desc, 0, sizeof (desc));
        
        Parser::AttributeIterator params_iter = make_attribute_iterator (*shader_iter);
        
        desc.name    = get<const char*> (params_iter);
        desc.profile = get<const char*> (params_iter);
        
        media::rfx::Shader* shader = library.ShaderLibrary ().Find (desc.name, desc.profile);
        
        if (!shader)
          raise_parser_exception (*shader_iter, "Shader '%s' for profile '%s' not found", desc.name, desc.profile);
          
        desc.source_code_size = shader->SourceCodeSize ();
        desc.source_code      = shader->SourceCode ();
        desc.options          = options;
        
        shaders.push_back (desc);
      }
      
      if (shaders.empty ())
        raise_parser_exception (*iter, "No shaders found for program");
      
      try
      {
        LowLevelProgramPtr program (device_manager->Device ().CreateProgram (shaders.size (), &shaders [0], ShaderLoaderLog (*iter, parser.Log ())), false);
      
        library.Programs ().Add (id, program);
      }
      catch (std::exception& e)
      {
        parser.Log ().Error (*iter, "%s", e.what ());
        throw;
      }            
    }    
    
///Разбор шаблонной программы
    void ParseTemplateProgram (Parser::Iterator iter)
    {
      /*
        template
        {
          option ".bump" "#define HAS_BUMP"
          
          program "my"
          {
            shader "my.vert" "glsl.vs"
            shader "my.frag" "glsl.ps"
          }
        }
      */
    }
  
  private:
    common::Parser         parser;         //парсер файла эффектов
    EffectLoaderLibrary    library;        //библиотека загрузки эффектов
    render::mid_level::Log log;            //протокол рендера
    DeviceManagerPtr       device_manager; //менеджер устройства отрисовки
};

}

namespace render
{

namespace mid_level
{

void parse_effect_library (const DeviceManagerPtr& device_manager, const char* file_name)
{
  EffectLoader loader (device_manager, file_name);
}

}

}
