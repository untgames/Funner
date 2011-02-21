#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::mid_level;

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
      for_each_child (*iter, "blend",        xtl::bind (&EffectLoader::ParseBlendState, this, _1));
      for_each_child (*iter, "depthstencil", xtl::bind (&EffectLoader::ParseDepthStencilState, this, _1));
      for_each_child (*iter, "rasterizer",   xtl::bind (&EffectLoader::ParseRasterizerState, this, _1));
      for_each_child (*iter, "sampler",      xtl::bind (&EffectLoader::ParseSamplerState, this, _1));
      for_each_child (*iter, "program",      xtl::bind (&EffectLoader::ParseProgram, this, _1));
    }
    
///Разбор аргумента операции смешивания цветов
    BlendArgument ParseBlendArgument (ParseNode& node, const char* value)
    {
      if (!xtl::xstricmp (value, "zero"))        return BlendArgument_Zero;
      if (!xtl::xstricmp (value, "one"))         return BlendArgument_One;
      if (!xtl::xstricmp (value, "srccolor"))    return BlendArgument_SourceColor;
      if (!xtl::xstricmp (value, "srcalpha"))    return BlendArgument_SourceAlpha;
      if (!xtl::xstricmp (value, "invsrccolor")) return BlendArgument_InverseSourceColor;
      if (!xtl::xstricmp (value, "invsrcalpha")) return BlendArgument_InverseSourceAlpha;
      if (!xtl::xstricmp (value, "dstcolor"))    return BlendArgument_DestinationColor;
      if (!xtl::xstricmp (value, "dstalpha"))    return BlendArgument_DestinationAlpha;
      if (!xtl::xstricmp (value, "invdstcolor")) return BlendArgument_InverseDestinationColor;
      if (!xtl::xstricmp (value, "invdstalpha")) return BlendArgument_InverseDestinationAlpha;
      
      raise_parser_exception (node, "Bad blend argument '%s'", value);
      
      return BlendArgument_Zero;
    }
    
///Разбор операции смешивания
    void ParseBlendOperation (ParseNode& node, Parser::AttributeIterator iter, BlendOperation& operation, BlendArgument& src_arg, BlendArgument& dst_arg)
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
      
      desc.blend_enable                     = true;
      desc.sample_alpha_to_coverage         = false;
      desc.blend_color_operation            = BlendOperation_Add;
      desc.blend_color_source_argument      = BlendArgument_One;
      desc.blend_color_destination_argument = BlendArgument_Zero;
      desc.blend_alpha_operation            = BlendOperation_Add;
      desc.blend_alpha_source_argument      = BlendArgument_One;
      desc.blend_alpha_destination_argument = BlendArgument_Zero;
      desc.color_write_mask                 = 0;
      
      if (iter->First ("noblend"))           desc.blend_enable             = false;
      if (iter->First ("alpha_to_coverage")) desc.sample_alpha_to_coverage = true;
      
      const char* color_mask = get<const char*> (*iter, "mask", "rgba");
      
      if (strchr (color_mask, 'r')) desc.color_write_mask |= ColorWriteFlag_Red;
      if (strchr (color_mask, 'g')) desc.color_write_mask |= ColorWriteFlag_Green;
      if (strchr (color_mask, 'b')) desc.color_write_mask |= ColorWriteFlag_Blue;
      if (strchr (color_mask, 'a')) desc.color_write_mask |= ColorWriteFlag_Alpha;
      
      if (ParseNode color_node = iter->First ("color"))
      {
        Parser::AttributeIterator attr_iter = make_attribute_iterator (color_node);
        
        ParseBlendOperation (*iter, attr_iter, desc.blend_color_operation, desc.blend_color_source_argument, desc.blend_color_destination_argument);
      }
      
      if (ParseNode alpha_node = iter->First ("alpha"))
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
    
///Разбор состояния уровня отсечения
    void ParseDepthStencilState (Parser::Iterator iter)
    {
    }
    
///Разбор состояния уровня растеризации
    void ParseRasterizerState (Parser::Iterator iter)
    {
    }
    
///Разбор состояния сэмплера
    void ParseSamplerState (Parser::Iterator iter)
    {
    }
    
///Разбор программы
    void ParseProgram (Parser::Iterator iter)
    {
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
