#include "shared.h"

using namespace common;
using namespace render;
using namespace render::low_level;
using namespace render::manager;

namespace
{

/*
    Загрузчик эффектов
*/

class EffectLoader
{
  public:
///Конструктор  
    EffectLoader (const DeviceManagerPtr& in_device_manager, const char* name, const common::ParseNode& in_root, EffectLoaderLibrary& in_library)
      : root (in_root)
      , library (in_library)
      , device_manager (in_device_manager)
    {
      try
      {
        ParseRoot (root);

        if (root.Log ().HasErrors ())
          throw xtl::format_operation_exception ("render::manager::EffectLoader::EffectLoader", "Error at parsing effect file '%s'", name);
      }
      catch (...)
      {
        PrintLog ();
        throw;
      }

      PrintLog ();      
    }
    
  private:
///Печать лога
    void PrintLog ()
    {
      const common::ParseLog& parse_log = root.Log ();

      try
      {  
        for (size_t i=0; i<parse_log.MessagesCount (); ++i)
          log.Print (parse_log.Message (i));
      }
      catch (...)
      {
        //подавление всех исключений
      }      
    }
  
///Разбор корня
    void ParseRoot (Parser::Iterator iter)
    {
      for_each_child (*iter, "blend",         xtl::bind (&EffectLoader::ParseBlendState, this, _1));
      for_each_child (*iter, "depth_stencil", xtl::bind (&EffectLoader::ParseDepthStencilState, this, _1));
      for_each_child (*iter, "rasterizer",    xtl::bind (&EffectLoader::ParseRasterizerState, this, _1));
      for_each_child (*iter, "sampler",       xtl::bind (&EffectLoader::ParseSamplerState, this, _1));
      for_each_child (*iter, "texture",       xtl::bind (&EffectLoader::ParseTextureDesc, this, _1));      
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
      const char* operation_tag = xtl::io::get<const char*> (iter);
      const char* src_arg_tag   = xtl::io::get<const char*> (iter);
      const char* dst_arg_tag   = xtl::io::get<const char*> (iter);
      
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
    void ParseBlendTargetState (Parser::Iterator iter, RenderTargetBlendDesc& desc)
    {
      desc.blend_enable                     = xtl::xstrcmp (get<const char*> (*iter, "blend_enable", "true"), "true") == 0;
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
    }

    void ParseBlendState (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      BlendDesc desc;
      
      memset (&desc, 0, sizeof (desc));
      
      desc.sample_alpha_to_coverage = xtl::xstrcmp (get<const char*> (*iter, "sample_alpha_to_coverage",  "false"), "true") == 0;
      desc.independent_blend_enable = xtl::xstrcmp (get<const char*> (*iter, "independent_blend_enable",  "false"), "true") == 0;
  
      if (desc.independent_blend_enable)
      {
        for (Parser::NamesakeIterator rt_iter = iter->First ("color_target"); rt_iter; ++rt_iter)
	{
          Parser::AttributeIterator attr_iter = make_attribute_iterator (*rt_iter);

          size_t index = xtl::io::get<size_t> (attr_iter);

          if (index >= DEVICE_RENDER_TARGET_SLOTS_COUNT)
            raise_parser_exception (*rt_iter, "Unexpected blend render target index %u", index);

          ParseBlendTargetState (rt_iter, desc.render_target [index]);
        }
      }
      else
      {
        ParseBlendTargetState (iter, desc.render_target [0]);
      }

      try
      {
        LowLevelBlendStatePtr state (device_manager->Device ().CreateBlendState (desc), false);
      
        library.BlendStates ().Add (id, state);

        log.Printf ("Effect blend state '%s' loaded", id);                
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
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
        
        log.Printf ("Effect depth-stencil state '%s' loaded", id);        
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
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
        
        desc.scissor_enable = true;

        state = LowLevelRasterizerStatePtr (device_manager->Device ().CreateRasterizerState (desc), false);

        library.RasterizerScissorStates ().Add (id, state);

        log.Printf ("Effect rasterizer '%s' loaded", id);        
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }

///Разбор фильтра минимизации текстур
    static TexMinFilter ParseTexMinFilter (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "", "default");
      
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

///Разбор уровня детализацтии сэмплера
    static float ParseSamplerLod (const ParseNode& node, const char* tag)
    {
      const char* value = get<const char*> (node, tag, "0");

      if (!strcmp (value, "max"))
        return FLT_MAX;

      return get<float> (node, tag);
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
      desc.mip_lod_bias         = ParseSamplerLod (*iter, "mip_lod_bias");
      desc.min_lod              = ParseSamplerLod (*iter, "min_lod");
      desc.max_lod              = ParseSamplerLod (*iter, "max_lod");
      desc.max_anisotropy       = get<size_t> (*iter, "max_anisotropy", 1u);
      
      math::vec4f border_color = get<math::vec4f> (*iter, "border_color", math::vec4f (0.0f));
      
      for (size_t i=0; i<4; i++)
        desc.border_color [i] = border_color [i];

      try
      {
        LowLevelSamplerStatePtr state (device_manager->Device ().CreateSamplerState (desc), false);
      
        library.SamplerStates ().Add (id, state);
        
        log.Printf ("Effect texture sampler '%s' loaded", id);        
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }
    
///Разбор формата пикселей
    static low_level::PixelFormat ParsePixelFormat (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "");
      
      struct Tag2Value
      {
        const char*            tag;
        low_level::PixelFormat value;
      };
      
      static Tag2Value values [] = {
        {"rgb8",            low_level::PixelFormat_RGB8},
        {"rgba8",           low_level::PixelFormat_RGBA8},
        {"l8",              low_level::PixelFormat_L8},
        {"a8",              low_level::PixelFormat_A8},
        {"la8",             low_level::PixelFormat_LA8},
        {"dxt1",            low_level::PixelFormat_DXT1},
        {"dxt3",            low_level::PixelFormat_DXT3},
        {"dxt5",            low_level::PixelFormat_DXT5},
        {"rgb_pvrtc2",      low_level::PixelFormat_RGB_PVRTC2},
        {"rgb_pvrtc4",      low_level::PixelFormat_RGB_PVRTC4},
        {"rgba_pvrtc2",     low_level::PixelFormat_RGBA_PVRTC2},
        {"rgba_pvrtc4",     low_level::PixelFormat_RGBA_PVRTC4},
        {"atc",             low_level::PixelFormat_ATC_RGB_AMD},
        {"atci",            low_level::PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD},
        {"atca",            low_level::PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD},
        {"d16",             low_level::PixelFormat_D16},
        {"d24x8",           low_level::PixelFormat_D24X8},
        {"d24s8",           low_level::PixelFormat_D24S8},
        {"d32",             low_level::PixelFormat_D32},
        {"s8",              low_level::PixelFormat_S8},
      };

      static const size_t values_count = sizeof (values) / sizeof (*values);
      
      for (size_t i=0; i<values_count; i++)
        if (!strcmp (values [i].tag, value))
          return values [i].value;

      raise_parser_exception (node, "Unexpected pixel format '%s'", value);
      
      return low_level::PixelFormat_RGBA8; 
    }
    
///Разбор размерности текстуры
    static low_level::TextureDimension ParseTextureDimension (const ParseNode& node)
    {
      const char* value = get<const char*> (node, "");
      
      struct Tag2Value
      {
        const char*                 tag;
        low_level::TextureDimension value;
      };
      
      static Tag2Value values [] = {
        {"texture2d", low_level::TextureDimension_2D},
        {"texture3d", low_level::TextureDimension_3D},
        {"cubemap",   low_level::TextureDimension_Cubemap},
      };

      static const size_t values_count = sizeof (values) / sizeof (*values);
      
      for (size_t i=0; i<values_count; i++)
        if (!strcmp (values [i].tag, value))
          return values [i].value;

      raise_parser_exception (node, "Unexpected texture dimension '%s'", value);
      
      return low_level::TextureDimension_2D;
    }
    
///Разбор размера текстуры
    static math::vec3ui ParseTextureSize (const ParseNode& node)
    {
      math::vec3ui result;
      size_t       index = 0;

      for (Parser::AttributeIterator params_iter = make_attribute_iterator (node); params_iter && index < 3; ++params_iter, ++index)
        result [index] = xtl::io::get<size_t> (*params_iter);

      return result;
    }
    
///Разбор флагов биндинга
    static size_t ParseBindFlags (const ParseNode& node)
    {
      size_t flags = 0;
      
      for (Parser::AttributeIterator params_iter = make_attribute_iterator (node); params_iter; ++params_iter)
      {
        const char* value = *params_iter;
        
        if      (!strcmp (value, "texture"))       flags |= BindFlag_Texture;
        else if (!strcmp (value, "render_target")) flags |= BindFlag_RenderTarget;
        else if (!strcmp (value, "depth_stencil")) flags |= BindFlag_DepthStencil;
      }

      return flags;
    }
    
///Разбор описателя текстуры
    void ParseTextureDesc (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      LowLevelTextureDescPtr desc (new low_level::TextureDesc);
      
      memset (&*desc, 0, sizeof (low_level::TextureDesc));
      
      if (!iter->First ("size"))
        raise_parser_exception (*iter, "Texture must have size (use 'size' tag)");
        
      if (!iter->First ("bind_flags"))
        raise_parser_exception (*iter, "Texture must have bind flags (use 'bind_flags' tag)");
        
      math::vec3ui size = ParseTextureSize (iter->First ("size"));
      
      desc->width  = size.x;
      desc->height = size.y;
      desc->layers = size.z;
      
      ParseNode dimension_node = iter->First ("dimension");
      
      if (!dimension_node || strcmp (get<const char*> (dimension_node, "", "auto"), "auto") == 0)
      {
        if      (desc->layers <= 1 && desc->height >= 1 && desc->width >= 1) desc->dimension = low_level::TextureDimension_2D;
        else if (desc->layers >= 1 && desc->height >= 1 && desc->width >= 1) desc->dimension = low_level::TextureDimension_3D;
        else
        {
          raise_parser_exception (dimension_node, "Can't detect texture dimension for size %ux%ux%u", desc->width, desc->height, desc->layers);
        }
      }
      else
      {
        desc->dimension = ParseTextureDimension (dimension_node);
      }
      
      desc->bind_flags           = ParseBindFlags (iter->First ("bind_flags"));
      desc->format               = ParsePixelFormat (iter->First ("format"));
      desc->generate_mips_enable = strcmp (get<const char*> (*iter, "generate_mips_enable", "false"), "true") == 0;
      desc->access_flags         = low_level::AccessFlag_ReadWrite;
      
      try
      {
        library.TextureDescs ().Add (id, desc);
        
        log.Printf ("Effect auxilary texture description '%s' loaded", id);        
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
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

      ParseLog parse_log (root.Log ());

      library.Shaders ().Load (file_mask, "", ShaderLoaderLog (*iter, parse_log));

      log.Printf ("Effect shaders library '%s' loaded (%u shaders loaded)", file_mask, library.Shaders ().Size ());
    }
    
///Разбор настроек шэйдинга
    void ParseProgram (Parser::Iterator iter)
    {
      const char* id      = get<const char*> (*iter, "");
      const char* defines = get<const char*> (*iter, "defines", "");      
      const char* options = get<const char*> (*iter, "options", "");
      
      ProgramPtr program (new Program (device_manager, id, defines, options), false);
      
      for (Parser::NamesakeIterator shader_iter=iter->First ("shader"); shader_iter; ++shader_iter)
      {
        Parser::AttributeIterator params_iter = make_attribute_iterator (*shader_iter);
        
        const char* shader_name = *params_iter; ++params_iter;

        for (;params_iter; ++params_iter)
        {
          const char* profile = *params_iter;
          
          media::rfx::Shader* shader = library.Shaders ().Find (shader_name, profile);
          
          if (!shader)
            raise_parser_exception (*shader_iter, "Shader '%s' for profile '%s' not found", shader_name, profile);
            
          program->Attach (*shader);
        }
      }

      if (!program->ShadersCount ())
        raise_parser_exception (*iter, "No shaders found for shading");
        
      size_t channel = 0;
        
      for (Parser::Iterator texmap_iter=iter->First (); texmap_iter; ++texmap_iter)
      {
        if (strcmp (texmap_iter->Name (), "texmap") && strcmp (texmap_iter->Name (), "framemap"))
          continue;
          
        Parser::AttributeIterator params_iter = make_attribute_iterator (*texmap_iter);
        
        const char* semantic    = *params_iter; ++params_iter;
        const char* param_name  = *params_iter; ++params_iter;
        bool        is_framemap = strcmp (texmap_iter->Name (), "framemap") == 0;
        
        program->AddTexmap (channel, semantic, param_name, is_framemap);
        
        ++channel;
      }        

      try
      {      
        library.Programs ().Add (id, program);
        
        log.Printf ("Effect program '%s' loaded", id);        
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
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
      const char* value = get<const char*> (node, "sort", "default");

      if (!xtl::xstrcmp (value, "default"))       return SortMode_Default;
      if (!xtl::xstrcmp (value, "states"))        return SortMode_StateSwitch;
      if (!xtl::xstrcmp (value, "back_to_front")) return SortMode_BackToFront;
      if (!xtl::xstrcmp (value, "front_to_back")) return SortMode_FrontToBack;
      if (!xtl::xstrcmp (value, "none"))          return SortMode_None;
      
      raise_parser_exception (node, "Unexpected sort mode '%s'", value);
      
      return SortMode_Default;      
    }
    
    static render::manager::ClearFlag ParseClearFlags (const ParseNode& node, const char* node_name)
    {
      size_t flags = render::manager::ClearFlag_ViewportOnly;
      
      for (Parser::AttributeIterator params_iter = make_attribute_iterator (node.First (node_name)); params_iter; ++params_iter)      
      {
        if      (!xtl::xstrcmp (*params_iter, "render_targets"))       flags |= render::manager::ClearFlag_RenderTarget;
        else if (!xtl::xstrcmp (*params_iter, "depth_stencil_target")) flags |= render::manager::ClearFlag_DepthStencil;
        else if (!xtl::xstrcmp (*params_iter, "depth"))                flags |= render::manager::ClearFlag_Depth;
        else if (!xtl::xstrcmp (*params_iter, "stencil"))              flags |= render::manager::ClearFlag_Stencil;
        else if (!xtl::xstrcmp (*params_iter, "color"))                flags |= render::manager::ClearFlag_RenderTarget;
        else if (!xtl::xstrcmp (*params_iter, "all"))                  flags |= render::manager::ClearFlag_All;
        else
        {
          raise_parser_exception (node, "Unknown clear flag '%s'", *params_iter);
        }
      }
      
      return (render::manager::ClearFlag)flags;
    }

    void ParseNamedPass (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      EffectPassPtr pass = ParsePass (iter);
      
      library.EffectPasses ().Add (id, pass);
      
      log.Printf ("Effect pass '%s' loaded", id);      
    }
    
    EffectPassPtr ParsePass (Parser::Iterator iter)
    {
      try
      {
        common::StringArray          tags                     = ParseTags (*iter, "tags");
        common::StringArray          color_targets            = ParseTags (*iter, "color_targets", false);
        SortMode                     sort_mode                = ParseSortMode (*iter);
        render::manager::ClearFlag   clear_flags              = ParseClearFlags (*iter, "clear");        
        const char*                  depth_stencil_target     = get<const char*> (*iter, "depth_stencil_target", "null");
        const char*                  depth_stencil_state_name = get<const char*> (*iter, "depth_stencil_state", "");
        const char*                  blend_state_name         = get<const char*> (*iter, "blend_state", "");
        const char*                  rasterizer_state_name    = get<const char*> (*iter, "rasterizer_state", "");
        LowLevelBlendStatePtr        blend_state              = *blend_state_name ? library.BlendStates ().Find (blend_state_name) : LowLevelBlendStatePtr ();
        LowLevelDepthStencilStatePtr depth_stencil_state      = *depth_stencil_state_name ? library.DepthStencilStates ().Find (depth_stencil_state_name) : LowLevelDepthStencilStatePtr ();
        LowLevelRasterizerStatePtr   rasterizer_state         = *rasterizer_state_name ? library.RasterizerStates ().Find (rasterizer_state_name) : LowLevelRasterizerStatePtr ();
        
        EffectPassPtr pass (new EffectPass (device_manager), false);

        pass->SetColorTargets       (color_targets);
        pass->SetDepthStencilTarget (depth_stencil_target);
        pass->SetTags               (tags);
        pass->SetSortMode           (sort_mode);
        pass->SetBlendState         (blend_state);
        pass->SetDepthStencilState  (depth_stencil_state);
        pass->SetRasterizerState    (rasterizer_state);
        pass->SetClearFlags         (clear_flags);
        
        return pass;
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }
    
///Разбор эффекта рендеринга
    void ParseNamedEffect (Parser::Iterator iter)
    {
      const char* id = get<const char*> (*iter, "");
      
      EffectPtr effect = ParseEffect (iter);
      
      library.Effects ().Add (id, effect);
      
      log.Printf ("Effect '%s' loaded", id);
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
        root.Log ().Error (*iter, "%s", e.what ());
        throw;
      }
    }    
  
  private:
    common::ParseNode    root;           //корневой узел
    EffectLoaderLibrary& library;        //библиотека загрузки эффектов
    render::manager::Log log;            //протокол рендера
    DeviceManagerPtr     device_manager; //менеджер устройства отрисовки
};

}

namespace render
{

namespace manager
{

void parse_effect_library (const DeviceManagerPtr& device_manager, const common::ParseNode& node, const char* name, EffectLoaderLibrary& library)
{
  EffectLoader loader (device_manager, name, node, library);
}

}

}
