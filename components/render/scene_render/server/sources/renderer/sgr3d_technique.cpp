#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

/*
    Описание реализации техники рендеринга
*/

struct Technique::Impl
{
  stl::string                        name;               //имя техники
  scene_graph::Camera*               camera;             //текущая камера
  stl::auto_ptr<common::PropertyMap> default_properties; //свойства по умолчанию
  
///Конструктор
  Impl ()
    : camera ()
  {
  }
};

/*
    Конструктор / деструктор
*/

Technique::Technique ()
  : impl (new Impl)
{
}

Technique::~Technique ()
{
}

/*
    Имя техники
*/

void Technique::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene_render3d::Technique::SetName", "name");
    
  impl->name = name;
}

const char* Technique::Name ()
{
  return impl->name.c_str ();
}

/*
    Камера
*/

void Technique::SetCamera (scene_graph::Camera* camera)
{
  impl->camera = camera;
}

scene_graph::Camera* Technique::Camera ()
{
  return impl->camera;
}

/*
    Установка свойств по умолчанию
*/

void Technique::SetDefaultProperties (const common::PropertyMap& properties)
{
  if (!impl->default_properties.get ())
  {
    impl->default_properties = stl::auto_ptr<common::PropertyMap> (new common::PropertyMap (properties.Clone ()));
  }
  else
  {
    *impl->default_properties = properties.Clone ();
  }
}

void Technique::SetDefaultProperties (const common::ParseNode& node)
{
  try
  {
    SetDefaultProperties (to_properties (node));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Technique::SetDefaultProperties(const common::ParseNode&)");
    throw;
  }
}

const common::PropertyMap Technique::DefaultProperties ()
{
  if (!impl->default_properties.get ())
    return common::PropertyMap ();
    
  return *impl->default_properties;
}


/*
    Обновление свойств ренедринга
*/

void Technique::UpdateProperties (const common::PropertyMap& properties)
{
  try
  {
    ResetPropertiesCore ();

    if (impl->default_properties.get () && impl->default_properties->Size ()) 
      UpdatePropertiesCore (*impl->default_properties);
      
    if (properties.Size ())
      UpdatePropertiesCore (properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Technique::UpdateProperties");
    throw;
  }
}

/*
    Отрисовка
*/

void Technique::UpdateFrame (Scene& scene, Frame& frame, ITraverseResultCache& traverse_result_cache)
{
  try
  {
    UpdateFrameCore (scene, frame, traverse_result_cache);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Technique::UpdateFrame");
    throw;
  }
}
