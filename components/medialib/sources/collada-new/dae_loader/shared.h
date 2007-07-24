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
class  DaeParser;
class  MeshSourceMap;
class  MeshInputBuilder;
struct SurfaceInfo;

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
    LogScope  (Parser::Node* node, DaeParser& parser, const char* id = 0);
    ~LogScope ();

    Parser::Node* Node () const { return node; }
    LogScope*     Prev () const { return prev; }
    const char*   Id   () const { return id; }
  
  private:
    DaeParser&    parser;
    Parser::Node* node;
    LogScope*     prev;
    const char*   id;
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
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Протоколирование
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogError   (Parser::Node* node, const char* format, ...);
    void LogWarning (Parser::Node* node, const char* format, ...);    
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Печать ошибок в протокол
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PrintLog (const LogFunction&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Разбор отдельных элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ParseRoot                (Parser::Iterator);
    void ParseLibraries           (Parser::Iterator);
    void ParseLibraryEffects      (Parser::Iterator);
    void ParseLibraryMaterials    (Parser::Iterator);
    void ParseLibraryGeometries   (Parser::Iterator);
    void ParseImage               (Parser::Iterator);
    void ParseEffect              (Parser::Iterator);
    void ParseEffectProfileCommon (Parser::Iterator, Effect& effect);
    bool ParseTexture             (Parser::Iterator, Parser::Iterator profile_iter, Texture& texture);
    void ParseMaterial            (Parser::Iterator);
    void ParseGeometry            (Parser::Iterator);
    void ParseMesh                (Parser::Iterator, Mesh& mesh);
    void ParseMeshSource          (Parser::Iterator, MeshSourceMap& sources);
    void ParseSurfaceInput        (Parser::Iterator, Parser::Iterator mesh_iter, MeshSourceMap& sources, MeshInputBuilder& inputs);
    void ParseSurface             (Parser::Iterator, Parser::Iterator mesh_iter, Mesh& mesh, PrimitiveType type, MeshSourceMap& sources);
    void ParseSurfaceBuffers      (Parser::Iterator, Parser::Iterator surface_iter, SurfaceInfo& info);

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
