#include "shared.h"

using namespace math;

/*
    –азбор библиотеки визуальных сцен
*/

void DaeParser::ParseLibraryVisualScenes (Parser::Iterator iter)
{
  if (!iter->First ("visual_scene"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_visual_scenes' node. Must be at least one 'visual_scene' sub-tag");
    return;
  }

  for_each_child (*iter, "visual_scene", bind (&DaeParser::ParseVisualScene, this, _1));
}

/*
    –азбор визуальной сцены
*/

void DaeParser::ParseVisualScene (Parser::Iterator iter)
{
  const char *id   = get<const char*> (*iter, "id"),
             *name = get<const char*> (*iter, "name", "");

    //создание сцены

  Node scene;
  
  scene.SetId (id);  
  
  if (*name)
    scene.SetName (name);
  
  for_each_child (*iter, "node", bind (&DaeParser::ParseNode, this, _1, ref (scene)));
  
    //добавление сцены в библиотеку
    
  model.Scenes ().Insert (id, scene);
}

/*
    –азбор узла
*/

void DaeParser::ParseNode (Parser::Iterator iter, Node& parent)
{
  const char *id   = get<const char*> (*iter, "id"),
             *sid  = get<const char*> (*iter, "sid", ""),
             *name = get<const char*> (*iter, "name", "");

    //разбор преобразований узла

  mat4f tm;
   
  ParseTransform (iter, id, tm);
  
    //создание узла
  
  Node node;
  
  node.SetId (id);
  
    //биндинг к родительскому узлу
    
  parent.Nodes ().Add (node);
  
    //установка дополнительного идентификатора

  if (*sid)
    node.SetSubId (sid);
    
    //установка имени узла
    
  if (*name)
    node.SetName (name);
    
    //установка преобразований узла
 
  node.SetTransform (tm);
  
    //добавление узла в библиотеку
    
  model.Nodes ().Insert (id, node);  
  
    //разбор инстанцированной геометрии
    
  for_each_child (*iter, "instance_geometry", bind (&DaeParser::ParseInstanceGeometry, this, _1, ref (node.Meshes ())));
  
    //разбор инстанцированных контроллеров
    
  for_each_child (*iter, "instance_controller", bind (&DaeParser::ParseInstanceController, this, _1, ref (node.Controllers ())));
  
    //разбор инстанцированных источников света
    
  for_each_child (*iter, "instance_light", bind (&DaeParser::ParseInstanceLight, this, _1, ref (node.Lights ())));
  
    //разбор инстанцированных камер
    
  for_each_child (*iter, "instance_camera", bind (&DaeParser::ParseInstanceCamera, this, _1, ref (node.Cameras ())));

    //разбор вложенных узлов

  for_each_child (*iter, "node", bind (&DaeParser::ParseNode, this, _1, ref (node)));  
}

/*
    –азбор преобразований узла
*/

void DaeParser::ParseTransform (Parser::Iterator iter, const char* node_id, mat4f& tm)
{  
  for (Parser::Iterator i=iter->First (); i; ++i)
  {
    const char* name = i->Name ();
    
    if (!strcmp (name, "matrix"))
    {
      mat4f sub_tm;
      
      read (*i, "#text", sub_tm);
        
      tm = sub_tm * tm;

      //занесение ссылки на матрицу в карту семантик анимаций
      const char* sid = get<const char*> (*i, "sid", "");

      if (xtl::xstrlen (sid))
      {
        stl::string target_name (common::format ("%s/%s", node_id, sid));

        animation_semantics.insert_pair (target_name.c_str (), AnimationChannelSemantic_Transform);
      }
    }
    else if (!strcmp (name, "translate"))
    {
      vec3f offset = get<vec3f> (*i, "#text");

      tm = translate (offset) * tm;
    }
    else if (!strcmp (name, "rotate"))
    {
      vec4f r = get<vec4f> (*i, "#text");

      tm = rotate (degree (r.w), vec3f (r.x, r.y, r.z)) * tm;
    }
    else if (!strcmp (name, "scale"))
    {
      vec3f s = get<vec3f> (*i, "#text");

      tm = scale (s) * tm;
    }
    else if (!strcmp (name, "lookat"))
    {
      raise_parser_exception (*i, "Lookat transform doesn't supported");
    }
    else if (!strcmp (name, "skew"))
    {
      raise_parser_exception (*i, "Skew transform doesn't supported");
    }
  }
}

/*
    –азбор инстанцированного источника света
*/

void DaeParser::ParseInstanceLight (Parser::Iterator iter, Node::LightList& lights)
{
  const char* url = get<const char*> (*iter, "url");  

  url++; //избавл€емс€ от префисного '#'

  Light* light = model.Lights ().Find (url);

  if (!light)
    raise_parser_exception (*iter, "No light with id='%s' detected", url);

    //добавление источнкиа света в коллекцию узла

  lights.Add (*light);
}

/*
    –азбор инстанцированной камеры
*/

void DaeParser::ParseInstanceCamera (Parser::Iterator iter, Node::CameraList& cameras)
{
  const char* url = get<const char*> (*iter, "url");

  url++; //избавл€емс€ от префисного '#'

  Camera* camera = model.Cameras ().Find (url);
  
  if (!camera)
    raise_parser_exception (*iter, "No camera with id='%s' detected", url);

    //добавление камеры в коллекцию узла

  cameras.Add (*camera);
}

/*
    –азбор инстанцированной геометрии
*/

void DaeParser::ParseInstanceGeometry (Parser::Iterator iter, Node::MeshList& meshes)
{
  const char* url = get<const char*> (*iter, "url");  

  url++; //избавл€емс€ от префисного '#'

  Mesh* mesh = model.Meshes ().Find (url);

  if (!mesh)
    return;

  InstanceMesh imesh;

  imesh.SetMesh (url);

    //разбор прикреплЄнных материалов
  
  for_each_child (*iter, "bind_material", bind (&DaeParser::ParseBindMaterial, this, _1, ref (imesh.MaterialBinds ())));
  
    //добавление меша в коллекцию узла
    
  meshes.Add (imesh);
}

/*
    –азбор инстанцированного контроллера
*/

void DaeParser::ParseInstanceController (Parser::Iterator iter, Node::ControllerList& controllers)
{
  const char* url = get<const char*> (*iter, "url");

  url++; //избавл€емс€ от префисного '#'

  Skin*  skin  = model.Skins ().Find (url);
  Morph* morph = model.Morphs ().Find (url);

  if (!skin && !morph)
    raise_parser_exception (*iter, "No controller with id='%s' detected", url);

  InstanceController icontroller;
  
  icontroller.SetController (url);
  
    //разбор прикреплЄнных материалов
    
  for_each_child (*iter, "bind_material", bind (&DaeParser::ParseBindMaterial, this, _1, ref (icontroller.MaterialBinds ())));
  
    //разбор корней поиска соединений
    
  for (Parser::NamesakeIterator skeleton_iter=iter->First ("skeleton"); skeleton_iter; ++skeleton_iter)
  {
    const char* root_id = get<const char*> (*skeleton_iter, "#text");

    icontroller.InsertJointSearchRoot (root_id);
  }  

    //добавление контроллера в коллекцию узла

  controllers.Add (icontroller);
}

/*
    –азбор прикреплЄнного материала
*/

void DaeParser::ParseBindMaterial (Parser::Iterator iter, MaterialBinds& binds)
{
  common::ParseNode technique_common_node = get_first_child (*iter, "technique_common");
  
  for (Parser::NamesakeIterator i=technique_common_node.First ("instance_material"); i; ++i)
  {
    const char *target = get<const char*> (*i, "target"),
               *symbol = get<const char*> (*i, "symbol");

    target++; //избавл€емс€ от префисного '#'

    Material* material = model.Materials ().Find (target);
    
    if (!material)
      raise_parser_exception (*i, "No material with id='%s' detected", target);

      //добавл€ем материал в список присоединЄнных материалов

    binds.SetMaterial (symbol, target);

      //разбор текстурных каналов

    for (Parser::NamesakeIterator j=i->First ("bind_vertex_input"); j; ++j)
    {
      const char* input_semantic = get<const char*> (*j, "input_semantic", "");
      
      if (*input_semantic && !strcmp (input_semantic, "TEXCOORD"))
      {
        const char *semantic = get<const char*> (*j, "semantic"),
                   *set      = get<const char*> (*j, "input_set");

        binds.SetTexcoordChannelName (symbol, semantic, set);
      }
    }
  }
}
