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
  TransformationsShaderParams params;

  IBuffer* cb = test.device->SSGetConstantBuffer (ConstantBufferSemantic_Transformations);

  if (!cb)
  {
    printf ("Null transformations constant buffer\n");
    return;
  }

  params.object_tm          = model.WorldTM ();
  params.model_view_tm      = transpose (view_tm * model.WorldTM ());
  params.model_view_proj_tm = transpose (view_projection_tm * model.WorldTM ());

  cb->SetData (0, sizeof params, &params);

  ModelMeshPtr mesh = test.mesh_manager.FindMesh (model.MeshName ());

  if (!mesh)
  {
    printf ("Can't find mesh '%s'\n", model.MeshName ());      
    return;
  }

  draw (*test.device, *mesh);
}
