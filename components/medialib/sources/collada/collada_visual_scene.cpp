#include "h/shared.h"

ColladaVisualScene::ColladaVisualScene (const ColladaVisualSceneImpl* source)
  : ColladaEntity(source), impl (source)
  { }

size_t ColladaVisualScene::NodesCount ()
{
  return impl->nodes.size();
}

ColladaNode ColladaVisualScene::Node (size_t index)
{
  if ((impl->nodes.empty()) || (index >= impl->nodes.size())) 
  {
    RaiseOutOfRange ("ColladaVisualScene::Node", "index", index, (size_t)0, impl->nodes.size());
    return ColladaWrappers::Create <ColladaNode> (&ColladaNodeImpl());;
  }
  return ColladaWrappers::Create <ColladaNode> (&impl->nodes[index]);
}

ColladaNode ColladaVisualScene::Node (const char* name)
{
  for (size_t i = 0; i < impl->nodes.size(); i++)
    if (impl->nodes[i].id == name)
      return ColladaWrappers::Create <ColladaNode> (&impl->nodes[i]);
  _RaiseSearchFailed ("ColladaVisualScene::Node");
  return ColladaWrappers::Create <ColladaNode> (&ColladaNodeImpl());;
}

void ColladaImpl::parse_library_visual_scenes (Parser::Iterator p)
{
  if(!p->Present("visual_scene"))
  {
    log->Error(p, "Uncorrect 'library_visual_scenes' tag. Must be at least one 'visual_scene' sub-tag");
    return;
  }
  for (Parser::NamesakeIterator i = p->First("visual_scene"); i; i++)
    parse_visual_scene (i);
}

void ColladaImpl::parse_visual_scene (Parser::Iterator p)
{
  visual_scenes.resize(visual_scenes.size() + 1);
  p->Read ("id", visual_scenes.back().id, "No id");
  visual_scenes.back().line = p->LineNumber();

  for(Parser::NamesakeIterator i = p->First("node"); i; i++)
  {
    visual_scenes.back().nodes.resize(visual_scenes.back().nodes.size() + 1);
    parse_node(i, &visual_scenes.back().nodes.back());
  }
}
  