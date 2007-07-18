#include "h/shared.h"

ColladaMaterial::ColladaMaterial (const ColladaMaterialImpl* source)
  : ColladaEntity(source), impl (source)
  { }

ColladaEffect ColladaMaterial::Effect () const
{
  for (size_t i = 0; i < impl->collada_impl->effects.size (); i++)
    if (impl->collada_impl->effects[i].id == impl->effect)
      return ColladaWrappers::Create <ColladaEffect> (&impl->collada_impl->effects[i]);
  _RaiseSearchFailed ("ColladaMaterial::Effect");
  return ColladaWrappers::Create <ColladaEffect> (&impl->collada_impl->effects[0]);
}

void ColladaImpl::parse_library_materials (Parser::Iterator p)
{
  if(!p->Present("material"))
  {
    log->Error(p, "Uncorrect 'library_materials' tag. Must be at least one 'material' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("material"); i; i++)
    parse_material (i);
}

void ColladaImpl::parse_material (Parser::Iterator p)
{
  Parser::NamesakeIterator i = p->First("instance_effect");

  if (i)
  {
    materials.resize(materials.size() + 1);
    p->Read ("id", materials.back().id, "No id");
    materials.back().line = p->LineNumber();
    parse_instance_effect (i, &materials.back());

    if (++i)
      log->Error(p, "Detected more than one child 'instance_effect'.");
  }
}

void ColladaImpl::parse_instance_effect (Parser::Iterator p, ColladaMaterialImpl *destination)
{
  if (p->Present("url"))
    destination->effect = &(p->ReadString("url", "##"))[1];
  else
    destination->effect = "No effect";
}

