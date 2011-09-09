#include "../shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации техники
*/

struct ForEachLightTechnique::Impl
{
};

/*
    Конструктор / деструктор
*/

ForEachLightTechnique::ForEachLightTechnique (RenderManager& manager, common::ParseNode& node)
  : impl (new Impl)
{
}

ForEachLightTechnique::~ForEachLightTechnique ()
{
}

/*
    Обновление кадра
*/

void ForEachLightTechnique::UpdateFrameCore (Scene& scene, Frame& frame)
{
  try
  {
    if (!Camera ())
      return;
      
      //получение камеры
      
    scene_graph::Camera& camera = *Camera ();
    
      //определение списка источников, попадающих в камеру
    
//    camera.Scene ()->Traverse (camera.Frustum (), );
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::ForEachLightTechnique::UpdateFrameCore");
    throw;
  }
}

/*
    Обновление свойств
*/

void ForEachLightTechnique::ResetPropertiesCore ()
{
}

void ForEachLightTechnique::UpdatePropertiesCore (const common::PropertyMap&)
{
}
