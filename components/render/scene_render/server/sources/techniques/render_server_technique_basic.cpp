#include "shared.h"

using namespace render;
using namespace render::scene;
using namespace render::scene::server;

/*
     онструктор / деструктор
*/

BasicTechnique::BasicTechnique (RenderManager& in_manager, const common::ParseNode& node)
  : manager (in_manager)
{
    //чтение имени эффекта

  effect_name = common::get<const char*> (node, "effect");
}    

BasicTechnique::~BasicTechnique ()
{
}

/*
    ќбновление кадра
*/

void BasicTechnique::UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder)
{
  try
  {
      //получение приватных данных техники

    PrivateData& private_data = private_data_holder.Get<PrivateData> (*this);

      //обновление кадра

    private_data.renderer->Draw (parent_context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::BasicTechnique::UpdateFrameCore");
    throw;
  }
}

/*
    ќбновление свойств / св€зывание свойств техники с методами техники
*/

void BasicTechnique::UpdatePropertiesCore ()
{
}

void BasicTechnique::BindProperties (common::PropertyBindingMap&)
{
}
