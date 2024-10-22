#ifndef RENDER_SCENE_SERVER_MATERIAL_MANAGER_SHARED_HEADER
#define RENDER_SCENE_SERVER_MATERIAL_MANAGER_SHARED_HEADER

#include <stl/auto_ptr.h>

#include <shared/render_manager.h>

namespace render
{

namespace scene
{

namespace server
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер материалов
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialManager  (const RenderManager&);
    ~MaterialManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление текстурами
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CreateTexture (const char* texture_name, const media::Image& image, manager::TextureDimension dimension, bool create_mips);
    void UpdateTexture (const char* texture_name, const media::Image& image);
    void RemoveTexture (const char* texture_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление материалами
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CloneMaterial            (const char* material_name, const char* prototype_name);
    void SetMaterialTexmapImage   (const char* material_name, const char* semantic, const char* image_name);
    void SetMaterialTexmapSampler (const char* material_name, const char* semantic, const char* sampler);
    void RemoveMaterial           (const char* material_name);

  private:
    MaterialManager (const MaterialManager&); //no impl
    MaterialManager& operator = (const MaterialManager&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

}

#endif
