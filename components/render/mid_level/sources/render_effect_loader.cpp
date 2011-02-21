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
    EffectLoader (const char* file_name)
      : parser (file_name, "wxf")
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
    
///Разбор состояния уровня смешивания цветов
    void ParseBlendState (Parser::Iterator iter)
    {
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
    common::Parser         parser;  //парсер файла эффектов
    EffectLoaderLibrary    library; //библиотека загрузки эффектов
    render::mid_level::Log log;     //протокол рендера
};

}

namespace render
{

namespace mid_level
{

void parse_effect_library (const char* file_name)
{
  EffectLoader loader (file_name);
}

}

}
