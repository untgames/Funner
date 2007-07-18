#include "h/shared.h"

using namespace medialib;

ColladaImpl* ColladaEntityImpl::collada_impl;
stl::string  ColladaEntityImpl::file_name;

template ColladaLib <ColladaMesh>;
template ColladaLib <ColladaMaterial>;
template ColladaLib <ColladaEffect>;
template ColladaLib <ColladaLight>;
template ColladaLib <ColladaCamera>;
template ColladaLib <ColladaController>;
template ColladaLib <ColladaNode>;
template ColladaLib <ColladaVisualScene>;
template ColladaLib <ColladaAnimation>;
template ColladaLib <ColladaAnimationClip>;

Collada::Collada (ColladaImpl* source)
  : impl (source)
  { }

Collada::Collada (common::Parser::Log* log, const char* file_name)
{
  impl = new ColladaImpl; 
  
  impl->log = log;

  Parser p (*impl->log, file_name);

  Parser::Iterator root = p.Root ()->First("COLLADA");

  if(!root)
  {
    printf("Parse unsuccsesful\n");
    return;
  }

  if(!test (root, "version", "1.4.1"))
    impl->log->Warning(root, "Currently supported Collada version - 1.4.1, document uses other version (%s), some features may be not fully supported or unsupported", get<const char*> (root, "version", ""));    

  impl->parse_collada (root);  

  ColladaEntityImpl::file_name = file_name;

  impl->check_dependencies();

  for (size_t i = 0; i < impl->log->MessagesCount (); i++)
    printf ("%s\n", impl->log->Message(i));
}

Collada::~Collada ()
{
  delete impl;
}

ColladaEntity::ColladaEntity (const ColladaEntityImpl* source)
  : impl (source)
  { }

const char* ColladaEntity::EntityID () const
{
  return impl->id.c_str();
}

Collada ColladaEntity::Owner () const
{
  return ColladaWrappers::Create <Collada> (impl->collada_impl);
}

const char* ColladaEntity::ParseFileName () const
{
  return impl->file_name.c_str();
}

size_t ColladaEntity::ParseLineNumber () const
{
  return impl->line;
}

bool ColladaEntity::operator == (const ColladaEntity& right) const
{
  return impl->id == right.impl->id;
}

bool ColladaEntity::operator != (const ColladaEntity& right) const
{
  return impl->id != right.impl->id;
} 

template <> inline const vector<ColladaMeshImpl>&          ColladaImpl::SelectLib () const { return meshes; }
template <> inline const vector<ColladaMaterialImpl>&      ColladaImpl::SelectLib () const { return materials; }
template <> inline const vector<ColladaEffectImpl>&        ColladaImpl::SelectLib () const { return effects; }
template <> inline const vector<ColladaLightImpl>&         ColladaImpl::SelectLib () const { return lights; }
template <> inline const vector<ColladaCameraImpl>&        ColladaImpl::SelectLib () const { return cameras; }
template <> inline const vector<ColladaControllerImpl>&    ColladaImpl::SelectLib () const { return controllers; }
template <> inline const vector<ColladaNodeImpl>&          ColladaImpl::SelectLib () const { return nodes; }
template <> inline const vector<ColladaVisualSceneImpl>&   ColladaImpl::SelectLib () const { return visual_scenes; }
template <> inline const vector<ColladaAnimationImpl>&     ColladaImpl::SelectLib () const { return animations; }
template <> inline const vector<ColladaAnimationClipImpl>& ColladaImpl::SelectLib () const { return animation_clips; }

template <class Item>
ColladaLib<Item>::ColladaLib (const Collada& source)
  : impl (source.impl)
  { }                                                  

template <class Item>
size_t ColladaLib<Item>::Count () const
{
  const vector<typename ColladaImplTraits<Item>::Impl>& v = impl->SelectLib<typename ColladaImplTraits<Item>::Impl> ();
  return v.size();
}

template <class Item>
Item ColladaLib<Item>::operator [] (size_t index) const
{
  const vector<typename ColladaImplTraits<Item>::Impl>& v = impl->SelectLib<typename ColladaImplTraits<Item>::Impl> ();
  if ((v.empty()) || (index >= v.size()))
  {
    RaiseOutOfRange ("ColladaLib::operator []", "index", index, (size_t)0, v.size()); 
    return ColladaWrappers::Create <Item> (&v[0]);
  }
  return ColladaWrappers::Create <Item> (&v[index]);
}

template <class Item>
Item ColladaLib<Item>::operator [] (const char* id) const
{
  const vector<typename ColladaImplTraits<Item>::Impl>& v = impl->SelectLib<typename ColladaImplTraits<Item>::Impl> ();
  for (size_t i = 0; i < v.size(); i++)
    if (v[i].id == id) return ColladaWrappers::Create <Item> (&v[i]);
  _RaiseSearchFailed ("ColladaLib::operator []");
  return ColladaWrappers::Create <Item> (&v[0]);
} 

ColladaVisualScene Collada::ActiveScene () const
{
  if(!impl->scene.empty())
    for (size_t i = 0; i < impl->visual_scenes.size(); i++)
      if (!strcmp(impl->visual_scenes[i].id.c_str(), (impl->scene.c_str() + 1)))
        return ColladaWrappers::Create <ColladaVisualScene> (&impl->visual_scenes[i]);
  _RaiseSearchFailed ("Collada::ActiveScene");
  return ColladaWrappers::Create <ColladaVisualScene> (&ColladaVisualSceneImpl());
}

void ColladaImpl::parse_collada (Parser::Iterator p)
{
  if (test (p, "scene.instance_visual_scene"))
  {
    scene = get<const char*> (p, "scene.instance_visual_scene.url");
    
    if (scene [0] != '#')
      log->Error(p->First("scene.instance_visual_scene"), "Wrong URL format, no '#' symbol.");
  } 

  for_each_child (p, "library_materials", bind (&ColladaImpl::parse_library_materials, this, _1));
  for_each_child (p, "library_geometries", bind (&ColladaImpl::parse_library_geometries, this, _1));


  for (size_t i = 0; i < meshes.size(); i++)
    for (size_t j = 0; j < meshes[i].surfaces.size(); j++)
      if (!SearchMaterial(&meshes[i].surfaces[j].material))
      {
        log->Error(ColladaEntityImpl::file_name.c_str(), meshes[i].line, "Mesh material %s not founded", meshes[i].surfaces[j].material.c_str());
        meshes[i].surfaces.erase(meshes[i].surfaces.begin() + j);
      }      

  for_each_child (p, "library_effects", bind (&ColladaImpl::parse_library_effects, this, _1));
  for_each_child (p, "library_images", bind (&ColladaImpl::parse_library_images, this, _1));
  for_each_child (p, "library_lights", bind (&ColladaImpl::parse_library_lights, this, _1));  
  for_each_child (p, "library_cameras", bind (&ColladaImpl::parse_library_cameras, this, _1));
  for_each_child (p, "library_controllers", bind (&ColladaImpl::parse_library_controllers, this, _1));
  for_each_child (p, "library_nodes", bind (&ColladaImpl::parse_library_nodes, this, _1));
  for_each_child (p, "library_visual_scenes", bind (&ColladaImpl::parse_library_visual_scenes, this, _1));
  for_each_child (p, "library_animations", bind (&ColladaImpl::parse_library_animations, this, _1));
  for_each_child (p, "library_animation_clips", bind (&ColladaImpl::parse_library_animation_clips, this, _1));  

  ColladaEntityImpl::collada_impl = this;
}

ColladaNodeImpl* ColladaImpl::SearchNodeID (const stl::string* id)
{
  ColladaVisualSceneImpl* root_scene = NULL;

  for (size_t i = 0; i < visual_scenes.size(); i++)
    if (visual_scenes[i].id == &scene[1])
    {
      root_scene = &visual_scenes[i];
      break;
    }

  if (!root_scene)
  {
    log->Error(ColladaEntityImpl::file_name.c_str(), 0, "Can't find root scene");    
    return NULL;
  }

  for (size_t i = 0; i < root_scene->nodes.size(); i++)
  {
    if (root_scene->nodes[i].id == *id)
      return &root_scene->nodes[i];
    else 
    {
      ColladaNodeImpl* result = SearchNodeID (&root_scene->nodes[i], id);
      if (result) return result;
    }
  }
  return NULL;
}

ColladaNodeImpl* ColladaImpl::SearchNodeSID (const stl::string* sid)
{
  ColladaVisualSceneImpl* root_scene = NULL;

  for (size_t i = 0; i < visual_scenes.size(); i++)
    if (visual_scenes[i].id == &scene[1])
    {
      root_scene = &visual_scenes[i];
      break;
    }

  if (!root_scene)
  {
    log->Error(ColladaEntityImpl::file_name.c_str(), 0, "Can't find root scene");    
    return NULL;
  }

  for (size_t i = 0; i < root_scene->nodes.size(); i++)
  {
    if (root_scene->nodes[i].sid == *sid)
      return &root_scene->nodes[i];
    else 
    {
      ColladaNodeImpl* result = SearchNodeSID (&root_scene->nodes[i], sid->c_str());
      if (result) return result;
    }
  }
  return NULL;
}

ColladaMaterialImpl* ColladaImpl::SearchMaterial (string* id)
{
  for (size_t i = 0; i < materials.size(); i++)
    if (materials[i].id == *id)
      return &materials[i];
  return NULL;
}

bool ColladaImpl::SearchMesh (string* id)
{
  for (size_t i = 0; i < meshes.size(); i++)
    if (meshes[i].id == *id)
      return true;
  return false;
}

ColladaEffectImpl* ColladaImpl::SearchEffect (string* id)
{
  for (size_t i = 0; i < effects.size(); i++)
    if (effects[i].id == *id)
      return &effects[i];
  return NULL;
}

bool ColladaImpl::SearchAnimation (string* id)
{
  for (size_t i = 0; i < animations.size(); i++)
    if (animations[i].id == *id)
      return true;
  return false;
}

bool ColladaImpl::SearchCamera (string* id)
{
  for (size_t i = 0; i < cameras.size(); i++)
    if (cameras[i].id == *id)
      return true;
  return false;
}

bool ColladaImpl::SearchLight (string* id)
{
  for (size_t i = 0; i < lights.size(); i++)
    if (lights[i].id == *id)
      return true;
  return false;
}

bool ColladaImpl::SearchController (string* id)
{
  for (size_t i = 0; i < controllers.size(); i++)
    if (controllers[i].id == *id)
      return true;
  return false;
}

bool ColladaImpl::check_anim_dependencies (ColladaAnimationImpl* animation)
{
  string node_id;
  size_t offset;

  for (size_t i = 0; i < animation->anim_channels.size(); i++)
  {
    offset = animation->anim_channels[i].target.find('/');
    if (offset != -1)
      node_id.assign(animation->anim_channels[i].target.begin(), animation->anim_channels[i].target.begin() + offset);
    if (!SearchNodeID(&node_id) && !SearchMaterial (&node_id) && !SearchMesh (&node_id) && !SearchEffect (&node_id) && 
        !SearchCamera (&node_id) && SearchLight (&node_id) && !SearchController (&node_id))
    {
      log->Error(ColladaEntityImpl::file_name.c_str(), animation->line, "No target for animation %s channel %d", animation->id.c_str(), i);
      return false;
    }
  }
  
  for (size_t i = 0; i < animation->child_animations.size(); i++)
    if (!check_anim_dependencies (&animation->child_animations[i]))
      animation->child_animations.erase (animation->child_animations.begin() + i);

  return true;
}

bool ColladaImpl::check_material_bind (ColladaInstanceMaterialImpl* i_material, ColladaNodeImpl* node)
{
  bool found_material = false;
  ColladaSurfaceImpl *material_surface;
  ColladaMaterialImpl *material;

  for (size_t i = 0; i < i_material->mesh->surfaces.size(); i++)
    if (i_material->mesh->surfaces[i].material == i_material->symbol)
    {
      found_material = true;
      material_surface = &i_material->mesh->surfaces[i];
    }

  if (!found_material)
  {
    log->Error(ColladaEntityImpl::file_name.c_str(), node->line, "No such material symbol %s in mesh found", i_material->symbol.c_str());
    return false;
  }

  material = SearchMaterial(&i_material->target);
  if (!material)
  {
    log->Error(ColladaEntityImpl::file_name.c_str(), node->line, "No such material  %s found", i_material->target.c_str());
    return false;
  }

  for (size_t i = 0; i < i_material->binds.size(); i++)
  {
    found_material = false;
    ColladaEffectImpl* effect;

    for (size_t j = 0; j < material_surface->tex_data.size(); j++)
      if (material_surface->tex_data[j].channel_name == i_material->binds[i].target)
        found_material = true;

    if (!found_material)
    {
      log->Error(ColladaEntityImpl::file_name.c_str(), node->line, "No such texture target %s in surface found", i_material->binds[i].target.c_str());
      return false;
    }

    effect = SearchEffect(&material->effect);
    
    size_t map_index = 0;
    
    for (;map_index<COLLADA_MAP_NUM;map_index++)
      if (effect->shader.maps [map_index].texture.texcoord == i_material->binds [i].semantic)
        break;
    
    if (map_index == COLLADA_MAP_NUM)
    {
      log->Error (ColladaEntityImpl::file_name.c_str (),node->line,"No such semantic %s in effect found",i_material->binds [i].semantic.c_str ());
      return false;
    }
  }

  return true;
}

bool ColladaImpl::check_node_dependencies (ColladaNodeImpl* node)
{
  bool correct_node = true;

  for (size_t i = 0; i < node->instance_camera.size(); i++)
    if (!SearchCamera(&node->instance_camera[i]))
    {
      log->Error(ColladaEntityImpl::file_name.c_str(), node->line, "Instantiated camera %s not found", node->instance_camera[i].c_str());
      correct_node = false;
    }

  for (size_t i = 0; i < node->instance_light.size(); i++)
    if (!SearchLight(&node->instance_light[i]))
    {
      log->Error(ColladaEntityImpl::file_name.c_str(), node->line, "Instantiated light %s not found", node->instance_light[i].c_str());
      correct_node =  false;
    }

  for (size_t i = 0; i < node->instance_geometry.size(); i++)
  {
    if (!SearchMesh(&node->instance_geometry[i].url))
    {
      log->Error(ColladaEntityImpl::file_name.c_str(), node->line, "Instantiated mesh %s not found", node->instance_geometry[i].url.c_str());
      correct_node = false;
    }

    for (size_t j = 0; j < node->instance_geometry[i].bind.materials.size(); j++)
    {
      if (!check_material_bind(&node->instance_geometry[i].bind.materials[j], node))
        correct_node = false;
    }  
  }

  for (size_t i = 0; i < node->instance_controller.size(); i++)
  {
    if (!SearchController(&node->instance_controller[i].url))
    {
      log->Error(ColladaEntityImpl::file_name.c_str(), node->line, "Instantiated controller %s not found", node->instance_controller[i].url.c_str());
      correct_node = false;
      continue;
    }

    for (size_t j = 0; j < node->instance_controller[i].bind.materials.size(); j++)
    {
      if (!check_material_bind(&node->instance_controller[i].bind.materials[j], node))
        correct_node = false;
    }  
  }

  for (size_t i = 0; i < node->child_nodes.size(); i++)
    if (!check_node_dependencies (&node->child_nodes[i]))
      node->child_nodes.erase (node->child_nodes.begin() + i--);

  if (correct_node)
    return true;
  return false;
}

void ColladaImpl::check_dependencies ()
{
  for (size_t i = 0; i < materials.size(); i++)
    if (!SearchEffect(&materials[i].effect))
    {
      log->Error(ColladaEntityImpl::file_name.c_str(), materials[i].line, "Material effect %s not founded", materials[i].effect.c_str());
      materials.erase(materials.begin() + i--);
    }
 
  for (size_t i = 0; i < animations.size(); i++)
    if (!check_anim_dependencies (&animations[i]))
      animations.erase(animations.begin() + i--);
  
  for (size_t i = 0; i < animation_clips.size(); i++)
    for (size_t j = 0; j < animation_clips[i].instances_anim.size(); j++)
      if (!SearchAnimation(&animation_clips[i].instances_anim[j]))
      {
        log->Error(ColladaEntityImpl::file_name.c_str(), animation_clips[i].line, "Animation clip element %s not founded", animation_clips[i].instances_anim[j].c_str());
        animation_clips[i].instances_anim.erase(animation_clips[i].instances_anim.begin () + j--);
      }

  for (size_t i = 0; i < controllers.size(); i++)
  {
    bool correct_controller = true;
    if (controllers[i].type == COLLADA_CONTROLLER_MORPH)
    {
      if (!SearchMesh(&controllers[i].morph.base_mesh))
      {
        log->Error(ColladaEntityImpl::file_name.c_str(), controllers[i].line, "Controller base mesh %s not founded", controllers[i].morph.base_mesh.c_str());
        correct_controller = false;
      }
      for (size_t j = 0; j < controllers[i].morph.targets.size(); j++)
        if (!SearchMesh(&controllers[i].morph.targets[j]))
        {
          log->Error(ColladaEntityImpl::file_name.c_str(), controllers[i].line, "Controller target mesh %s not founded", controllers[i].morph.targets[j].c_str());
          correct_controller = false;
        }
      if (!correct_controller)
        controllers.erase(controllers.begin () + i--);
    }
    else
    {
      if (!SearchMesh(&controllers[i].skin.base_mesh))
      {
        log->Error(ColladaEntityImpl::file_name.c_str(), controllers[i].line, "Controller base mesh %s not founded", controllers[i].skin.base_mesh.c_str());
        correct_controller = false;
      }
      for (size_t j = 0; j < controllers[i].skin.joints.size(); j++)
        if (!SearchNodeSID(&controllers[i].skin.joints[j]))
        {
          log->Error(ColladaEntityImpl::file_name.c_str(), controllers[i].line, "Controller joint %s not founded", controllers[i].skin.joints[j].c_str());
          correct_controller = false;
        }
      if (!correct_controller)
        controllers.erase(controllers.begin () + i--);
    }
  }      

  for (size_t i = 0; i < nodes.size(); i++)
    if (!check_node_dependencies(&nodes[i]))
      nodes.erase(nodes.begin() + i--);

  for (size_t i = 0; i < visual_scenes.size(); i++)
    for (size_t j = 0; j < visual_scenes[i].nodes.size(); j++)
      if (!check_node_dependencies(&visual_scenes[i].nodes[j]))
        visual_scenes[i].nodes.erase(visual_scenes[i].nodes.begin() + j--);
}

void _RaiseException(const char* source, const char* description)
{
  Exception exception(description);

  exception.Raise (source);
}

void _RaiseSearchFailed(const char* source)
{
  Exception exception("Element not founded");

  exception.Raise (source);
}
