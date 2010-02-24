#include "shared.h"

SceneRenderer::SceneRenderer (Test& in_test)
  : test (in_test)
{
}

///Рисование сцены
void SceneRenderer::Draw (scene_graph::Camera& camera)
{
  if (!camera.Scene ())
    return;

  view_tm            = inverse (camera.WorldTM ());
  view_projection_tm = camera.ProjectionMatrix () * view_tm;

  camera.Scene ()->VisitEach (*this);
}

void SceneRenderer::visit (scene_graph::VisualModel& model)
{
  MyShaderParameters params;

  IBuffer* cb = test.device->SSGetConstantBuffer (0);

  if (!cb)
  {
    printf ("Null #0 constant buffer\n");
    return;
  }

  params.object_tm = model.WorldTM ();
  params.view_tm   = view_tm;
  params.proj_tm   = view_projection_tm;

  cb->SetData (0, sizeof params, &params);

  ModelMeshPtr mesh = test.mesh_manager.FindMesh (model.MeshName ());

  if (!mesh)
  {
    printf ("Can't find mesh '%s'\n", model.MeshName ());      
    return;
  }

  draw (*test.device, *mesh);
}
