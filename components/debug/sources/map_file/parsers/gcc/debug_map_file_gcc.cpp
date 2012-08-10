#include <stl/vector>
#include <stl/string>

#include <xtl/token_parser.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>

#include <debug/map_file.h>

using namespace common;
using namespace debug;

namespace
{

const char* LOG_NAME = "debug.gcc_map_file_parser";

}

namespace components
{

namespace debug_map_file_gcc
{

/*
    Парсер
*/

class GccMapFileParser
{
  public:
///Конструктор
    GccMapFileParser (const char* file_name, MapFile& file)
      : map_file (file)
      , log (LOG_NAME)
    {
      try
      {
          //загрузка содержимого map file
        
        FileSystem::LoadTextFile (file_name, file_content);
        
          //инициализация разбора
          
        state = State_SearchingSymbol;
        
        line_number = 0;
        
        sym_header        = "";
        sym_name          = "";
        sym_start_address = 0;
        sym_size          = 0;
        
        static const size_t MAX_TOKENS_SIZE = 16;
        
        tokens.reserve (MAX_TOKENS_SIZE);
        
          //разбор
          
        Parse (&file_content [0]);
      }
      catch (xtl::exception& e)
      {
        e.touch ("debug::GccMapFileParser::GccMapFileParser");
        throw;
      }
    }
    
  private:
///Разбор
    void Parse (char* content)
    {
      char *first = content, *s = first;

      for (;;s++)
      {
        switch (*s)
        {
          case '\r':
            *s = ' ';          
            break;
          case '\n':
            *s = '\0';

            ParseLine (first);

            first = s + 1;
            break;
          case '\0':
            ParseLine (first);
            return;
          default:
            break;
        }
      }      
    }
    
///Разбиение строки на токены
  size_t TokenizeLine (char* line)
  {
    size_t empty_space = 0;
    
    tokens.clear ();
    
    char* s = &line [0];
    
    for (;;)
    {    
      bool loop = true;
      
      for (;loop;)
      {
        switch (*s)
        {
          case ' ':
          case '\t':
            s++;
            break;
          default:
            loop = false;
            break;
        }
      }
      
      const char *first = s, *last = s;

      if (*s)
      {
        bool loop = true;
        
        for (++s;loop;)
        {
          switch (*s)
          {
            case ' ':
            case '\t':
            case '\0':
              loop = false;
              break;
            default:
              s++;
              break;
          }
        }
        
        last = s;
      }
      
      if (first != last)      
      {
        if (tokens.empty ())
          empty_space = first - line;

        tokens.push_back (first);
      }

      if (!*s)
        break;

      *s++ = '\0';
    }    
    
    return empty_space;
  }
    
///Разбор строки map file
    void ParseLine (char* line)
    {
      line_number++;      
      
      if (!*line)
        return;        
        
      try
      {        
        size_t empty_space = TokenizeLine (line);            
        
        switch (state)
        {
          case State_SearchingSymbol:
            if (empty_space != 1 || tokens.size () > 1)
              return;                                  

            state      = State_SymbolHeaderRead;
            sym_header = tokens [0];

            break;
          case State_SymbolHeaderRead:
            if (tokens.size () != 3 || empty_space < 16)
            {
              state = State_SearchingSymbol;
              return;
            }

            sym_start_address = xtl::io::get<size_t> (tokens [0]);
            sym_size          = xtl::io::get<size_t> (tokens [1]);
            state             = State_SymbolFirstLineRead;          
          
            break;
          case State_SymbolFirstLineRead:
          {
            if (tokens.size () != 2)
            {
              state = State_SearchingSymbol;
              return;
            }
            
            size_t checker = xtl::io::get<size_t> (tokens [0]);
            
            sym_name = tokens [1];
            state    = State_SearchingSymbol;          
            
            map_file.AddSymbol (Symbol (sym_name, sym_start_address, sym_size));

            break;          
          }
          default:
            break;
        }
      }
      catch (std::exception& e)
      {
        state = State_SearchingSymbol;        
        
        log.Printf ("%s\n    at GccMapFileParser::ParseLine(line_number=%u)", e.what (), line_number);
      }
      catch (...)
      {
        state = State_SearchingSymbol;        
        
        log.Printf ("unknown exception\n    at GccMapFileParser::ParseLine(line_number=%u)", line_number);        
      }
    }    

  private:
    enum State
    {
      State_SearchingSymbol,
      State_SymbolHeaderRead,
      State_SymbolFirstLineRead,
    };
    
    typedef stl::vector<const char*> TokenList;
    
  private:
    MapFile&    map_file;          //файл карты символов
    stl::string file_content;      //содержимое файла
    size_t      line_number;       //номер строки разбора
    State       state;             //состояние разбора
    TokenList   tokens;            //токены строки разбора        
    const char* sym_header;        //заголовок символа
    size_t      sym_start_address; //начальный адрес
    size_t      sym_size;          //размер символа
    const char* sym_name;          //имя символа
    common::Log log;               //протокол
};

/*
    Компонент парсера gcc map files
*/

class Component
{
  public:
    Component ()
    {
      MapFileManager::RegisterLoader ("gcc", &Loader);
    }

  private:
    static void Loader (const char* file_name, MapFile& file)
    {
      GccMapFileParser (file_name, file);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> DebugMapFileGcc ("debug.map_file.loaders.gcc");

}

}

}
