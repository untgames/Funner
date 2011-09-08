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
  throw xtl::make_not_implemented_exception ("render::scene_render3d::ForEachLightTechnique::UpdateFrameCore");
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
