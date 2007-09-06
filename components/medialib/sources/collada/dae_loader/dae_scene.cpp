#include "shared.h"

using namespace math;

/*
    –азбор библиотеки визуальных сцен
*/

void DaeParser::ParseLibraryVisualScenes (Parser::Iterator iter)
{
  if (!test (iter, "visual_scene"))
  {
    parse_log.Warning (iter, "Incorrect 'library_visual_scenes' node. Must be at least one 'visual_scene' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "visual_scene", bind (&DaeParser::ParseVisualScene, this, _1));
}

/*
    –азбор визуальной сцены
*/

void DaeParser::ParseVisualScene (Parser::Iterator iter)
{
  const char *id   = get<const char*> (iter, "id"),
             *name = get<const char*> (iter, "name");
  
  LogScope scope (iter, *this);
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }

  Scene& scene = model.Scenes ().Create (id); 
  
  if (name)
    scene.SetName (name);
  
  for_each_child (iter, "node", bind (&DaeParser::ParseNode, this, _1, ref (scene)));
}

/*
    –азбор узла
*/

void DaeParser::ParseNode (Parser::Iterator iter, Node& parent)
{
  const char *id   = get<const char*> (iter, "id"),
             *sid  = get<const char*> (iter, "sid"),
             *name = get<const char*> (iter, "name");
  
  LogScope scope (iter, *this);
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }
  
    //разбор преобразований узла
    
  mat4f tm;
   
  if (!ParseTransform (iter, tm))
  {
    LogError (iter, "Error at parse node transformation sub-tags");
    return;
  }
  
  Node& node = model.Nodes ().Create (id);
  
    //биндинг к родительскому узлу
    
  parent.Nodes ().Insert (node);
  
    //установка дополнительного идентификатора

  if (sid)
    node.SetSubId (sid);
    
    //установка имени узла
    
  if (name)
    node.SetName (name);
    
    //установка преобразований узла
 
  node.SetTransform (tm);
  
    //разбор инстанцированной геометрии
    
  for_each_child (iter, "instance_geometry", bind (&DaeParser::ParseInstanceGeometry, this, _1, ref (node.Meshes ())));
  
    //разбор инстанцированных источников света
    
  for_each_child (iter, "instance_light", bind (&DaeParser::ParseInstanceLight, this, _1, ref (node.Lights ())));
  
    //разбор инстанцированных камер
    
  for_each_child (iter, "instance_camera", bind (&DaeParser::ParseInstanceCamera, this, _1, ref (node.Cameras ())));

    //разбор вложенных узлов

  for_each_child (iter, "node", bind (&DaeParser::ParseNode, this, _1, ref (node)));  
}

/*
    –азбор преобразований узла
*/

bool DaeParser::ParseTransform (Parser::Iterator iter, mat4f& tm)
{
  for (Parser::Iterator i=iter->First (); i; ++i)
  {
    if (test_tag (i, "matrix"))
    {
      mat4f sub_tm;
      
      if (!CheckedRead (i, "#text", sub_tm))
        return false;
        
      tm = sub_tm * tm;
    }
    else if (test_tag (i, "translate"))
    {
      vec3f offset;            
      
      if (!CheckedRead (i, "#text", offset))
        return false;        
        
      tm = translate (offset) * tm;
    }
    else if (test_tag (i, "rotate"))
    {
      vec4f r;
      
      if (!CheckedRead (i, "#text", r))
        return false;
        
      tm = rotatef (deg2rad (r.w), r.x, r.y, r.z) * tm;
    }
    else if (test_tag (i, "scale"))
    {
      vec3f s;      
      
      if (!CheckedRead (i, "#text", s))
        return false;        
        
      tm = scale (s) * tm;
    }
    else if (test_tag (i, "lookat"))
    {
      LogError (i, "Lookat transform doesn't supported");
      return false;
    }
    else if (test_tag (i, "skew"))
    {
      LogError (i, "Skew transform doesn't supported");
      return false;
    }
  }

  return true;
}

/*
    –азбор инстанцированного источника света
*/

void DaeParser::ParseInstanceLight (Parser::Iterator iter, Node::LightList& lights)
{
  const char* url = get<const char*> (iter, "url");
  
  if (!url)
  {
    LogError (iter, "No url. Error at parser 'instance_light'");
    return;
  }
  
  url++; //избавл€емс€ от префисного '#'
  
  Light* light = model.Lights ().Find (url);
  
  if (!light)
  {
    LogError (iter, "No light with id='%s' detected", url);
    return;
  }

  lights.Insert (*light);
}

/*
    –азбор инстанцированной камеры
*/

void DaeParser::ParseInstanceCamera (Parser::Iterator iter, Node::CameraList& cameras)
{
  const char* url = get<const char*> (iter, "url");
  
  if (!url)
  {
    LogError (iter, "No url. Error at parser 'instance_camera'");
    return;
  }
  
  url++; //избавл€емс€ от префисного '#'
  
  Camera* camera = model.Cameras ().Find (url);
  
  if (!camera)
  {
    LogError (iter, "No camera with id='%s' detected", url);
    return;
  }

  cameras.Insert (*camera);
}

/*
    –азбор инстанцированной геометрии
*/

void DaeParser::ParseInstanceGeometry (Parser::Iterator iter, Node::MeshList& meshes)
{
  const char* url = get<const char*> (iter, "url");
  
  if (!url)
  {
    LogError (iter, "No url. Error at parser 'instance_geometry'");
    return;
  }
  
  url++; //избавл€емс€ от префисного '#'
  
  Mesh* mesh = model.Meshes ().Find (url);
  
  if (!mesh)
  {
      //это может быть не меш
//    LogError (iter, "No mesh with id='%s' detected", url);
    return;
  }

  InstanceMesh& imesh = meshes.Create (*mesh);
  
    //разбор прикреплЄнных материалов
  
  for_each_child (iter, "bind_material", bind (&DaeParser::ParseBindMaterial, this, _1, ref (imesh.MaterialBinds ())));
}

/*
    –азбор прикреплЄнного материала
*/

void DaeParser::ParseBindMaterial (Parser::Iterator iter, MaterialBinds& binds)
{
  if (!test (iter, "technique_common"))
  {
    LogError (iter, "No 'technique_common' sub-tag");
    return;
  }
  
  for (Parser::NamesakeIterator i=iter->First ("technique_common.instance_material"); i; ++i)
  {
    const char *target = get<const char*> (i, "target"),
               *symbol = get<const char*> (i, "symbol");
    
    if (!target)
    {
      LogError (i, "No 'target' attribute");
      continue;
    }
    
    if (!symbol)
    {
      LogError (i, "No 'symbol' attribute");
      continue;
    }    
    
    target++; //избавл€емс€ от префисного '#'
    
    Material* material = model.Materials ().Find (target);
    
    if (!material)
    {
      LogError (i, "No material with id='%s' detected", target);
      continue;
    }
    
      //добавл€ем материал в список присоединЄнных материалов

    binds.SetMaterial (symbol, *material);

      //разбор текстурных каналов

    for (Parser::NamesakeIterator j=i->First ("bind_vertex_input"); j; ++j)
      if (test (j, "input_semantic", "TEXCOORD"))
      {
        const char *semantic = get<const char*> (j, "semantic"),
                   *set      = get<const char*> (j, "input_set");
                        
        if (!semantic)
        {
          LogError (j, "No 'semantic' attribute");
          continue;
        }
        
        if (!set)
        {
          LogError (j, "No 'input_set' attribute");
          continue;
        }

        binds.SetTexcoordChannelName (symbol, semantic, set);
      }
  }
}
