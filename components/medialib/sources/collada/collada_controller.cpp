#include "h/shared.h"

ColladaMorph::ColladaMorph (const ColladaMorphImpl* source)
  : ColladaEntity(source), impl (source)
  { }

ColladaMesh ColladaMorph::Mesh () const
{
  for (size_t i = 0; i < impl->collada_impl->meshes.size (); i++)
    if (impl->collada_impl->meshes[i].id == impl->base_mesh)
      return ColladaWrappers::Create <ColladaMesh> (&impl->collada_impl->meshes[i]);
  _RaiseSearchFailed ("ColladaMorph::Mesh");
  return ColladaWrappers::Create <ColladaMesh> (&ColladaMeshImpl());
}

ColladaMorphMethod ColladaMorph::Method () const
{
  return impl->method;
}

size_t ColladaMorph::TargetsCount () const
{
  return impl->targets.size();
}

float ColladaMorph::TargetWeight (size_t index) const
{
  if ((impl->weights.empty()) || (index >= impl->weights.size()))
  { 
    RaiseOutOfRange ("ColladaMorph::TargetWeight", "index", index, (size_t)0, impl->weights.size());
    return 0;
  }
  return impl->weights[index];
}

ColladaMesh ColladaMorph::TargetMesh (size_t index) const
{
  if ((impl->targets.empty()) || (index >= impl->targets.size()))
  {
    RaiseOutOfRange ("ColladaMorph::TargetMesh", "index", index, (size_t)0, impl->targets.size());
    return ColladaWrappers::Create <ColladaMesh> (&ColladaMeshImpl());
  }
  for (size_t i = 0; i < impl->collada_impl->meshes.size (); i++)
    if (impl->collada_impl->meshes[i].id == impl->targets[index])
      return ColladaWrappers::Create <ColladaMesh> (&impl->collada_impl->meshes[i]);
  _RaiseSearchFailed ("ColladaMorph::TargetMesh");
  return ColladaWrappers::Create <ColladaMesh> (&ColladaMeshImpl());
}

ColladaSkin::ColladaSkin (const ColladaSkinImpl* source)
  : ColladaEntity(source), impl (source)
  { }

ColladaMesh ColladaSkin::Mesh () const
{
  for (size_t i = 0; i < impl->collada_impl->meshes.size (); i++)
    if (impl->collada_impl->meshes[i].id == impl->base_mesh)
      return ColladaWrappers::Create <ColladaMesh> (&impl->collada_impl->meshes[i]);
  _RaiseSearchFailed ("ColladaSkin::Mesh");
  return ColladaWrappers::Create <ColladaMesh> (&ColladaMeshImpl());
}

const math::mat4f& ColladaSkin::BindShapeMatrix () const
{
  return impl->bind_shape_matrix;
}

size_t ColladaSkin::JointsCount () const
{
  return impl->joints.size();
}

const char* ColladaSkin::JointName (size_t joint_index) const
{
  if ((impl->joints.empty()) || (joint_index >= impl->joints.size()))
  {
    RaiseOutOfRange ("ColladaSkin::JointName", "joint_index", joint_index, (size_t)0, impl->joints.size());
    return NULL;
  }
  return impl->joints[joint_index].c_str();
}

const math::mat4f& ColladaSkin::JointInvMatrix (size_t joint_index) const
{
  if ((impl->inv_bind_matrices.empty()) || (joint_index >= impl->inv_bind_matrices.size()))
  {
    RaiseOutOfRange ("ColladaSkin::JointInvMatrix", "joint_index", joint_index, (size_t)0, impl->inv_bind_matrices.size());
    return impl->inv_bind_matrices[0];
  }
  return impl->inv_bind_matrices[joint_index];
}

size_t ColladaSkin::InfluencesCount () const //совпадает с количеством вершин в меше
{
  return impl->vertex_influences.size();
}

size_t ColladaSkin::WeightsCount () const 
{
  return impl->joint_weights.size();
}

const ColladaJointWeight*  ColladaSkin::Weights () const
{
  return impl->joint_weights.begin();
}

const ColladaVertexInfluence* ColladaSkin::Influences () const
{
  return impl->vertex_influences.begin();
}

ColladaController::ColladaController (const ColladaControllerImpl* source)
  : ColladaEntity(source), impl (source)
  { }

ColladaControllerType ColladaController::Type () const
{
  return impl->type;
}

ColladaMorph ColladaController::Morph () const
{
  if (impl->type != COLLADA_CONTROLLER_MORPH)
  {
    _RaiseException ("ColladaController::Morph", "Not morph controller");
    return ColladaWrappers::Create <ColladaMorph> (&ColladaMorphImpl());
  }
  return ColladaWrappers::Create <ColladaMorph> (&impl->morph);
}

ColladaSkin ColladaController::Skin  () const
{
  if (impl->type != COLLADA_CONTROLLER_SKIN)
  {
    _RaiseException ("ColladaController::Skin", "Not skin controller");
    return ColladaWrappers::Create <ColladaSkin> (&ColladaSkinImpl());
  }
  return ColladaWrappers::Create <ColladaSkin> (&impl->skin);
}

void ColladaImpl::parse_library_controllers (Parser::Iterator p)
{
  if(!test (p,  "controller"))
  {
    log->Error(p, "Uncorrect 'library_controllers' tag. Must be at least one 'controller' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("controller"); i; i++)
  {
    controllers.resize(controllers.size() + 1);
    read (i, "id", controllers.back().id, "No id");
    parse_controller (i, &controllers.back());
  }
}

void ColladaImpl::parse_controller (Parser::Iterator p, ColladaControllerImpl* destination)
{
  destination->line = p->LineNumber();
  if(test (p,  "skin"))
  {
    destination->type = COLLADA_CONTROLLER_SKIN;
    parse_skin (p->First("skin"), &destination->skin);
  }
  else if (test (p,  "morph")) 
  {
    destination->type = COLLADA_CONTROLLER_MORPH;
    parse_morph (p->First("morph"), &destination->morph);
  }
  else 
  {
    log->Error(p, "Uncorrect 'controller' tag, no expected sub-tag (one of 'skin' or 'morph')");
    return;
  }
}

void ColladaImpl::parse_skin (Parser::Iterator p, ColladaSkinImpl* destination)
{
  size_t cur_first_weight = 0;
  int    index_buf;
  stl::vector <float> weights;

  destination->line = p->LineNumber ();
  if(!test (p,  "source"))
  {
    log->Error(p, "Uncorrect 'skin' tag, no 'source' attribute");
    return;
  }

  destination->base_mesh = get<const char*> (p, "source", "##") + 1;

  if(test (p,  "bind_shape_matrix"))
    read (p, "bind_shape_matrix.#text", destination->bind_shape_matrix);

  if(!test (p,  "vertex_weights") || !test (p,  "joints"))
  {
    log->Error(p, "Syntax error. One of required subtag ('vertex_weights' or 'joints') was not detected.");
    return;
  }

  for(Parser::NamesakeIterator input = p->First("joints")->First("input"); input; input++)
  {
    if (test (input, "semantic", "JOINT"))
      parse_source (p, &destination->joints, &(get<const char*> (input, "source", "##")[1]));
    else if (test (input, "semantic", "INV_BIND_MATRIX"))
      parse_source (p, &destination->inv_bind_matrices, &(get<const char*> (input, "source", "##")[1]));
    else log->Error(input, "Unknown semantic.");
  }

  for(Parser::NamesakeIterator input = p->First("vertex_weights.input"); input; input++)
    if (test (input, "semantic", "WEIGHT"))
      parse_source (p, &weights, &(get<const char*> (input, "source", "##")[1]));

  p = p->First("vertex_weights");

  destination->vertex_influences.resize (get<int> (p, "count")); 

  if(test (p,  "vcount"))
  {
    Parser::AttributeIterator v_counts = make_attribute_iterator (p, "vcount.#text");
    for(size_t i = 0; i < destination->vertex_influences.size (); i++)
    {
      read (v_counts, destination->vertex_influences[i].weights_count);
      destination->vertex_influences[i].first_weight = cur_first_weight;
      cur_first_weight += destination->vertex_influences[i].weights_count;
    }
  }

  if(!test (p,  "v"))
    return;

  destination->joint_weights.resize (cur_first_weight);
  Parser::AttributeIterator indexes = make_attribute_iterator (p, "v.#text");
  for(size_t i = 0; i < cur_first_weight; i++)
  {
    read (indexes, index_buf);
    destination->joint_weights[i].joint = index_buf + 1;
    read (indexes, index_buf);
    destination->joint_weights[i].weight = weights[index_buf];
  }
}

void ColladaImpl::parse_morph (Parser::Iterator p, ColladaMorphImpl* destination)
{
  destination->line = p->LineNumber ();
  destination->base_mesh = get<const char*> (p, "source", "##") + 1;
  if (test (p, "method", "RELATIVE"))
    destination->method = COLLADA_MORPH_RELATIVE;
  else destination->method = COLLADA_MORPH_NORMALIZED;
       
  if(!test (p,  "targets"))
  {
    log->Error(p, "Required sub-tag 'targets' was not detected");
    return;
  }

  for(Parser::NamesakeIterator input = p->First("targets.input"); input; input++)
  {
    if (test (input, "semantic", "MORPH_TARGET"))
      parse_source (p, &destination->targets, &(get<const char*> (input, "source", "##")[1]));
    else if (test (input, "semantic", "MORPH_WEIGHT"))
      parse_source (p, &destination->weights, &(get<const char*> (input, "source", "##")[1]));
    else log->Error(input, "Unknown semantic.");
  } 
}

