#include "shared.h"

using namespace render;
using namespace render::scene::server;

namespace
{

struct FrameMapDesc
{
  manager::Texture color_texture;       //color texture
  manager::Texture depth_texture;       //depth texture
  stl::string      color_render_target; //color render target name
  stl::string      depth_render_target; //depth render target name
  stl::string      framemap;            //frame map name

  FrameMapDesc (const manager::Texture& in_color_texture, const manager::Texture& in_depth_texture, const char* in_color_render_target,
                const char* in_depth_render_target, const char* in_framemap)
    : color_texture (in_color_texture)
    , depth_texture (in_depth_texture)
    , color_render_target (in_color_render_target)
    , depth_render_target (in_depth_render_target)
    , framemap (in_framemap)
    {}
};

typedef stl::vector<FrameMapDesc> FrameMapArray;

}

/*
    Rendering technique implementation
*/

struct Technique::Impl
{
  stl::string                                           name;                            //technique name
  common::PropertyMap                                   properties;                      //properties
  common::PropertyMap                                   default_properties;              //default properties
  common::PropertyMap                                   binded_properties;               //properties values after binding
  common::PropertyBindingMap                            property_bindings;               //binding technique methods with properties
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> binded_properties_synchronizer;  //binded properties synchronizer
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> default_properties_synchronizer; //default properties synchronizer
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> properties_synchronizer;         //properties synchronizer
  bool                                                  need_update_properties;          //is properties update required flag
  bool                                                  need_bind_properties;            //is properties binding required flag
  bool                                                  need_bind_framemaps;             //is framemaps binding required flag
  xtl::auto_connection                                  properties_update_connection;    //properties update connection
  common::PropertyMap::EventHandler                     properties_update_handler;       //properties update handler
  FrameMapArray                                         framemaps;                       //framemaps array
  
///Constructor
  Impl (RenderManager& manager, const common::ParseNode& node)
    : need_update_properties (true)
    , need_bind_properties (true)
    , need_bind_framemaps (true)
  {
    size_t framemaps_count = 0;

    for (common::ParseNode framemap_node = node.First ("framemap"); framemap_node; framemap_node = framemap_node.NextNamesake (), framemaps_count++);

    framemaps.reserve (framemaps_count);

    for (common::ParseNode framemap_node = node.First ("framemap"); framemap_node; framemap_node = framemap_node.NextNamesake ())
    {
      const char* color_texture_name = common::get<const char*> (framemap_node, "color_texture");
      const char* depth_texture_name = common::get<const char*> (framemap_node, "depth_texture");

      bool color_texture_shared = manager.Manager ().HasSharedTexture (color_texture_name);
      bool depth_texture_shared = manager.Manager ().HasSharedTexture (depth_texture_name);

      manager::Texture color_texture = color_texture_shared ? manager.Manager ().CreateSharedTexture (color_texture_name) : manager.Manager ().CreateTexture (color_texture_name);
      manager::Texture depth_texture = depth_texture_shared ? manager.Manager ().CreateSharedTexture (depth_texture_name) : manager.Manager ().CreateTexture (depth_texture_name);

      framemaps.push_back (FrameMapDesc (color_texture,
                                         depth_texture,
                                         common::get<const char*> (framemap_node, "color_render_target"),
                                         common::get<const char*> (framemap_node, "depth_render_target", ""),
                                         common::get<const char*> (framemap_node, "framemap")));

      if (!color_texture_shared)
        manager.Manager ().ShareTexture (color_texture_name, color_texture);

      if (!depth_texture_shared)
        manager.Manager ().ShareTexture (depth_texture_name, depth_texture);
    }
  }
};

/*
    Constructor / destructor
*/

Technique::Technique (RenderManager& manager, const common::ParseNode& node)
  : impl (new Impl (manager, node))
{
  impl->properties_update_handler = xtl::bind (&Technique::UpdateProperties, this);
}

Technique::~Technique ()
{
}

/*
    Technique name
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
    Default properties
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
    Rendering properties
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
    Binding technique properties with methods
*/

void Technique::BindProperties (common::PropertyBindingMap&)
{
}

/*
    Rendering
*/

void Technique::UpdateFrame (RenderingContext& context, TechniquePrivateData& private_data)
{
  try
  {
      //properties update

    if (impl->need_update_properties)
    {
        //properties binding

      if (impl->need_bind_properties)
      {        
        BindProperties (impl->property_bindings);

        impl->binded_properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->binded_properties));
        
        impl->binded_properties_synchronizer->CopyToPropertyMap ();

        impl->need_bind_properties = false;
      }

        //default properties binding

      if (!impl->default_properties_synchronizer)
      {
        impl->default_properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->default_properties));

        impl->default_properties_synchronizer->CopyFromPropertyMap (); //updates only once
      }

        //properties update

      if (!impl->properties_synchronizer)
      {
          //apply default properties

        impl->binded_properties_synchronizer->CopyFromPropertyMap ();
        impl->default_properties_synchronizer->CopyFromPropertyMap ();

          //create synchronizer

        impl->properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->properties));
      }

      impl->properties_synchronizer->CopyFromPropertyMap ();

        //update notification

      UpdatePropertiesCore ();

      impl->need_update_properties = false;
    }

    if (impl->need_bind_framemaps)
    {
      manager::Frame& frame = context.Frame ();

      for (FrameMapArray::const_iterator iter = impl->framemaps.begin (), end = impl->framemaps.end (); iter != end; ++iter)
      {
        frame.SetRenderTarget (iter->color_render_target.c_str (), iter->color_texture.RenderTarget (0, 0));
        frame.SetLocalTexture (iter->framemap.c_str (), iter->color_texture);

        if (!iter->depth_render_target.empty ())
          frame.SetRenderTarget (iter->depth_render_target.c_str (), iter->depth_texture.RenderTarget (0, 0));
      }

      impl->need_bind_framemaps = false;
    }

      //render frame

    UpdateFrameCore (context, private_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Technique::UpdateFrame");
    throw;
  }
}

/*
    TechniquePrivateData
*/

void TechniquePrivateData::RaiseSizeError (const std::type_info& type, size_t size)
{
  throw xtl::format_operation_exception ("render::scene::server::Technique::RaiseSizeError", "Can't convert TechniquePrivateData to %s because its size %u is larger than max buffer size %u",
    type.name (), size, DATA_BUFFER_SIZE);
}
