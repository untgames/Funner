#include "scene_graph.h"

using namespace scene_graph;

template class engine::decl_sg_cast<VisualModel, Node>;
template class engine::decl_sg_cast<VisualModel, Entity>;
template class engine::decl_sg_cast<Scissor,     Entity>;
template class engine::decl_sg_cast<Scissor,     Node>;
template class engine::decl_sg_cast<Entity,      Node>;

namespace engine
{

namespace scene_graph_script_binds
{

/*
    �������� �����
*/

shared_ptr<Scene> create_scene ()
{
  return shared_ptr<Scene> (new Scene ());
}

Node::Pointer get_scene_root (Scene& scene)
{
  return &scene.Root ();
}

/*
   ����������� ���������� ������ �� ������
*/

void bind_scene_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_SCENE_LIBRARY);

    //����������� ������� ��������

  lib.Register ("Create", make_invoker (&create_scene));

    //����������� ��������

  lib.Register ("set_Name", make_invoker (&Scene::SetName));
  lib.Register ("get_Name", make_invoker (&Scene::Name));

  lib.Register ("get_Root", make_invoker (&get_scene_root));

  lib.Register ("get_EntitiesCount", make_invoker (&Scene::EntitiesCount));

  environment.RegisterType<Scene> (SCENE_SCENE_LIBRARY);
}

/*
    ����������� ���������� ������ � ��������� �����
*/

void bind_entity_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_ENTITY_LIBRARY);

    //������������

  lib.Register (environment, SCENE_NODE_LIBRARY);

    //����������� ��������

  lib.Register ("set_WireColor",  make_invoker (implicit_cast<void (Entity::*) (const vec3f&)> (&Entity::SetWireColor)));
  lib.Register ("get_WireColor",  make_invoker (&Entity::WireColor));

  lib.Register ("SetWireColor", make_invoker (implicit_cast<void (Entity::*) (float, float, float)> (&Entity::SetWireColor)));
  lib.Register ("set_Visible", make_invoker (&Entity::SetVisible));
  lib.Register ("get_Visible", make_invoker (&Entity::IsVisible));

  lib.Register ("get_BoundBox",      make_invoker (&Entity::BoundBox));
  lib.Register ("get_WorldBoundBox", make_invoker (&Entity::WorldBoundBox));

  lib.Register ("get_InfiniteBounds", make_invoker (&Entity::IsInfiniteBounds));

  lib.Register ("get_ChildrenBoundBox",      make_invoker (&Entity::ChildrenBoundBox));
  lib.Register ("get_FullBoundBox",          make_invoker (&Entity::FullBoundBox));
  lib.Register ("get_WorldChildrenBoundBox", make_invoker (&Entity::WorldChildrenBoundBox));
  lib.Register ("get_WorldFullBoundBox",     make_invoker (&Entity::WorldFullBoundBox));
  lib.Register ("GetIntersections",          make_invoker (make_invoker ((void (Entity::*)(NodeArray&) const)&Entity::GetIntersections),
                                                           make_invoker ((NodeArray (Entity::*)() const)&Entity::GetIntersections)));

    //����������� ����� ������

  environment.RegisterType<Entity> (SCENE_ENTITY_LIBRARY);
}

void bind_visual_model_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_VISUAL_MODEL_LIBRARY);

    //������������

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //����������� ��������

  lib.Register ("set_Scissor", make_invoker (&VisualModel::SetScissor));
  lib.Register ("get_Scissor", make_invoker (implicit_cast<Scissor* (VisualModel::*)()> (&VisualModel::Scissor)));

    //����������� ����� ������

  environment.RegisterType<VisualModel> (SCENE_VISUAL_MODEL_LIBRARY);
}

void bind_scissor_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (SCENE_SCISSOR_LIBRARY);

    //������������

  lib.Register (environment, SCENE_ENTITY_LIBRARY);

    //����������� ����� ������

  environment.RegisterType<VisualModel> (SCENE_SCISSOR_LIBRARY);
}

}

}
