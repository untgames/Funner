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
  if(!p->Present("controller"))
  {
    log->Error(p, "Uncorrect 'library_controllers' tag. Must be at least one 'controller' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("controller"); i; i++)
  {
    controllers.resize(controllers.size() + 1);
    i->Read ("id", controllers.back().id, "No id");
    parse_controller (i, &controllers.back());
  }
}

void ColladaImpl::parse_controller (Parser::Iterator p, ColladaControllerImpl* destination)
{
  destination->line = p->LineNumber();
  if(p->Present("skin"))
  {
    destination->type = COLLADA_CONTROLLER_SKIN;
    parse_skin (p->First("skin"), &destination->skin);
  }
  else if (p->Present("morph")) 
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
  if(!p->Present("source"))
  {
    log->Error(p, "Uncorrect 'skin' tag, no 'source' attribute");
    return;
  }

  destination->base_mesh = p->ReadString ("source", "##") + 1;

  if(p->Present("bind_shape_matrix"))
    p->First("bind_shape_matrix")->Read ("#text", destination->bind_shape_matrix);

  if(!p->Present("vertex_weights") || !p->Present("joints"))
  {
    log->Error(p, "Syntax error. One of required subtag ('vertex_weights' or 'joints') was not detected.");
    return;
  }

  for(Parser::NamesakeIterator input = p->First("joints")->First("input"); input; input++)
  {
    if (input->Test("semantic", "JOINT"))
      parse_source (p, &destination->joints, &(input->ReadString("source", "##")[1]));
    else if (input->Test("semantic", "INV_BIND_MATRIX"))
      parse_source (p, &destination->inv_bind_matrices, &(input->ReadString("source", "##")[1]));
    else log->Error(input, "Unknown semantic.");
  }

  for(Parser::NamesakeIterator input = p->First("vertex_weights")->First("input"); input; input++)
    if (input->Test("semantic", "WEIGHT"))
      parse_source (p, &weights, &(input->ReadString("source", "##")[1]));

  p = p->First("vertex_weights");

  destination->vertex_influences.resize (p->ReadInt("count", 0)); 

  if(p->Present("vcount"))
  {
    Parser::AttributeReader v_counts = p->First("vcount")->Reader("#text");
    for(size_t i = 0; i < destination->vertex_influences.size (); i++)
    {
      v_counts.Read (destination->vertex_influences[i].weights_count);
      destination->vertex_influences[i].first_weight = cur_first_weight;
      cur_first_weight += destination->vertex_influences[i].weights_count;
    }
  }

  if(!p->Present("v"))
    return;

  destination->joint_weights.resize (cur_first_weight);
  Parser::AttributeReader indexes = p->First("v")->Reader("#text");
  for(size_t i = 0; i < cur_first_weight; i++)
  {
    indexes.Read (index_buf);
    destination->joint_weights[i].joint = index_buf + 1;
    indexes.Read (index_buf);
    destination->joint_weights[i].weight = weights[index_buf];
  }
}

void ColladaImpl::parse_morph (Parser::Iterator p, ColladaMorphImpl* destination)
{
  destination->line = p->LineNumber ();
  destination->base_mesh = p->ReadString("source", "##") + 1;
  if (p->Test("method", "RELATIVE"))
    destination->method = COLLADA_MORPH_RELATIVE;
  else destination->method = COLLADA_MORPH_NORMALIZED;
       
  if(!p->Present("targets"))
  {
    log->Error(p, "Required sub-tag 'targets' was not detected");
    return;
  }

  for(Parser::NamesakeIterator input = p->First("targets")->First("input"); input; input++)
  {
    if (input->Test("semantic", "MORPH_TARGET"))
      parse_source (p, &destination->targets, &(input->ReadString("source", "##")[1]));
    else if (input->Test("semantic", "MORPH_WEIGHT"))
      parse_source (p, &destination->weights, &(input->ReadString("source", "##")[1]));
    else log->Error(input, "Unknown semantic.");
  } 
}

