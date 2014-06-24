#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    Описание реализации менеджера материалов
*/

typedef stl::hash_map<stl::string, manager::Texture>  TextureMap;
typedef stl::hash_map<stl::string, manager::Material> MaterialMap;

struct MaterialManager::Impl
{
  RenderManager render_manager; //менеджер рендеринга
  TextureMap    textures;       //карта текстур
  MaterialMap   materials;      //карта материалов

  Impl (const RenderManager& in_render_manager)
    : render_manager (in_render_manager)
  {
  }

  ~Impl ()
  {
    manager::RenderManager manager = render_manager.Manager ();

    for (MaterialMap::iterator iter=materials.begin (), end=materials.end (); iter!=end; ++iter)
      manager.UnshareMaterial (iter->first.c_str ());

    for (TextureMap::iterator iter=textures.begin (), end=textures.end (); iter!=end; ++iter)
      manager.UnshareTexture (iter->first.c_str ());
  }
};

/*
    Конструктор / деструктор
*/

MaterialManager::MaterialManager (const RenderManager& render_manager)
  : impl (new Impl (render_manager))
{
}

MaterialManager::~MaterialManager ()
{
}

/*
    Управление текстурами
*/

void MaterialManager::UpdateTexture (const char* texture_name, const media::Image& image)
{
  try
  {
    if (!texture_name)
      throw xtl::make_null_argument_exception ("", "texture_name");

    TextureMap::iterator iter = impl->textures.find (texture_name);

    if (iter != impl->textures.end ())
    {
      iter->second.Update (image);

      return;
    }

    manager::Texture texture = impl->render_manager.Manager ().CreateTexture (image, manager::TextureDimension_2D, true); //TODO: pass params from scene render client

    impl->textures.insert_pair (texture_name, texture);

    impl->render_manager.Manager ().ShareTexture (texture_name, texture);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::MaterialManager::UpdateTexture");
    throw;
  }
}

void MaterialManager::RemoveTexture (const char* texture_name)
{
  if (!texture_name)
    return;

  TextureMap::iterator iter = impl->textures.find (texture_name);

  if (iter == impl->textures.end ())
    return;

  impl->textures.erase (iter);

  impl->render_manager.Manager ().UnshareTexture (texture_name);
}

/*
    Управление материалами
*/

void MaterialManager::CloneMaterial (const char* material_name, const char* prototype_name)
{
  try
  {
    if (!material_name)
      throw xtl::make_null_argument_exception ("", "material_name");

    if (!prototype_name)
      throw xtl::make_null_argument_exception ("", "prototype_name");

    if (impl->materials.find (material_name) != impl->materials.end ())
      throw xtl::make_argument_exception ("", "material_name", material_name, "Material has been already cloned");

    manager::Material material = impl->render_manager.Manager ().CreateSharedMaterial (prototype_name);

    impl->materials.insert_pair (material_name, material);

    impl->render_manager.Manager ().ShareMaterial (material_name, material);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::MaterialManager::CloneMaterial");
    throw;
  }
}

void MaterialManager::SetMaterialTexmapImage (const char* material_name, const char* semantic, const char* image_name)
{
  try
  {
    if (!material_name)
      throw xtl::make_null_argument_exception ("", "material_name");

    if (!semantic)
      throw xtl::make_null_argument_exception ("", "semantic");

    if (!image_name)
      throw xtl::make_null_argument_exception ("", "image_name");
    
    MaterialMap::iterator iter = impl->materials.find (material_name);

    if (iter == impl->materials.end ())
      throw xtl::make_argument_exception ("", "material_name", material_name, "Material has not been defined");

    iter->second.SetTexmapImage (semantic, image_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::MaterialManager::SetMaterialTexmapImage");
    throw;
  }
}

void MaterialManager::SetMaterialTexmapSampler (const char* material_name, const char* semantic, const char* sampler)
{
  try
  {
    if (!material_name)
      throw xtl::make_null_argument_exception ("", "material_name");

    if (!semantic)
      throw xtl::make_null_argument_exception ("", "semantic");

    if (!sampler)
      throw xtl::make_null_argument_exception ("", "sampler");
    
    MaterialMap::iterator iter = impl->materials.find (material_name);

    if (iter == impl->materials.end ())
      throw xtl::make_argument_exception ("", "material_name", material_name, "Material has not been defined");

    iter->second.SetTexmapSampler (semantic, sampler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::MaterialManager::SetMaterialTexmapSampler");
    throw;
  }
}

void MaterialManager::RemoveMaterial (const char* material_name)
{
  if (!material_name)
    return;

  MaterialMap::iterator iter = impl->materials.find (material_name);

  if (iter == impl->materials.end ())
    return;

  impl->materials.erase (iter);

  impl->render_manager.Manager ().UnshareMaterial (material_name);
}
