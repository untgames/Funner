#include "shared.h"

using namespace scene_graph;
using namespace common;
using namespace math;

namespace
{

/*
    Константы
*/

const math::vec3f DEFAULT_BEFORE_NODE_OFFSET (0.0f, 0.0f, 1e-06f);

}

/*
    Описание реализации парсера
*/

struct XmlSceneParser::Impl
{
  typedef stl::hash_map<stl::hash_key<const char*>, common::ParseNode>            IncludeMap;
  typedef stl::hash_map<stl::hash_key<const char*>, XmlSceneParser::ParseHandler> ParserMap;

  common::ParseNode root;        //корневой узел
  SceneParserCache  cache;       //кэш парсера
  ResourceGroup     resources;   //ресурсы сцены
  IncludeMap        includes;    //карта подключаемых файлов
  ParserMap         parsers;     //парсеры
  
///Конструктор
  Impl (const ParseNode& in_root) : root (in_root) {}
  
///Предварительный разбор
  void Prepare (const ParseNode& decl)
  {
    for (Parser::Iterator iter=decl.First (); iter; ++iter)
    {
      try
      {
        if (!strcmp (iter->Name (), "resource"))
        {
          ParseResourceNode (*iter);
        }
        else if (!strcmp (iter->Name (), "include"))
        {
          PrepareIncludeNode (*iter);
        }
      }
      catch (std::exception& e)
      {
        root.Log ().Error (*iter, "%s", e.what ());
      }
      catch (...)
      {
        root.Log ().Error (*iter, "unknown exception");
      }
    }
  }

///Разбор узла описания ресурсов
  void ParseResourceNode (const ParseNode& decl)
  {
    const char* name = get<const char*> (decl, "name");
    
    if (!name)
      return;      
      
    resources.Add (name);
  }
  
///Разбор подключаемого узла
  void PrepareIncludeNode (const ParseNode& decl)
  {
    const char* source_name = get<const char*> (decl, "source");

    bool ignore_unavailability = strcmp (get<const char*> (decl, "ignore_unavailability", "false"), "true") == 0;
    
    if (!source_name)
      return;
      
      //попытка найти в списке уже загруженных
      
    IncludeMap::iterator iter = includes.find (source_name);
    
    if (iter != includes.end ())
      return;

      //парсинг

    if (!FileSystem::IsFileExist (source_name) && ignore_unavailability)
      return;

    Parser parser (root.Log (), source_name, "xml");
    
    const ParseNode& xscene_root = parser.Root ().First ();
    
    if (strcmp (xscene_root.Name (), XSCENE_ROOT))
      throw xtl::format_operation_exception ("", "Bad XML scene file '%s'. Root node not found", source_name);
      
    bool partial = strcmp (get<const char*> (xscene_root, "partial", ""), "true") == 0;

    if (!partial)
      throw xtl::format_operation_exception ("", "Bad XML scene file '%s'. Only partial definitions are allowed in this context", source_name);            

      //сохранение ссылки на подключаемый файл
      
    includes.insert_pair (source_name, xscene_root);
    
      //вложенный разбор
      
    Prepare (xscene_root);
  }
};

/*
    Конструктор / деструктор
*/

XmlSceneParser::XmlSceneParser (const ParseNode& root)
{
  try
  { 
      //создание реализации            

    impl = new Impl (root);
    
      //регистрация парсеров
      
    RegisterParser ("node", xtl::bind (&XmlSceneParser::CreateNode<Node>, this, _1, _2, _3));
    RegisterParser ("ortho_camera", xtl::bind (&XmlSceneParser::CreateNode<OrthoCamera>, this, _1, _2, _3));
    RegisterParser ("perspective_camera", xtl::bind (&XmlSceneParser::CreateNode<PerspectiveCamera>, this, _1, _2, _3));
    RegisterParser ("spot_light", xtl::bind (&XmlSceneParser::CreateNode<SpotLight>, this, _1, _2, _3));
    RegisterParser ("direct_light", xtl::bind (&XmlSceneParser::CreateNode<DirectLight>, this, _1, _2, _3));
    RegisterParser ("point_light", xtl::bind (&XmlSceneParser::CreateNode<PointLight>, this, _1, _2, _3));
    RegisterParser ("mesh", xtl::bind (&XmlSceneParser::CreateNode<VisualModel>, this, _1, _2, _3));
    RegisterParser ("text_line", xtl::bind (&XmlSceneParser::CreateNode<TextLine>, this, _1, _2, _3));
    RegisterParser ("sprite", xtl::bind (&XmlSceneParser::CreateNode<Sprite>, this, _1, _2, _3));
    RegisterParser ("listener", xtl::bind (&XmlSceneParser::CreateNode<Listener>, this, _1, _2, _3));
    RegisterParser ("sound", xtl::bind (&XmlSceneParser::CreateNode<SoundEmitter>, this, _1, _2, _3));
    RegisterParser ("include", xtl::bind (&XmlSceneParser::IncludeSubscene, this, _1, _2, _3));    

      //построение списка ресурсов

    impl->Prepare (root);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::XmlSceneParser");
    throw;
  }
}

XmlSceneParser::~XmlSceneParser ()
{
}

/*
    Создание парсера
*/

ISceneParser* XmlSceneParser::Create (const ParseNode& root)
{
  try
  {
    return new XmlSceneParser (root);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Create");
    throw;
  }
}

/*
    Получение списка ресурсов
*/

const ResourceGroup& XmlSceneParser::Resources ()
{
  return impl->resources;
}

/*
    Создание сцены
*/

namespace
{

void print_log (const SceneManager::LogHandler& log_handler, const char* message)
{
  try
  {
    log_handler (message);
  }
  catch (...)
  {
  }
}

}

void XmlSceneParser::CreateScene (Node& parent, SceneContext& context)
{
  try
  {
    Parse (impl->root, parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::CreateScene");
    throw;
  }
}

/*
    Корневой узел
*/

const ParseNode& XmlSceneParser::Root () const
{
  return impl->root;
}

/*
    Кэш
*/

SceneParserCache& XmlSceneParser::Cache ()
{
  return impl->cache;
}

/*
    Регистрация парсера узла определенного типа
*/

void XmlSceneParser::RegisterParser (const char* type, const ParseHandler& handler)
{
  if (!type)
    throw xtl::make_null_argument_exception ("scene_graph::XmlSceneParser::RegisterParser", "type");
    
  impl->parsers.insert_pair (type, handler);
}

void XmlSceneParser::UnregisterParser (const char* type)
{
  if (!type)
    return;
    
  impl->parsers.erase (type);
}

void XmlSceneParser::UnregisterAllParsers ()
{
  impl->parsers.clear ();
}

/*
    Разбор атрибутов
*/

namespace
{

class WordParser
{
  public:
    WordParser (const char* string)
      : pos ((unsigned char*)string)
    {
    }

    typedef stl::pair<const char*, const char*> Word;

    Word NextWord ()
    {
      if (!*pos)
        return Word ((const char*)pos, (const char*)pos);

      //cut leading spaces        
      
      bool loop = true;
      
      for (;loop; pos++)
      {
        switch (*pos)
        {
          case ' ':
          case '\t':
            break;
          default:
            loop = false;
            pos--;
            break;
        }
      }

      const unsigned char *first = 0, *last = 0;

      first = pos;
      
      loop = true;

      for (;loop; pos++)
      {
        switch (*pos)
        {
          case ';':
          case ',':
          case '\0':
            loop = false;
            pos--;
            break;          
          default:
            break;
        }
      }

      last = pos;

      if (*pos)
        pos++;

      if (last != first)
      {
        loop = true;
        
        for (--last; loop; last--)
        {
          switch (*last)
          {
            case ' ':
            case '\t':
              break;
            default:
              loop = false;
              last++;
              break;
          }
        }        

        ++last;
      }

      return Word ((const char*)first, (const char*)last);
    }

    bool EndOfString () const { return *pos == '\0'; }

  private:
    unsigned char* pos;
};

bool try_parse_attribute (const common::ParseNode& decl, const char* name, size_t size, float* value)
{
  ParseNode node = decl.First (name);
  
  if (!node || !node.AttributesCount ())
    return false;

  const char* string_value = node.Attribute (0);
  
  WordParser parser (string_value);
  
  size_t token_index = 0;
  
  char parse_buffer [32];

  do
  {
    WordParser::Word word = parser.NextWord ();

    if (word.first != word.second)
    {
      size_t length = word.second - word.first;
      
      if (length >= sizeof (parse_buffer))
        raise_parser_exception (node, "Internal error: string is too big for tokenize '%s' (max token size is %u)", string_value, sizeof (parse_buffer) - 1);

      if (token_index >= size)
        raise_parser_exception (node, "Too many tokens in '%s' (expected %u tokens)", string_value, size);
        
      memcpy (parse_buffer, word.first, length);
      
      parse_buffer [length] = 0;      

      if (!xtl::io::read_and_cast<float> (parse_buffer, value [token_index]))
        raise_parser_exception (node, "Can't parse token '%s' while parsing value '%s'", parse_buffer, string_value);
        
      token_index++;
    }

  } while (!parser.EndOfString ());  

  if (token_index != size)
    raise_parser_exception (node, "Too little tokens in '%s' (expected %u tokens)", string_value, size);

  return true;
}

}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::vec2f& value)
{
  try
  {    
    try_parse_attribute (decl, name, 2, &value [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::vec2f&)");
    throw;
  }
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::vec3f& value)
{
  try
  {
    try_parse_attribute (decl, name, 3, &value [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::vec3f&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::vec4f& value)
{
  try
  {
    try_parse_attribute (decl, name, 4, &value [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::vec4f&)");
    throw;
  }  
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::quatf& value)
{
  try
  {
    try_parse_attribute (decl, name, 4, &value [0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::quatf&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::mat2f& value)
{
  try
  {
    try_parse_attribute (decl, name, 4, &value [0][0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::mat2f&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::mat3f& value)
{
  try
  {
    try_parse_attribute (decl, name, 9, &value [0][0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::mat3f&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, math::mat4f& value)
{
  try
  {
    try_parse_attribute (decl, name, 16, &value [0][0]);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,math::mat4f&)");
    throw;
  }    
}

void XmlSceneParser::ParseAttribute (const common::ParseNode& decl, const char* name, size_t size, float* value)
{
  try
  {
    try_parse_attribute (decl, name, size, value);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseAttribute(const ParseNode&,const char*,size_t,float*)");
    throw;
  }    
}
    
/*
    Разбор узла
*/

template <class T>
void XmlSceneParser::CreateNode (const common::ParseNode& decl, Node& parent, SceneContext& context)
{
  T::Pointer node = T::Create ();

  Parse (decl, *node, parent, context);
}

template <> void XmlSceneParser::CreateNode<SoundEmitter> (const common::ParseNode& decl, Node& parent, SceneContext& context)
{
  const char* source = get<const char*> (decl, "source");

  SoundEmitter::Pointer node = SoundEmitter::Create (source);

  Parse (decl, *node, parent, context);
}

void XmlSceneParser::IncludeSubscene (const common::ParseNode& decl, Node& parent, SceneContext& context)
{
  const char* source_name = "";

  try
  {
    source_name = get<const char*> (decl, "source", "");
    
    Impl::IncludeMap::iterator iter = impl->includes.find (source_name);
    
    if (iter == impl->includes.end ())
      return;
      
    Parse (iter->second, parent, context);
  }
  catch (xtl::exception& e)
  {
    if (source_name) e.touch ("scene_graph::XmlSceneParser::IncludeSubscene");
    else             e.touch ("scene_graph::XmlSceneParser::IncludeSubscene('%s')", source_name);

    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Node& parent, SceneContext& context)
{
  try
  {
    for (Parser::Iterator iter=decl.First (); iter; ++iter)
    {
      try
      {
        ParseNode decl = *iter;

        const char* type = decl.Name ();

        Impl::ParserMap::iterator iter = impl->parsers.find (type);

        if (iter == impl->parsers.end ())
          continue; //игнорирование неизвестных узлов

        iter->second (decl, parent, context);
      }
      catch (std::exception& e)
      {
        if (!context.FilterError (e.what ()))
          throw;
        
        print_log (context.LogHandler (), e.what ());
      }
      catch (...)            
      {
        static const char* ERROR_STRING = "unknown exception";
        
        if (!context.FilterError (ERROR_STRING))
          throw;        

        print_log (context.LogHandler (), ERROR_STRING);
      }
    }    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::ParseDispatch");
    throw;
  }  
}

/*
    Парсинг
*/

namespace
{

///Описание параметров узла
struct NodeDecl: public xtl::reference_counter
{
  struct Pivot
  {
    math::vec3f position;              //точка поворота
    bool        has_orientation_pivot; //центр поворота не совпадает с геометрическим центром
    bool        has_scale_pivot;       //центр масштабирования не совпадает с геометрическим центром    
    
    Pivot ()
      : has_orientation_pivot (true)
      , has_scale_pivot (true) 
    {
    }
  };

  stl::string                name;               //имя узла    
  math::vec3f                position;           //положение
  math::vec3f                scale;              //масштаб
  math::quatf                orientation;        //ориентация
  stl::auto_ptr<Pivot>       pivot;              //точка поворота/масштабирования
  bool                       is_world_transform; //трансформации заданы в мировых координатах
  stl::auto_ptr<stl::string> before_node;         //имя узла, перед которым будет прибинден данный узел
  stl::auto_ptr<PropertyMap> properties;         //пользовательские свойства узла
  stl::auto_ptr<stl::string> parent_name;        //имя родительского узла

///Конструктор
  NodeDecl ()
    : is_world_transform (false)
    , scale (1.0f)
  {
  }  
};

///Параметр объекта
template <class T> struct Param
{
  T    value;
  bool state;
  
  Param () : value (), state () {}
  
  void Parse (const ParseNode& node, const char* name)
  {
    ParseNode child = node.First (name);
    
    if (!child)
    {
      state = false;
      return;
    }

    value = get<T> (child, "");

    state = true;
  }
};

///Описание параметров камеры
struct OrthoCameraDecl: public xtl::reference_counter
{
  enum ParamId
  {
    Left, Right, Top, Bottom, ZNear, ZFar, Param_Num
  };  
  
  Param<float> params [Param_Num];
};

struct PerspectiveCameraDecl: public xtl::reference_counter
{
  enum ParamId
  {
    FovX, FovY, ZNear, ZFar, Param_Num
  };  
  
  Param<float> params [Param_Num];
};

typedef xtl::intrusive_ptr<NodeDecl>              NodeDeclPtr;
typedef xtl::intrusive_ptr<OrthoCameraDecl>       OrthoCameraDeclPtr;
typedef xtl::intrusive_ptr<PerspectiveCameraDecl> PerspectiveCameraDeclPtr;

PropertyType get_property_type (common::ParseNode& node)
{
  const char* value = get<const char*> (node, "");

  if (!xtl::xstrcmp (value, "string")) return PropertyType_String;
  if (!xtl::xstrcmp (value, "int"))    return PropertyType_Integer;
  if (!xtl::xstrcmp (value, "float"))  return PropertyType_Float;
  if (!xtl::xstrcmp (value, "vector")) return PropertyType_Vector;
  if (!xtl::xstrcmp (value, "matrix")) return PropertyType_Matrix;

  raise_parser_exception (node, "Unexpected property type '%s'", value);

  return common::PropertyType_String;
}

}

void XmlSceneParser::Parse (const ParseNode& decl, Node& node, Node& default_parent, SceneContext& context)
{
  try
  {
      //попытка найти параметры в кеше
      
    NodeDeclPtr node_decl;

    if (NodeDeclPtr* node_decl_ptr = impl->cache.FindValue<NodeDeclPtr> (decl))
    {
      node_decl = *node_decl_ptr;
    }
    else
    {
      node_decl = xtl::intrusive_ptr<NodeDecl> (new NodeDecl, false);            

        //парсинг базовых свойств
      
      node_decl->name = get<const char*> (decl, "id", "");

        //парсинг точки поворота

      if (decl.First ("pivot"))      
      {
        stl::auto_ptr<NodeDecl::Pivot> pivot (new NodeDecl::Pivot);

        ParseAttribute (decl, "pivot", pivot->position);

        pivot->has_orientation_pivot = strcmp (get<const char*> (decl, "orientation_pivot", "true"), "true") == 0;
        pivot->has_scale_pivot       = strcmp (get<const char*> (decl, "scale_pivot", "true"), "true") == 0;

        node_decl->pivot = pivot;
      }

        //парсинг трансформаций

      node_decl->is_world_transform = strcmp (get<const char*> (decl, "bind_space", "local"), "world") == 0;

      if (ParseNode tm_node = decl.First ("transform"))
      {
        math::mat4f tm;        
        
        ParseAttribute (tm_node, "", tm);
        
        affine_decompose (tm, node_decl->position, node_decl->orientation, node_decl->scale);
      }
      else
      {
        ParseAttribute (decl, "position", node_decl->position);
        ParseAttribute (decl, "scale", node_decl->scale);        
        
        if (ParseNode rotation_node = decl.First ("rotation"))
        {
          float rotation [3] = {0.0f, 0.0f, 0.0f};
          
          ParseAttribute (rotation_node, "", 3, &rotation [0]);
          
          node_decl->orientation = to_quat (degree (rotation [0]), degree (rotation [1]), degree (rotation [2]));
        }
        else
        {
          ParseAttribute (decl, "orientation", node_decl->orientation);
        }
      }

        //парсинг пользовательских свойств              

      PropertyMap* properties = 0;
      
      if (decl.First ("property"))
        node_decl->properties.reset (properties = new PropertyMap);

      for (Parser::NamesakeIterator iter = decl.First ("property"); iter; ++iter)
      {
        ParseNode property_decl = *iter;

        const char*  name  = get<const char*> (property_decl, "name");
        PropertyType type  = get_property_type (property_decl.First ("type"));

        size_t property_index = properties->AddProperty (name, type, 1);
        
        stl::string value = get<const char*> (property_decl, "value");
        
        properties->SetProperty (property_index, value.c_str ());
      }

        //парсинг after node
        
      if (ParseNode before_node_decl = decl.First ("before_node"))
      {
        node_decl->before_node.reset (new stl::string);
        
        *node_decl->before_node = get<const char*> (before_node_decl, "");
      }
              
        //парсинг привязки к родителю
        
      if (ParseNode parent_name_decl = decl.First ("parent"))
      {
        node_decl->parent_name.reset (new stl::string);
        
        *node_decl->parent_name = get<const char*> (parent_name_decl, "");
      }

        //регистрация описателя узла
        
      impl->cache.SetValue (decl, node_decl);
    }

      //настройка узла

    if (!node_decl->name.empty ())
      node.SetName (node_decl->name.c_str ());
      
    if (node_decl->pivot)
    {
      node.SetPivotPosition (node_decl->pivot->position);
      node.SetOrientationPivotEnabled (node_decl->pivot->has_orientation_pivot);
      node.SetScalePivotEnabled (node_decl->pivot->has_scale_pivot);
    }
      
    node.SetPosition (node_decl->position);
    node.SetOrientation (node_decl->orientation);
    node.SetScale (node_decl->scale);
    
    if (node_decl->properties)
      node.SetProperties (node_decl->properties->Clone ());
      
    if (node_decl->before_node && !node_decl->before_node->empty ()) //TODO: сделать в виде контроллера!!!
    {
      math::vec3f offset = DEFAULT_BEFORE_NODE_OFFSET;
      
      int property_index = context.Properties ().IndexOf ("BeforeNodeOffset");
      
      if (property_index != -1)
        offset = context.Properties ().GetVector (property_index);        
        
      for (Node* search_base = &default_parent; search_base; search_base = &*search_base->Parent ())
      {
        if (Node::Pointer before_node = search_base->FindChild (node_decl->before_node->c_str (), NodeSearchMode_OnAllSublevels))
        {
          offset *= -before_node->WorldOrtZ ();
          
          node.SetPosition (node.Position () + offset);
        }
      }                        
    }

    Node::Pointer parent = &default_parent;
    
    if (node_decl->parent_name && !node_decl->parent_name->empty ())
    {
      for (Node* search_base = &default_parent; search_base = &*search_base->Parent ();)
      {
        if (Node::Pointer node = search_base->FindChild (node_decl->parent_name->c_str (), NodeSearchMode_OnAllSublevels))
        {
          parent = node;
          break;
        }
      }
    }    

    node.BindToParent (*parent, NodeBindMode_AddRef, node_decl->is_world_transform ? NodeTransformSpace_World : NodeTransformSpace_Local);

      //разбор вложенных узлов
      
    Parse (decl, node, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Node&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Entity& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Node&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Entity&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Camera& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Entity&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Camera&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, OrthoCamera& node, Node& parent, SceneContext& context)
{
  try
  { 
      //попытка найти параметры в кеше
      
    OrthoCameraDeclPtr node_decl;

    if (OrthoCameraDeclPtr* node_decl_ptr = impl->cache.FindValue<OrthoCameraDeclPtr> (decl))
    {
      node_decl = *node_decl_ptr;
    }
    else
    {
      node_decl = xtl::intrusive_ptr<OrthoCameraDecl> (new OrthoCameraDecl, false);
      
      node_decl->params [OrthoCameraDecl::Left].Parse (decl, "left");
      node_decl->params [OrthoCameraDecl::Right].Parse (decl, "right");
      node_decl->params [OrthoCameraDecl::Top].Parse (decl, "top");
      node_decl->params [OrthoCameraDecl::Bottom].Parse (decl, "bottom");
      node_decl->params [OrthoCameraDecl::ZNear].Parse (decl, "znear");
      node_decl->params [OrthoCameraDecl::ZFar].Parse (decl, "zfar");
    }

      //настройка камеры
      
    if (node_decl->params [OrthoCameraDecl::Left].state)    node.SetLeft (node_decl->params [OrthoCameraDecl::Left].value);
    if (node_decl->params [OrthoCameraDecl::Right].state)   node.SetRight (node_decl->params [OrthoCameraDecl::Right].value);
    if (node_decl->params [OrthoCameraDecl::Top].state)     node.SetTop (node_decl->params [OrthoCameraDecl::Top].value);
    if (node_decl->params [OrthoCameraDecl::Bottom].state)  node.SetBottom (node_decl->params [OrthoCameraDecl::Bottom].value);
    if (node_decl->params [OrthoCameraDecl::ZNear].state)   node.SetZNear (node_decl->params [OrthoCameraDecl::ZNear].value);
    if (node_decl->params [OrthoCameraDecl::ZFar].state)    node.SetZFar (node_decl->params [OrthoCameraDecl::ZFar].value);

      //разбор родительских параметров
    
    Parse (decl, static_cast<Camera&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,OrthoCamera&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, PerspectiveCamera& node, Node& parent, SceneContext& context)
{
  try
  {
      //попытка найти параметры в кеше
      
    PerspectiveCameraDeclPtr node_decl;

    if (PerspectiveCameraDeclPtr* node_decl_ptr = impl->cache.FindValue<PerspectiveCameraDeclPtr> (decl))
    {
      node_decl = *node_decl_ptr;
    }
    else
    {
      node_decl = xtl::intrusive_ptr<PerspectiveCameraDecl> (new PerspectiveCameraDecl, false);

      node_decl->params [PerspectiveCameraDecl::FovX].Parse (decl, "fov_x");
      node_decl->params [PerspectiveCameraDecl::FovY].Parse (decl, "fov_y");
      node_decl->params [PerspectiveCameraDecl::ZNear].Parse (decl, "znear");
      node_decl->params [PerspectiveCameraDecl::ZFar].Parse (decl, "zfar");
    }
    
      //настройка камеры
      
    if (node_decl->params [PerspectiveCameraDecl::FovX].state)  node.SetFovX (degree (node_decl->params [PerspectiveCameraDecl::FovX].value));
    if (node_decl->params [PerspectiveCameraDecl::FovY].state)  node.SetFovY (degree (node_decl->params [PerspectiveCameraDecl::FovY].value)); 
    if (node_decl->params [PerspectiveCameraDecl::ZNear].state) node.SetZNear (node_decl->params [PerspectiveCameraDecl::ZNear].value);
    if (node_decl->params [PerspectiveCameraDecl::ZFar].state)  node.SetZFar (node_decl->params [PerspectiveCameraDecl::ZFar].value);

      //разбор родительских параметров

    Parse (decl, static_cast<Camera&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,PerspectiveCamera&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Light& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Light&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, DirectLight& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Light&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,DirectLight&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, SpotLight& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Light&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,SpotLight&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, PointLight& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Light&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,PointLight&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, VisualModel& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Entity&> (node), parent, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,VisualModel&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, TextLine& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,TextLine&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Sprite& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Sprite&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, Listener& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,Listener&,Node&,SceneContext&)");
    throw;
  }
}

void XmlSceneParser::Parse (const ParseNode& decl, SoundEmitter& node, Node& parent, SceneContext& context)
{
  try
  {
    Parse (decl, static_cast<Entity&> (node), parent, context);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::XmlSceneParser::Parse(const ParseNode&,SoundEmitter&,Node&,SceneContext&)");
    throw;
  }
}
