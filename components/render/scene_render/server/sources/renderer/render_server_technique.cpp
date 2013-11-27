#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    ќписание реализации техники рендеринга
*/

struct Technique::Impl
{
  stl::string                                           name;                            //им€ техники
  common::PropertyMap                                   properties;                      //свойства
  common::PropertyMap                                   default_properties;              //свойства по умолчанию
  common::PropertyMap                                   binded_properties;               //значени€ свойств после св€зывани€
  common::PropertyBindingMap                            property_bindings;               //св€зывание методом техники со свойствами
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> binded_properties_synchronizer;  //синхронизатор свойств после св€зывани€
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> default_properties_synchronizer; //синхронизатор свойств по умолчанию
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> properties_synchronizer;         //синхронизатор свойств
  bool                                                  need_update_properties;          //флаг необходимости обновлени€ свойств
  bool                                                  need_bind_properties;            //флаг необходимости биндинга свойств
  xtl::auto_connection                                  properties_update_connection;    //соединение с сигналом обновлени€ свойств
  common::PropertyMap::EventHandler                     properties_update_handler;       //обработчик сигнала обновлени€ свойств
  
/// онструктор
  Impl ()
    : need_update_properties (true) 
    , need_bind_properties (true)
  {
  }
};

/*
     онструктор / деструктор
*/

Technique::Technique ()
  : impl (new Impl)
{
  impl->properties_update_handler = xtl::bind (&Technique::UpdateProperties, this);
}

Technique::~Technique ()
{
}

/*
    »м€ техники
*/

void Technique::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Technique::SetName", "name");
    
  impl->name = name;
}

const char* Technique::Name () const
{
  return impl->name.c_str ();
}

/*
     амера
*/

/*
    —войства по умолчанию
*/

void Technique::SetDefaultProperties (const common::PropertyMap& properties)
{
  impl->default_properties     = properties.Clone ();
  impl->need_update_properties = true;

  impl->default_properties_synchronizer.reset ();
}

void Technique::SetDefaultProperties (const common::ParseNode& node)
{
  try
  {
    //TODO: cache for node -> property map

    SetDefaultProperties (to_properties (node));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Technique::SetDefaultProperties(const common::ParseNode&)");
    throw;
  }
}

const common::PropertyMap& Technique::DefaultProperties ()
{
  return impl->default_properties;
}

/*
    —войства рендеринга
*/

void Technique::SetProperties (const common::PropertyMap& properties)
{
  try
  {
    impl->properties                   = properties;
    impl->need_update_properties       = true;
    impl->properties_update_connection = properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, impl->properties_update_handler);

    impl->properties_synchronizer.reset ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Technique::UpdateProperties");
    throw;
  }
}

void Technique::UpdateProperties ()
{
  impl->need_update_properties = true;
}

/*
    —в€зывание свойств техники с методами техники
*/

void Technique::BindProperties (common::PropertyBindingMap&)
{
}

/*
    ќтрисовка
*/

void Technique::UpdateFrame (RenderingContext& context, TechniquePrivateData& private_data)
{
  try
  {
      //обновление свойств

    if (impl->need_update_properties)
    {
        //биндинг свойств

      if (impl->need_bind_properties)
      {        
        BindProperties (impl->property_bindings);

        impl->binded_properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->binded_properties));
        
        impl->binded_properties_synchronizer->CopyToPropertyMap ();

        impl->need_bind_properties = false;
      }

        //обновление свойств по умолчанию

      if (!impl->default_properties_synchronizer)
      {
        impl->default_properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->default_properties));

        impl->default_properties_synchronizer->CopyFromPropertyMap (); //updates only once
      }

        //обновление свойств

      if (!impl->properties_synchronizer)
      {
          //применение свойств по умолчанию

        impl->binded_properties_synchronizer->CopyFromPropertyMap ();
        impl->default_properties_synchronizer->CopyFromPropertyMap ();

          //созданеи синхронизатора

        impl->properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->properties));
      }

      impl->properties_synchronizer->CopyFromPropertyMap ();

        //оповещение об обновлении

      UpdatePropertiesCore ();

      impl->need_update_properties = false;
    }

      //отрисовка кадра

    UpdateFrameCore (context, private_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Technique::UpdateFrame");
    throw;
  }
}

/*
    ќтрисовка массива отображаемых моделей
*/

void Technique::Draw (RenderingContext& context, size_t visual_models_count, VisualModel* const* model_ptr) const
{
  for (;visual_models_count--; model_ptr++)
  {
    VisualModel& model = **model_ptr;

    try
    {
      model.Draw (context);
    }
    catch (std::exception& e)
    {
      context.RenderManager ().Log ().Printf ("%s\n    at render::scene::server::Technique::Draw(technique='%s')", e.what (), Name ());
    }
    catch (...)
    {
      context.RenderManager ().Log ().Printf ("unknown exception\n    at render::scene::server::Technique::Draw(technique='%s')", Name ());
    }
  }
}

void Technique::Draw (RenderingContext& context, const stl::vector<VisualModel*>& models) const
{
  if (models.empty ())
    return;

  Draw (context, models.size (), &models [0]);
}

/*
    TechniquePrivateData
*/

void TechniquePrivateData::RaiseSizeError (const std::type_info& type, size_t size)
{
  throw xtl::format_operation_exception ("render::scene::server::Technique::RaiseSizeError", "Can't convert TechniquePrivateData to %s because its size %u is larger than max buffer size %u",
    type.name (), size, DATA_BUFFER_SIZE);
}
