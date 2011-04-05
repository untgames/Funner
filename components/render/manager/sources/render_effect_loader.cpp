#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render;

//TODO: templates for all blocks: shading, blend, depth_stencil, etc.

namespace
{

/*
    Константы
*/

const char* RFX_FILE_SUFFIX = ".rfx"; //суффикс имени файла описания библиотеки эффектов

/*
    Загрузчик эффектов
*/

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
      for_each_child (*iter, "blend",         xtl::bind (&EffectLoader::ParseBlendState, this, _1));
      for_each_child (*iter, "depth_stencil", xtl::bind (&EffectLoader::ParseDepthStencilState, this, _1));
      for_each_child (*iter, "rasterizer",    xtl::bind (&EffectLoader::ParseRasterizerState, this, _1));
      for_each_child (*iter, "sampler",       xtl::bind (&EffectLoader::ParseSamplerState, this, _1));
      for_each_child (*iter, "library",       xtl::bind (&EffectLoader::ParseShaderLibrary, this, _1));
      for_each_child (*iter, "program",       xtl::bind (&EffectLoader::ParseProgram, this, _1));
      for_each_child (*iter, "pass",          xtl::bind (&EffectLoader::ParseNamedPass, this, _1));
      for_each_child (*iter, "effect",        xtl::bind (&EffectLoader::ParseNamedEffect, this, _1));
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
        {"zero",           BlendArgument_Zero},
        {"0",              BlendArgument_Zero},        
        {"one",            BlendArgument_One},
        {"1",              BlendArgument_One},
        {"src_color",      BlendArgument_SourceColor},
        {"src_alpha",      BlendArgument_SourceAlpha},
        {"inv_src_color",  BlendArgument_InverseSourceColor},
        {"inv_src_alpha",  BlendArgument_InverseSourceAlpha},
        {"dst_color",      BlendArgument_DestinationColor},
        {"dst_alpha",      BlendArgument_DestinationAlpha},
        {"inv_dst_color",  BlendArgument_InverseDestinationColor},
        {"inv_dst_alpha",  BlendArgument_InverseDestinationAlpha},        
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
      
      if      (!xtl::xstricmp (operation_tag, "add"))    operation = BlendOperation_Add;
      else if (!xtl::xstricmp (operation_tag, "sub"))    operation = BlendOperation_Subtraction;
      else if (!xtl::xstricmp (operation_tag, "invsub")) operation = BlendOperation_ReverseSubtraction;
      else if (!xtl::xstricmp (operation_tag, "min"))    operation = BlendOperation_Min;
      else if (!xtl::xstricmp (operation_tag, "max"))    operation = BlendOperation_Max;
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
      
      desc.blend_enable                     = xtl::xstrcmp (get<const char*> (*iter, "blend_enable"               "true"), "true") == 0;
      desc.sample_alpha_to_coverage         = xtl::xstrcmp (get<const char*> (*iter, "sample_alpha_to_coverage",  "false"), "true") == 0;
      desc.blend_color_operation            = BlendOperation_Add;
      desc.blend_color_source_argument      = BlendArgument_One;
      desc.blend_color_destination_argument = BlendArgument_Zero;
      desc.blend_alpha_operation            = BlendOperation_Add;
      desc.blend_alpha_source_argument      = BlendArgument_One;
      desc.blend_alpha_destination_argument = BlendArgument_Zero;
      desc.color_write_mask                 = 0;
      
      const char* color_mask = get<const char*> (*iter, "color_write_mask", "rgba");
      
      if (strchr (color_mask, 'r')) desc.color_write_mask |= ColorWriteFlag_Red;
      if (strchr (color_mask, 'g')) desc.color_write_mask |= ColorWriteFlag_Green;
      if (strchr (color_mask, 'b')) desc.color_write_mask |= ColorWriteFlag_Blue;
      if (strchr (color_mask, 'a')) desc.color_write_mask |= ColorWriteFlag_Alpha;
      
      if (ParseNode color_node = iter->First ("blend_color"))
      {
        Parser::AttributeIterator attr_iter = make_attribute_iterator (color_node);
        
        ParseBlendOperation (*iter, attr_iter, desc.blend_color_operation, desc.blend_color_source_argument, desc.blend_color_destination_argument);
      }
      
      if (ParseNode alpha_node = iter->First ("blend_alpha"))
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
        {"always_fail",   CompareMode_AlwaysFail},
        {"always_pass",   CompareMode_AlwaysPass},
        {"equal",         CompareMode_Equal},
        {"not_equal",     CompareMode_NotEqual},
        {"less",          CompareMode_Less},
        {"less_equal",    CompareMode_LessEqual},
        {"greater",       CompareMode_Greater},
        {"great_erequal", CompareMode_GreaterEqual},        
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
        {"keep",      StencilOperation_Keep},
        {"zero",      StencilOperation_Zero},
        {"0",         StencilOperation_Zero},        
        {"replace",   StencilOperation_Replace},
        {"increment", StencilOperation_Increment},
        {"inc",       StencilOperation_Increment},        
        {"decrement", StencilOperation_Decrement},
        {"dec",       StencilOperation_Decrement},        
        {"invert",    StencilOperation_Invert},
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
      
      desc.depth_test_enable   = xtl::xstrcmp (get<const char*> (*iter, "depth_test",   "true"), "true") == 0;
      desc.depth_write_enable  = xtl::xstrcmp (get<const char*> (*iter, "depth_write",  "true"), "true") == 0;
      desc.stencil_test_enable = xtl::xstrcmp (get<const char*> (*iter, "stencil_test", "false"), "true") == 0;
      desc.depth_compare_mode  = iter->First ("depth_compare_mode") ? ParseCompareMode (iter->First ("depth_compare_mode")) : CompareMode_Less;
      desc.stencil_read_mask   = get<size_t> (*iter, "stencil_read_mask", 0u);
      desc.stencil_write_mask  = get<size_t> (*iter, "stencil_write_mask", 0u);
      
      for (Parser::NamesakeIterator stencil_iter=iter->First ("Stencil"); stencil_iter; ++stencil_iter)
      {
        const char* face       = get<const char*> (*stencil_iter, "");        
        size_t      face_index = 0;
        
        if      (!xtl::xstrcmp (face, "front") || !xtl::xstrcmp (face, "front_and_back")) face_index = FaceMode_Front;
        else if (!xtl::xstrcmp (face, "back"))                                            face_index = FaceMode_Back;
        
        StencilDesc& stencil_desc = desc.stencil_desc [face_index];
        
          //разбор параметров стенсила
          
        stencil_desc.stencil_compare_mode   = stencil_iter->First ("stencil_compare_mode") ? ParseCompareMode (stencil_iter->First ("StencilCompareMode")) : CompareMode_AlwaysPass;
        stencil_desc.stencil_fail_operation = ParseStencilOperation (*stencil_iter, "stencil_fail_operation");
        stencil_desc.depth_fail_operation   = ParseStencilOperation (*stencil_iter, "depth_fail_operation");
        stencil_desc.stencil_pass_operation = ParseStencilOperation (*stencil_iter, "stencil_pass_operation");        
          
          //дублирование данных
          
        if (!xtl::xstrcmp (face, "front_and_back"))
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
      const char* value = get<const char*> (node, "", "solid");
      
      if (!xtl::xstrcmp (value, "solid"))     return FillMode_Solid;
      if (!xtl::xstrcmp (value, "wireframe")) return FillMode_Wireframe;
      
      raise_parser_exception (node, "Unexpected fill mode '%s'", value);
      
      return FillMode_Solid;
    }
    
///Разбор режима отсечения граней
    static CullMode ParseCullMode (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "none");
      
      if (!xtl::xstrcmp (value, "none"))  return CullMode_None;
      if (!xtl::xstrcmp (value, "back"))  return CullMode_Back;
      if (!xtl::xstrcmp (value, "front")) return CullMode_Front;
      
      raise_parser_exception (node, "Unexpected cull mode '%s'", value);
      
      return CullMode_None;
    }
    
///Разбор состояния уровня растеризации
    void ParseRasterizerState (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      RasterizerDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      desc.fill_mode               = ParseFillMode (iter->First ("fill_mode"));
      desc.cull_mode               = ParseCullMode (iter->First ("cull_mode"));
      desc.front_counter_clockwise = xtl::xstrcmp (get<const char*> (*iter, "front_counter_clockwise", "true"), "true") == 0;
      desc.depth_bias              = get<int> (*iter, "depth_bias", 0);
      desc.scissor_enable          = false;
      desc.multisample_enable      = xtl::xstrcmp (get<const char*> (*iter, "multisample_enable", "false"), "true") == 0;
      desc.antialiased_line_enable = xtl::xstrcmp (get<const char*> (*iter, "antialiased_line_enable", "false"), "true") == 0;      

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
        {"default",            TexMinFilter_Default},
        {"point",              TexMinFilter_Point},
        {"linear",             TexMinFilter_Linear},
        {"point_mip_point",    TexMinFilter_PointMipPoint},
        {"linear_mip_point",   TexMinFilter_LinearMipPoint},
        {"point_mip_linear",   TexMinFilter_PointMipLinear},
        {"linear_mip_linear",  TexMinFilter_LinearMipLinear}
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
      const char* value = get<const char*> (node, "", "default");

      if (!xtl::xstrcmp (value, "default")) return TexMagFilter_Default;
      if (!xtl::xstrcmp (value, "point"))   return TexMagFilter_Point;
      if (!xtl::xstrcmp (value, "linear"))  return TexMagFilter_Linear;

      raise_parser_exception (node, "Unexpected texture mag filter '%s'", value);

      return TexMagFilter_Default;      
    }
    
///Разбор свёртки текстурных координат
    static TexcoordWrap ParseTexcoordWrap (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "repeat");
      
      if (!xtl::xstrcmp (value, "repeat"))          return TexcoordWrap_Repeat;
      if (!xtl::xstrcmp (value, "mirror"))          return TexcoordWrap_Mirror;
      if (!xtl::xstrcmp (value, "clamp"))           return TexcoordWrap_Clamp;
      if (!xtl::xstrcmp (value, "clamp_to_border")) return TexcoordWrap_ClampToBorder;
      
      raise_parser_exception (node, "Unexpected texcoord wrap mode '%s'", value);
      
      return TexcoordWrap_Repeat;
    }

///Разбор состояния сэмплера
    void ParseSamplerState (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      SamplerDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      desc.min_filter           = ParseTexMinFilter (iter->First ("min_filter"));
      desc.mag_filter           = ParseTexMagFilter (iter->First ("mag_filter"));
      desc.wrap_u               = ParseTexcoordWrap (iter->First ("wrap_u"));
      desc.wrap_v               = ParseTexcoordWrap (iter->First ("wrap_v"));
      desc.wrap_w               = ParseTexcoordWrap (iter->First ("wrap_w"));
      desc.comparision_function = iter->First ("comparision_function") ? ParseCompareMode (iter->First ("comparision_function")) : CompareMode_AlwaysPass;
      desc.mip_lod_bias         = get<float> (*iter, "mip_lod_bias", 0.0f);
      desc.min_lod              = get<float> (*iter, "min_lod", 0.0f);
      desc.max_lod              = get<float> (*iter, "max_lod", 0.0f);
      desc.max_anisotropy       = get<size_t> (*iter, "max_anisotropy", 1u);
      
      math::vec4f border_color = get<math::vec4f> (*iter, "border_color", math::vec4f (0.0f));
      
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
      
      library.Shaders ().Load (file_mask, ShaderLoaderLog (*iter, parser.Log ()));
    }
    
///Разбор настроек шэйдинга
    void ParseProgram (Parser::Iterator iter)
    {
      const char* id      = get<const char*> (*iter, "");
      const char* options = get<const char*> (*iter, "options", "");
      
      static const size_t SHADERS_RESERVE_COUNT = 4;
      
      stl::vector<ShaderDesc> shaders;
      
      shaders.reserve (SHADERS_RESERVE_COUNT);
      
      for (Parser::NamesakeIterator shader_iter=iter->First ("shader"); shader_iter; ++shader_iter)
      {
        ShaderDesc desc;
        
        memset (&desc, 0, sizeof (desc));
        
        Parser::AttributeIterator params_iter = make_attribute_iterator (*shader_iter);
        
        desc.name    = get<const char*> (params_iter);
        desc.profile = get<const char*> (params_iter);
        
        media::rfx::Shader* shader = library.Shaders ().Find (desc.name, desc.profile);
        
        if (!shader)
          raise_parser_exception (*shader_iter, "Shader '%s' for profile '%s' not found", desc.name, desc.profile);
          
        desc.source_code_size = shader->SourceCodeSize ();
        desc.source_code      = shader->SourceCode ();
        desc.options          = options;
        
        shaders.push_back (desc);
      }
      
      if (shaders.empty ())
        raise_parser_exception (*iter, "No shaders found for shading");
      
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
    
///Разбор списка тэгов
    common::StringArray ParseTags (const ParseNode& node, const char* node_name, bool non_empty_requirement = true)
    {
      Parser::Iterator tags_node = node.First (node_name);

      if (!tags_node && non_empty_requirement)
        raise_parser_exception (node, "Node 'tags' not found in description");

      Parser::AttributeIterator tags_attr_iter = make_attribute_iterator (*tags_node);

      common::StringArray tags;

      for (;tags_attr_iter; ++tags_attr_iter)
      {
        const char* tag = *tags_attr_iter;

        if (strchr (tag, ' ')) tags.Add (common::split (tag, " "));
        else                   tags.Add (tag);
      }
      
      return tags;
    }
    
///Разбор прохода рендеринга
    static SortMode ParseSortMode (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "default");

      if (!xtl::xstrcmp (value, "default"))       return SortMode_Default;
      if (!xtl::xstrcmp (value, "states"))        return SortMode_StateSwitch;
      if (!xtl::xstrcmp (value, "back_to_front")) return SortMode_BackToFront;
      if (!xtl::xstrcmp (value, "front_to_back")) return SortMode_FrontToBack;
      
      raise_parser_exception (node, "Unexpected sort mode '%s'", value);
      
      return SortMode_Default;      
    }

    void ParseNamedPass (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      EffectPassPtr pass = ParsePass (iter);
      
      library.EffectPasses ().Add (id, pass);
    }
    
    EffectPassPtr ParsePass (Parser::Iterator iter)
    {
      try
      {
        common::StringArray tags                     = ParseTags (*iter, "tags");
        common::StringArray color_targets            = ParseTags (*iter, "color_targets", false);
        SortMode            sort_mode                = ParseSortMode (*iter);
        const char*         depth_stencil_target     = get<const char*> (*iter, "depth_stencil_target", "null");
        const char*         depth_stencil_state_name = get<const char*> (*iter, "depth_stencil_state", "");
        const char*         blend_state_name         = get<const char*> (*iter, "blend_state", "");
        const char*         rasterizer_state_name    = get<const char*> (*iter, "rasterizer_state", "");

        LowLevelBlendStatePtr        blend_state         = *blend_state_name ? library.BlendStates ().Find (blend_state_name) : LowLevelBlendStatePtr ();
        LowLevelDepthStencilStatePtr depth_stencil_state = *depth_stencil_state_name ? library.DepthStencilStates ().Find (depth_stencil_state_name) : LowLevelDepthStencilStatePtr ();
        LowLevelRasterizerStatePtr   rasterizer_state    = *rasterizer_state_name ? library.RasterizerStates ().Find (rasterizer_state_name) : LowLevelRasterizerStatePtr ();

        EffectPassPtr pass (new EffectPass (device_manager), false);

        pass->SetColorTargets       (color_targets);
        pass->SetDepthStencilTarget (depth_stencil_target);
        pass->SetTags               (tags);
        pass->SetSortMode           (sort_mode);
        pass->SetBlendState         (blend_state);
        pass->SetDepthStencilState  (depth_stencil_state);
        pass->SetRasterizerState    (rasterizer_state);

        return pass;
      }
      catch (std::exception& e)
      {
        parser.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }
    
///Разбор эффекта рендеринга
    void ParseNamedEffect (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      EffectPtr effect = ParseEffect (iter);
      
      library.Effects ().Add (id, effect);
    }
    
    EffectPassPtr ParseEffectPass (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "", "");
      
      if (iter->First () && *id)
        raise_parser_exception (*iter, "Can't use named pass with content inside effect definition");
        
      EffectPassPtr pass;
        
      if (*id)
      {
        pass = library.EffectPasses ().Find (id);
        
        if (!pass)
          raise_parser_exception (*iter, "Pass '%s' not found", id);
      }
      else
      {
        pass = ParsePass (iter);
      }

      return pass;
    }
    
    InstantiatedEffectPtr ParseEffectGroup (Parser::Iterator iter)
    {
      common::StringArray tags = ParseTags (*iter, "");
      
      InstantiatedEffectPtr effect (new InstantiatedEffect, false);
      
      effect->SetTags (tags);
      
      return effect;
    }
    
    EffectPtr ParseEffect (Parser::Iterator iter)
    {
      try
      {
        common::StringArray tags = ParseTags (*iter, "tags", false);
        
        EffectPtr effect (new Effect, false);
        
        effect->SetTags (tags);
        
        for (Parser::Iterator node_iter=iter->First (); node_iter; ++node_iter)
        {
          const char* id = node_iter->Name ();
          
          if      (!xtl::xstrcmp (id, "pass"))         effect->AddOperation (ParseEffectPass (node_iter));
          else if (!xtl::xstrcmp (id, "effect_group")) effect->AddOperation (ParseEffectGroup (node_iter));
        }

        return effect;
      }
      catch (std::exception& e)
      {
        parser.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }    
  
  private:
    common::Parser       parser;         //парсер файла эффектов
    EffectLoaderLibrary  library;        //библиотека загрузки эффектов
    render::Log          log;            //протокол рендера
    DeviceManagerPtr     device_manager; //менеджер устройства отрисовки
};

}

namespace render
{

void parse_effect_library (const DeviceManagerPtr& device_manager, const char* file_name)
{
  EffectLoader loader (device_manager, file_name);
}

bool is_effect_library_file (const char* file_name)
{
  if (!file_name)
    return false;
    
  return xtl::xstricmp (common::suffix (file_name).c_str (), RFX_FILE_SUFFIX) == 0;
}

}
