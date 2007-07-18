#include "h/shared.h"

//To do: загрузка трансформаций сразу в матрицу

ColladaInstanceMaterial::ColladaInstanceMaterial (const ColladaInstanceMaterialImpl* source)
  : impl (source)
  { }

ColladaMaterial ColladaInstanceMaterial::Material ()
{
  for (size_t i = 0; i < impl->collada_impl->materials.size(); i++)
    if (impl->collada_impl->materials[i].id == impl->target)
      return ColladaWrappers::Create <ColladaMaterial> (&impl->collada_impl->materials[i]);
  _RaiseSearchFailed ("ColladaInstanceMaterial::Material");
  return ColladaWrappers::Create <ColladaMaterial> (&ColladaMaterialImpl());
}

const char* ColladaInstanceMaterial::TextureChannelName (const char* texture_name) const
{
  for (size_t i = 0; i < impl->binds.size(); i++)
    if (impl->binds[i].target == texture_name)
      return impl->binds[i].semantic.c_str();
  _RaiseSearchFailed ("ColladaInstanceMaterial::TextureChannelName");
  return NULL;
}

size_t ColladaInstanceMaterial::TextureChannel (const char* texture_name) const
{
  for (size_t i = 0; i < impl->mesh->surfaces.size(); i++)
    for (size_t j = 0; j < impl->mesh->surfaces[i].tex_data.size(); j++)
    {
      if(impl->mesh->surfaces[i].tex_data[j].channel_name == texture_name)
        return j;
    }
  _RaiseSearchFailed ("ColladaInstanceMaterial::TextureChannel");
  return 0;  
} 

ColladaInstanceController::ColladaInstanceController (const ColladaInstanceControllerImpl* source)
  : impl (source)
  { }

ColladaController ColladaInstanceController::Controller () const
{
  for (size_t i = 0; i < impl->collada_impl->controllers.size(); i++)
    if (impl->collada_impl->controllers[i].id == impl->url)
      return ColladaWrappers::Create <ColladaController> (&impl->collada_impl->controllers[i]);
  _RaiseSearchFailed ("ColladaInstanceController::Controller");
  return ColladaWrappers::Create <ColladaController> (&ColladaControllerImpl());
}

size_t ColladaInstanceController::SkeletonsCount () const
{
  return impl->bind.skeletons.size();
}

ColladaNodeImpl* ColladaImpl::SearchNodeID (ColladaNodeImpl* start, const stl::string* id)
{
  for(size_t i = 0; i < start->child_nodes.size(); i++)
  {
    if (start->child_nodes[i].id == *id) return &start->child_nodes[i];
    else
    {
      ColladaNodeImpl* result = SearchNodeID (&start->child_nodes[i], id);
      if (result) return result;
    }
  }
  return NULL;
}

ColladaNodeImpl* ColladaImpl::SearchNodeSID (ColladaNodeImpl* start, const char* sid)
{
  if (start->sid == sid) return start;

  for(size_t i = 0; i < start->child_nodes.size(); i++)
  {
    if (start->child_nodes[i].sid == sid) return &start->child_nodes[i];
    else
    {
      ColladaNodeImpl* result = SearchNodeSID (&start->child_nodes[i], sid);
      if (result) return result;
    }
  }

  return NULL;
}

ColladaNode ColladaInstanceController::Skeleton (size_t skeleton_index) const
{
  ColladaVisualSceneImpl* scene = NULL;

  if ((impl->bind.skeletons.empty()) || (skeleton_index >= impl->bind.skeletons.size()))
  {
    RaiseOutOfRange ("ColladaInstanceController::Skeleton", "skeleton_index", skeleton_index, (size_t)0, impl->bind.skeletons.size());
    return ColladaWrappers::Create <ColladaNode> (&ColladaNodeImpl());
  }

  for (size_t i = 0; i < impl->collada_impl->visual_scenes.size(); i++)
    if (impl->collada_impl->visual_scenes[i].id == &impl->collada_impl->scene[1]) scene = &impl->collada_impl->visual_scenes[i];
  if (!scene)
  {
    _RaiseException ("ColladaInstanceController::Skeleton", "Can't found root scene");
    return ColladaWrappers::Create <ColladaNode> (&ColladaNodeImpl());
  }

  for (size_t i = 0; i < scene->nodes.size(); i++)
  {
    if (scene->nodes[i].id == impl->bind.skeletons[skeleton_index])
      return ColladaWrappers::Create <ColladaNode> (&scene->nodes[i]);
    else 
    {
      ColladaNodeImpl* result = impl->collada_impl->SearchNodeID (&scene->nodes[i], &impl->bind.skeletons[skeleton_index]);
      if (result) return ColladaWrappers::Create <ColladaNode> (result);
    }
  }

  _RaiseSearchFailed ("ColladaInstanceController::Skeleton");
  return ColladaWrappers::Create <ColladaNode> (&ColladaNodeImpl());
}

ColladaNode ColladaInstanceController::Joint (const char* name) const
{
  ColladaVisualSceneImpl* scene = NULL;

  for (size_t i = 0; i < impl->collada_impl->visual_scenes.size(); i++)
    if (impl->collada_impl->visual_scenes[i].id == &impl->collada_impl->scene[1]) scene = &impl->collada_impl->visual_scenes[i];
  if (!scene)
  {
    _RaiseException ("ColladaInstanceController::Joint", "Can't found root scene");
    return ColladaWrappers::Create <ColladaNode> (&ColladaNodeImpl());
  }

  for (size_t skeleton_index = 0; skeleton_index < impl->bind.skeletons.size(); skeleton_index++)
    for (size_t i = 0; i < scene->nodes.size(); i++)
    {
      if (scene->nodes[i].id == impl->bind.skeletons[skeleton_index])
      {
        ColladaNodeImpl* sid_result = impl->collada_impl->SearchNodeSID (&scene->nodes[i], name);
        if (sid_result) return ColladaWrappers::Create <ColladaNode> (sid_result);
      }
      else 
      {
        ColladaNodeImpl* result = impl->collada_impl->SearchNodeID (&scene->nodes[i], &impl->bind.skeletons[skeleton_index]);
        if (result)
        {
          ColladaNodeImpl* sid_result = impl->collada_impl->SearchNodeSID (result, name);
          if (sid_result) return ColladaWrappers::Create <ColladaNode> (sid_result);
        }
      }
    }
  _RaiseSearchFailed ("ColladaInstanceController::Joint");
  return ColladaWrappers::Create <ColladaNode> (&ColladaNodeImpl());
}

ColladaNode::ColladaNode (const ColladaNodeImpl* source)
  : ColladaEntity(source), impl (source)
  { }

const char* ColladaNode::SubID () const
{
  return impl->sid.c_str();
}

const char* ColladaNode::Name () const
{
  return impl->name.c_str();
}

const mat4f& ColladaNode::GetTransform () const
{
  return impl->transform.matrix;
}

size_t ColladaNode::ChildrenCount () const
{
  return impl->child_nodes.size ();
}

ColladaNode ColladaNode::Child (size_t node_index) const
{
  if ((impl->child_nodes.empty()) || (node_index >= impl->child_nodes.size()))
  {
    RaiseOutOfRange ("ColladaNode::Child", "node_index", node_index, (size_t)0, impl->child_nodes.size());
    return ColladaWrappers::Create <ColladaNode> (&ColladaNodeImpl());
  }
  return ColladaWrappers::Create <ColladaNode> (&impl->child_nodes[node_index]);
}

size_t ColladaNode::LayersCount () const
{
  return impl->layers.size ();
}

const char* ColladaNode::Layer (size_t layer_index) const
{
  if ((impl->layers.empty()) || (layer_index >= impl->layers.size()))
  {
    RaiseOutOfRange ("ColladaNode::Layer", "layer_index", layer_index, (size_t)0, impl->layers.size());
    return NULL;
  }
  return impl->layers[layer_index].c_str();
}

size_t ColladaNode::CamerasCount () const
{
  return impl->instance_camera.size();
}

ColladaCamera ColladaNode::Camera (size_t camera_index) const
{
  if ((impl->instance_camera.empty()) || (camera_index >= impl->instance_camera.size())) 
  {
    RaiseOutOfRange ("ColladaNode::Camera", "camera_index", camera_index, (size_t)0, impl->instance_camera.size());
    return ColladaWrappers::Create <ColladaCamera> (&ColladaCameraImpl());
  }
  for (size_t i = 0; i < impl->collada_impl->cameras.size(); i++)
    if (impl->collada_impl->cameras[i].id == impl->instance_camera[camera_index])
      return ColladaWrappers::Create <ColladaCamera> (&impl->collada_impl->cameras[i]);

  _RaiseSearchFailed ("ColladaNode::Camera");
  return ColladaWrappers::Create <ColladaCamera> (&ColladaCameraImpl());
}

size_t ColladaNode::LightsCount () const
{
  return impl->instance_light.size ();
}

ColladaLight ColladaNode::Light (size_t light_index) const
{
  if ((impl->instance_light.empty()) || (light_index >= impl->instance_light.size()))
  {
    RaiseOutOfRange ("ColladaNode::Light", "light_index", light_index, (size_t)0, impl->instance_light.size());
    return ColladaWrappers::Create <ColladaLight> (&ColladaLightImpl());
  }
  for (size_t i = 0; i < impl->collada_impl->lights.size(); i++)
    if (impl->collada_impl->lights[i].id == impl->instance_light[light_index])
      return ColladaWrappers::Create <ColladaLight> (&impl->collada_impl->lights[i]);
  _RaiseSearchFailed ("ColladaNode::Light");
  return ColladaWrappers::Create <ColladaLight> (&ColladaLightImpl());
}

size_t ColladaNode::MeshesCount () const
{
  return impl->instance_geometry.size ();
}

ColladaMesh ColladaNode::Mesh (size_t mesh_index) const
{
  if ((impl->instance_geometry.empty()) || (mesh_index >= impl->instance_geometry.size()))
  {
    RaiseOutOfRange ("ColladaNode::Mesh", "mesh_index", mesh_index, (size_t)0, impl->instance_geometry.size());
    return ColladaWrappers::Create <ColladaMesh> (&ColladaMeshImpl());
  }
  for (size_t i = 0; i < impl->collada_impl->meshes.size(); i++)
    if (impl->collada_impl->meshes[i].id == impl->instance_geometry[mesh_index].url)
      return ColladaWrappers::Create <ColladaMesh> (&impl->collada_impl->meshes[i]);
  _RaiseSearchFailed ("ColladaNode::Mesh");
  return ColladaWrappers::Create <ColladaMesh> (&ColladaMeshImpl());
}

ColladaInstanceMaterial ColladaNode::MeshMaterial (size_t mesh_index,const char* material_name) const
{
  if ((impl->instance_geometry.empty()) || (mesh_index >= impl->instance_geometry.size()))
  {
    RaiseOutOfRange ("ColladaNode::MeshMaterial", "mesh_index", mesh_index, (size_t)0, impl->instance_geometry.size());
    return ColladaWrappers::Create <ColladaInstanceMaterial> (&ColladaInstanceMaterialImpl());
  }
  for (size_t i = 0; i < impl->instance_geometry[mesh_index].bind.materials.size(); i++)
    if (impl->instance_geometry[mesh_index].bind.materials[i].target == material_name)
      return ColladaWrappers::Create <ColladaInstanceMaterial> (&impl->instance_geometry[mesh_index].bind.materials[i]);
  _RaiseSearchFailed ("ColladaNode::MeshMaterial");
  return ColladaWrappers::Create <ColladaInstanceMaterial> (&ColladaInstanceMaterialImpl());
} 

size_t ColladaNode::ControllersCount () const
{
  return impl->instance_controller.size ();
}

ColladaInstanceController ColladaNode::Controller (size_t controller_index) const
{
  if ((impl->instance_controller.empty()) || (controller_index >= impl->instance_controller.size()))
  {
    RaiseOutOfRange ("ColladaNode::Controller", "controller_index", controller_index, (size_t)0, impl->instance_controller.size());
    return ColladaWrappers::Create <ColladaInstanceController> (&ColladaInstanceControllerImpl());
  }
  return ColladaWrappers::Create <ColladaInstanceController> (&impl->instance_controller[controller_index]);
} 
  
void ColladaImpl::parse_library_nodes (Parser::Iterator p)
{
  if(!test (p, "node"))
  {
    log->Error(p, "Uncorrect 'library_nodes' tag. Must be at least one 'node' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("node"); i; i++)
  {
    nodes.resize(nodes.size() + 1);
    parse_node (i, &nodes.back());
  }
}

void ColladaImpl::parse_node (Parser::Iterator p, ColladaNodeImpl* destination)
{
  string layer_buf;
  size_t cur_pos = 0;

  destination->line = p->LineNumber();
  read (p, "id", destination->id, "No id");
  read (p, "sid", destination->sid);
  read (p, "name", destination->name);
  if (test (p, "type", "JOINT"))
    destination->type = JOINT;
  else
    destination->type = NODE;

  read (p, "layer", layer_buf);
  if(!layer_buf.empty())
  {
    for(; (cur_pos != -1) && (cur_pos != layer_buf.size ()) && (layer_buf[cur_pos] != ' '); cur_pos = layer_buf.find (' ', cur_pos) + 1)
    {
      size_t end_pos = layer_buf.find (' ', cur_pos);

      destination->layers.resize(destination->layers.size() + 1);
      if (end_pos == -1)
        destination->layers.back().assign (layer_buf.begin() + cur_pos, layer_buf.end());
      else
        destination->layers.back().assign (layer_buf.begin() + cur_pos, layer_buf.begin() + end_pos + 1);
    }
    if (cur_pos != -1)
      log->Error (p, "Uncorrect 'layer' subtag. Detected more than one separator in a row or space at the end");
  }

  if(test (p, "lookat"))
    read_range (p, "lookat.#text", &(destination->transform.lookat[0]), 3);
  else
    destination->transform.lookat[0] = destination->transform.lookat[1] = destination->transform.lookat[2] = 0;
    
  read (p, "matrix.#text", destination->transform.matrix, mat4f (1.0f));
  read (p, "rotate.#text", destination->transform.rotate);
  read (p, "scale.#text", destination->transform.rotate, vec4f (1.0f));
  read (p, "translate.#text", destination->transform.rotate);  

  if(test (p, "skew"))
  {
    Parser::AttributeIterator attr_reader = make_attribute_iterator (p, "skew.#text");
    
    read (attr_reader, destination->transform.skew.angle, 0.0f);
    read (attr_reader, destination->transform.skew.rotate_axis, vec3f (0.0f));
    read (attr_reader, destination->transform.skew.translate_axis, vec3f (0.0f));
  }
  else
    destination->transform.skew.rotate_axis = destination->transform.skew.translate_axis = destination->transform.skew.angle = 0;

  for(Parser::NamesakeIterator i = p->First("instance_camera"); i; i++)
    destination->instance_camera.push_back (&(get<const char*> (i, "url")[1]));
    
  for(Parser::NamesakeIterator i = p->First("instance_light"); i; i++)
    destination->instance_light.push_back (&(get<const char*> (i, "url")[1]));
  if(test (p, "instance_node"))
//    destination->instance_node = &(attrs(p->First("instance_node"), "url")[1]);
    log->Warning(p, "instance_node currently unsupported.");
  for(Parser::NamesakeIterator i = p->First("instance_geometry"); i; i++)
  {
    destination->instance_geometry.resize (destination->instance_geometry.size() + 1);
    destination->instance_geometry.back().url = &(get<const char*> (i, "url")[1]);
    if(test (i, "bind_material"))
      parse_instance_materials (i->First("bind_material"), &destination->instance_geometry.back().bind.materials, &destination->instance_geometry.back().url);
  }

  for(Parser::NamesakeIterator i = p->First("instance_controller"); i; i++)
  {
    destination->instance_controller.resize (destination->instance_controller.size () + 1);
    destination->instance_controller.back().url = &(get<const char*> (i, "url")[1]);

    if(i->First("bind_material"))
    {
      parse_instance_materials (i->First("bind_material"), &destination->instance_controller.back().bind.materials, NULL);

      for (size_t j = 0; j < controllers.size(); j++)
      {
        if (controllers[j].id == destination->instance_controller.back().url)
        {
          if (controllers[j].type == COLLADA_CONTROLLER_SKIN)
          {
            for (size_t k = 0; k < meshes.size(); k++)
              if (meshes[k].id == controllers[j].skin.base_mesh)
              {
                destination->instance_controller.back().bind.materials.back().mesh = &meshes[k];
                break;
              }
          }
          else
          {
            for (size_t k = 0; k < meshes.size(); k++)
              if (meshes[k].id == controllers[j].morph.base_mesh)
              {
                destination->instance_controller.back().bind.materials.back().mesh = &meshes[k];
                break;
              }
          }
          break; 
        }
      }
    }

    for(Parser::NamesakeIterator j = i->First("skeleton"); j; j++)
      destination->instance_controller.back().bind.skeletons.push_back (&(get<const char*> (j, "#text")[1]));
  }

  for(Parser::NamesakeIterator i = p->First("node"); i; i++)
  {
    destination->child_nodes.resize(destination->child_nodes.size() + 1);
    parse_node (i, &(destination->child_nodes.back()));
  }
}

void ColladaImpl::parse_instance_materials (Parser::Iterator p, vector<ColladaInstanceMaterialImpl> *destination, string* mesh_url)
{
  if(test (p, "technique_common"))
    for(Parser::NamesakeIterator i = p->First("technique_common")->First("instance_material"); i; i++)
    {
      destination->resize(destination->size() + 1);
      destination->back().target = &(get<const char*> (i, "target")[1]);
      destination->back().symbol = get<const char*> (i, "symbol");
      for(Parser::NamesakeIterator j = i->First("bind"); j; j++)
      {
        destination->back().binds.resize(destination->back().binds.size() + 1);
        test (j, "semantic", destination->back().binds.back().semantic);
        test (j, "target", destination->back().binds.back().target);
      }
      if (mesh_url)
        for (size_t j = 0; j < meshes.size(); j++)
          if (meshes[j].id == *mesh_url)
          {
            destination->back().mesh = &meshes[j];
            break;
          }
    }
  else
    log->Error (p, "No 'technique_common' sub-tag.");
}
