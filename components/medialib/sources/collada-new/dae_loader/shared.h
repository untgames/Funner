#include <media/collada/model.h>
#include <common/parser.h>
#include <xtl/functional>
#include <stl/hash_map>

using namespace medialib::collada;
using namespace common;
using namespace xtl;
using namespace math;

namespace medialib
{

namespace collada
{

//forward declarations
class DaeParser;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отображение имени в значение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct String2Value
{
  const char* string; //строковое значение
  T           value;  //значение
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Утилита, позволяющая отслеживать стек вызовов парсера
///////////////////////////////////////////////////////////////////////////////////////////////////
class LogScope
{
  public:
    LogScope  (Parser::Node* node, DaeParser& parser);
    ~LogScope ();

    Parser::Node* Node () const { return node; }
    LogScope*     Prev () const { return prev; }
  
  private:
    DaeParser&    parser;
    Parser::Node* node;
    LogScope*     prev;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузчик коллады
///////////////////////////////////////////////////////////////////////////////////////////////////
class DaeParser
{
  friend class LogScope;
  typedef Model::LogFunction LogFunction;
  public:    
    DaeParser (const char* file_name, Model& model, const LogFunction& log);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Печать ошибок в протокол
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PrintLog (const LogFunction&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Протоколирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogError   (Parser::Node* node, const char* format, ...);
    void LogWarning (Parser::Node* node, const char* format, ...);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разбор отдельных элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ParseRoot                (Parser::Iterator);
    void ParseLibraries           (Parser::Iterator);
    void ParseLibraryEffects      (Parser::Iterator);
    void ParseLibraryMaterials    (Parser::Iterator);
    void ParseImage               (Parser::Iterator);
    void ParseEffect              (Parser::Iterator);
    void ParseEffectProfileCommon (Parser::Iterator, Effect& effect);
    bool ParseTexture             (Parser::Iterator, Parser::Iterator profile_iter, Texture& texture);
    void ParseMaterial            (Parser::Iterator);

  private:
    template <class T> bool CheckedRead (Parser::Node* node, const char* tag, T& value)
    {
      if (node && tag)
        node = node->First (tag);        

      if (!node)
        return false;
                    
      if (!read (node, 0, value))
      {
        const char* str_value = get<const char*> (node, 0);
        
        LogScope scope (node, *this);
        
        LogError (node, "Incorrect value '%s'", str_value ? str_value : "UNREADABLE");
        return false;
      }
      
      return true;
    }

  private:
    Model&    model;         //загружаемая модель
    ParseLog  parse_log;     //протокол парсера
    Parser    parser;        //парсер
    LogScope* current_scope; //текущий блок разбора
};

}

}
