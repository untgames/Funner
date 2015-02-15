#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

namespace
{

/*
    ���������
*/

const char* DEFAULT_PROGRAM_NAME = ""; //��� ��������� �� ���������

/*
    ���������� �����
*/

struct Texmap: public xtl::reference_counter, public CacheHolder
{
  TextureProxy            texture;               //������ ��������
  SamplerProxy            sampler;               //������� ��������
  bool                    is_dynamic;            //�������� �� �������� ������������
  size_t                  semantic_hash;         //��� ����� ���������
  TexturePtr              cached_texture;        //�������������� ��������
  LowLevelSamplerStatePtr cached_sampler;        //�������������� �������
  LowLevelTexturePtr      cached_device_texture; //�������������� ��������
  
  Texmap (CacheHolder& owner, const TextureProxy& in_texture_proxy, const SamplerProxy& in_sampler_proxy, bool in_is_dynamic, const char* semantic)
    : texture (in_texture_proxy)
    , sampler (in_sampler_proxy)
    , is_dynamic (in_is_dynamic)
    , semantic_hash (semantic ? common::strhash (semantic) : 0)
  {
    owner.AttachCacheSource (*this);
    
    texture.AttachCacheHolder (*this);
    sampler.AttachCacheHolder (*this);
  }

  Texmap (CacheHolder& owner, const Texmap& texmap)
    : texture (texmap.texture)
    , sampler (texmap.sampler)
    , is_dynamic (texmap.is_dynamic)
    , semantic_hash (texmap.semantic_hash)
  {
    owner.AttachCacheSource (*this);
    
    texture.AttachCacheHolder (*this);
    sampler.AttachCacheHolder (*this);
  }
  
  void ResetCacheCore ()
  {
    cached_sampler        = LowLevelSamplerStatePtr ();
    cached_device_texture = LowLevelTexturePtr ();
    cached_texture        = TexturePtr ();
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      TexturePtr              new_cached_texture        = is_dynamic ? TexturePtr () : texture.Resource ();                         
      LowLevelSamplerStatePtr new_cached_sampler        = sampler.Resource ();
      LowLevelTexturePtr      new_cached_device_texture = new_cached_texture ? new_cached_texture->DeviceTexture () : LowLevelTexturePtr ();

      if (new_cached_texture == cached_texture && new_cached_sampler == cached_sampler && new_cached_device_texture == cached_device_texture)
        return;                      
      
      cached_texture        = new_cached_texture;
      cached_device_texture = new_cached_device_texture;      
      cached_sampler        = new_cached_sampler;
     
      InvalidateCache ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::Texmap::UpdateCacheCore");
      throw;
    }
  }
  
  using CacheHolder::UpdateCache;
  using CacheHolder::ResetCache;
};

}

/*
    �������� ���������� ���������
*/

typedef xtl::intrusive_ptr<Texmap> TexmapPtr;
typedef stl::vector<TexmapPtr>     TexmapArray;
typedef stl::vector<size_t>        TagHashArray;

struct MaterialImpl::Impl: public CacheHolder, public DebugIdHolder
{
  DeviceManagerPtr           device_manager;               //�������� ���������� ���������
  TextureManagerPtr          texture_manager;              //�������� �������
  ProgramManagerPtr          program_manager;              //�������� ��������
  stl::string                name;                         //��� ���������
  TagHashArray               tags;                         //���� ���������
  ProgramProxy               program;                      //������ ���������
  PropertyBuffer             properties;                   //�������� ���������
  ProgramParametersLayoutPtr material_properties_layout;   //������������ ������� ���������
  TexmapArray                texmaps;                      //���������� �����
  bool                       has_dynamic_textures;         //���� �� � ��������� ������������ ��������
  size_t                     cached_state_block_mask_hash; //��� �������������� ����� ����� ��������� ���������
  ProgramPtr                 cached_program;               //�������������� ���������
  LowLevelStateBlockPtr      cached_state_block;           //�������������� ���� ���������
  ProgramParametersLayoutPtr cached_properties_layout;     //������������ ������� ��������� � ���������
  Log                        log;                          //�������� ���������� ���������
  
///�����������
  Impl (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager, const ProgramManagerPtr& in_program_manager, const char* in_name)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
    , program_manager (in_program_manager)
    , program (program_manager->GetProgramProxy (DEFAULT_PROGRAM_NAME))
    , properties (in_device_manager)
    , has_dynamic_textures (false)
    , cached_state_block_mask_hash (0)
  {
    if (!in_name)
      throw xtl::make_null_argument_exception ("render::manager::MaterialImpl::Impl::Impl", "name");
      
    name = in_name;
    
    AttachCacheSource (properties);
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Material '%s' has been created (id=%u)", name.c_str (), Id ());
  }

///����������� �����������
  Impl (Impl& impl)
    : device_manager (impl.device_manager)
    , texture_manager (impl.texture_manager)
    , program_manager (impl.program_manager)
    , name (impl.name)
    , tags (impl.tags)
    , program (impl.program)
    , properties (device_manager)
    , material_properties_layout (impl.material_properties_layout)
    , has_dynamic_textures (impl.has_dynamic_textures)
    , cached_state_block_mask_hash (0)
  {
    try
    {
      AttachCacheSource (properties);    

      program.AttachCacheHolder (*this);

      properties.SetProperties (impl.properties.Properties ());

      texmaps.reserve (impl.texmaps.size ());

      for (TexmapArray::iterator iter=impl.texmaps.begin (), end=impl.texmaps.end (); iter!=end; ++iter)
        texmaps.push_back (TexmapPtr (new Texmap (*this, **iter), false));
      
      if (tags.empty ())
        log.Printf ("Warning: material '%s' has no tags. Will not be displayed", name.c_str ());

      if (device_manager->Settings ().HasDebugLog ())
        log.Printf ("Material '%s' copy has been created (id=%u)", name.c_str (), Id ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::MaterialImpl::Impl::Impl(const Impl&)");
      throw;
    }    
  }  
  
///����������
  ~Impl ()
  {
      //��������������� ������� ��������� ��� ����������� ����������� ������� �������� �������� �� � ����� �������� �������
      
    cached_program               = ProgramPtr ();
    cached_properties_layout     = ProgramParametersLayoutPtr ();
    cached_state_block           = LowLevelStateBlockPtr ();
    cached_state_block_mask_hash = 0;
    
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Material '%s' has been destroyed (id=%u)", name.c_str (), Id ());
  }
  
///������ � �����
  void ResetCacheCore ()
  {
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Reset material cache (id=%u)", Id ());

    if (cached_program)
      DetachCacheSource (*cached_program);

    cached_properties_layout     = ProgramParametersLayoutPtr ();
    cached_program               = ProgramPtr ();
    cached_state_block           = LowLevelStateBlockPtr ();
    cached_state_block_mask_hash = 0;
  }
  
  void UpdateCacheCore ()
  {
    try
    {
      bool need_invalidate_deps = false; //���� ������������� ���������� ��������� �� ��������� �����
      
      bool has_debug_log = device_manager->Settings ().HasDebugLog ();
      
      if (has_debug_log)
        log.Printf ("Update material '%s' cache (id=%u)", name.c_str (), Id ());
        
        //����������� ��������� ����������

      ProgramPtr old_program = cached_program;

      cached_program       = program.Resource ();
      need_invalidate_deps = need_invalidate_deps || old_program != cached_program;

      if (cached_program)
      {
        ProgramParametersLayoutPtr old_layout = cached_properties_layout;
        
        cached_properties_layout = device_manager->ProgramParametersManager ().GetParameters (&*material_properties_layout, &*cached_program->ParametersLayout (), 0);
        need_invalidate_deps     = need_invalidate_deps || old_layout != cached_properties_layout;
      }
      else throw xtl::format_operation_exception ("", "Null program '%s' for material '%s' (id=%u)", program.Name (), name.c_str (), Id ());
      
        //����������� ������������ ������ ���������
      
      LowLevelBufferPtr device_properties = properties.Buffer ();      
      
      render::low_level::IDevice&        device  = device_manager->Device ();
      render::low_level::IDeviceContext& context = device_manager->ImmediateContext ();
      
        //��������� ������������ ������ � ����� ��������� ���������� � ���������� ���������

      context.SSSetConstantBuffer (ProgramParametersSlot_Material, device_properties.get ());
      context.SSSetConstantBuffer (ProgramParametersSlot_Program, 0);
      
        //���������� �������� ��������� � ��������� ���������� ��������� (��� ����������� �� � ����������� ���������)
        
      StateBlockMask mask;        

      if (cached_program)
      {
        LowLevelStateBlockPtr program_state_block = cached_program->StateBlock ();

        if (program_state_block)
        {
            //��������� ����� ������ ��������� ����������

          program_state_block->GetMask (mask);

            //���������� �������� ��������� ����������

          program_state_block->Apply (&context);
        }
      }

        //����������� �������� ���������

      mask.ss_constant_buffers [ProgramParametersSlot_Material] = true;
      mask.ss_constant_buffers [ProgramParametersSlot_Program]  = true;

        //��������� ����������� ���������� ���� � �� ��������� � �������� ���������� ���������

      if (cached_program)
      {
        const TexmapDesc* program_texmap = cached_program->Texmaps ();

        for (unsigned int i = 0, count = cached_program->TexmapsCount (); i < count; i++, program_texmap++)
        {
          unsigned int channel = program_texmap->channel;

          if (channel >= DEVICE_SAMPLER_SLOTS_COUNT)
            continue;

          Texmap* texmap = 0;

          for (TexmapArray::iterator iter = texmaps.begin (), end = texmaps.end (); iter != end; ++iter)
          {
            if ((*iter)->semantic_hash == program_texmap->semantic_hash)
            {
              texmap = &**iter;
              break;
            }
          }

          mask.ss_textures [channel] = !texmap || !texmap->is_dynamic;
          mask.ss_samplers [channel] = true;

          bool has_default_sampler = !program_texmap->default_sampler.empty ();

          if (texmap)
          {
            texmap->UpdateCache ();

            if (!texmap->cached_device_texture)
              log.Printf ("Texmap[%u] in program '%s' for material '%s' will be ignored. Bad texture '%s'", channel, cached_program->Name (), name.c_str (), texmap->texture.Name ());

            if (!texmap->cached_sampler)
              log.Printf ("Texmap[%u] in program '%s' for material '%s' will be ignored. Bad sampler '%s'", channel, cached_program->Name (), name.c_str (), texmap->sampler.Name ());

            context.SSSetTexture (channel, texmap->cached_device_texture.get ());

            if (!has_default_sampler || texmap->cached_sampler)
              context.SSSetSampler (channel, texmap->cached_sampler.get ());
          }
          else
          {
            context.SSSetTexture (channel, 0);

            if (!has_default_sampler)
              context.SSSetSampler (channel, 0);
          }
        }
      }

        //�������� ������������� ������������ ����� ��������� ���������

      size_t state_block_mask_hash = mask.Hash ();

      if (!cached_state_block || cached_state_block_mask_hash != state_block_mask_hash)
      {
        if (has_debug_log)
          log.Printf ("...create state block for material");

        cached_state_block           = LowLevelStateBlockPtr (device.CreateStateBlock (mask), false);
        cached_state_block_mask_hash = state_block_mask_hash;
        need_invalidate_deps         = true;
      }

        //���������� ��������� ��������� ���������� ���������

      cached_state_block->Capture (&context);

        //���������� ��������� �����

      if (need_invalidate_deps)
        InvalidateCacheDependencies ();

      if (has_debug_log)
        log.Printf ("...material cache updated");
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::MaterialImpl::Impl::UpdateCacheCore");
      throw;
    }
  }

/// ����� ����� �� ���������
  TexmapPtr FindTexmapBySemantic (const char* semantic)
  {
    if (!semantic)
      return TexmapPtr ();

    size_t semantic_hash = common::strhash (semantic);

    for (TexmapArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
      if ((*iter)->semantic_hash == semantic_hash)
        return *iter;

    return TexmapPtr ();
  }
  
  using CacheHolder::UpdateCache;  
  using CacheHolder::ResetCache;
  using CacheHolder::InvalidateCache;
};

/*
    ����������� / ����������
*/

MaterialImpl::MaterialImpl (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const ProgramManagerPtr& program_manager, const char* name)
  : impl (new Impl (device_manager, texture_manager, program_manager, name))
{
  AttachCacheSource (*impl);
}

MaterialImpl::MaterialImpl (const MaterialImpl& material)
  : impl (new Impl (*material.impl))
{
  AttachCacheSource (*impl);
}

MaterialImpl::~MaterialImpl ()
{
}

/*
    ���
*/

const char* MaterialImpl::Name ()
{
  return impl->name.c_str ();
}

void MaterialImpl::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::manager::MaterialImpl::SetName", "name");
    
  if (impl->device_manager->Settings ().HasDebugLog ())
    impl->log.Printf ("Material '%s' name changed to '%s' (id=%u)", impl->name.c_str (), name, impl->Id ());
    
  impl->name = name;
}

/*
    ����
*/

size_t MaterialImpl::TagsCount ()
{
  UpdateCache ();
  
  return impl->tags.size ();
}

const size_t* MaterialImpl::Tags ()
{
  UpdateCache ();
  
  if (impl->tags.empty ())
    return 0;
    
  return &impl->tags [0];
}

/*
    ���� ��������� ���������
*/

LowLevelStateBlockPtr MaterialImpl::StateBlock ()
{
  UpdateCache ();
  
  return impl->cached_state_block;
}

/*
    ��������� ��������
*/

ProgramPtr MaterialImpl::Program ()
{
  return impl->cached_program;
}

/*
    ��������� ��������
*/

unsigned int MaterialImpl::TexturesCount ()
{
  return (unsigned int)impl->texmaps.size ();
}

TexturePtr MaterialImpl::Texture (unsigned int index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::manager::MaterialImpl::Texture", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];  
  
  texmap.UpdateCache ();
    
  return texmap.cached_texture;
}

LowLevelTexturePtr MaterialImpl::DeviceTexture (unsigned int index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::manager::MaterialImpl::DeviceTexture", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];
    
  texmap.UpdateCache ();
  
  return texmap.cached_device_texture;
}

const char* MaterialImpl::TextureName (unsigned int index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::manager::MaterialImpl::TextureName", "index", index, impl->texmaps.size ());
    
  return impl->texmaps [index]->texture.Name ();
}

LowLevelSamplerStatePtr MaterialImpl::Sampler (unsigned int index)
{
  if (index >= impl->texmaps.size ())
    throw xtl::make_range_exception ("render::manager::MaterialImpl::Sampler", "index", index, impl->texmaps.size ());
    
  Texmap& texmap = *impl->texmaps [index];    
  
  texmap.UpdateCache ();
    
  return texmap.cached_sampler;
}

/*
    ���� �� � ��������� ������������ ��������
*/

bool MaterialImpl::HasDynamicTextures ()
{
  return impl->has_dynamic_textures;
}

/*
    ���������� ���������
*/

void MaterialImpl::Update (const media::rfx::Material& material)
{
  try
  {
      //���������� ������� ���������, ��������� ������-��������� ���������
    
    common::PropertyMap new_properties = material.Properties ();
    ProgramProxy        new_program    = impl->program_manager->GetProgramProxy (material.Program ());    
    
      //�������� ���������� ����
    
    TexmapArray new_texmaps;

    new_texmaps.reserve (material.TexmapCount ());
    
    bool new_has_dynamic_textures = false;
    
    for (unsigned int i = 0, count = (unsigned int)material.TexmapCount (); i < count; i++)
    {
      const media::rfx::Texmap& texmap = material.Texmap (i);
      
        //����������� �������� �� �������� ������������ ������������ �� �������� � ����� � ������ ����� ���� ��������� ����������
      
      bool is_dynamic_image = impl->texture_manager->IsDynamicTexture (texmap.Image ());

      if (is_dynamic_image)
        new_has_dynamic_textures = true;        
      
      TexmapPtr new_texmap (new Texmap (*impl, impl->texture_manager->GetTextureProxy (texmap.Image ()),
        impl->texture_manager->GetSamplerProxy (texmap.Sampler ()), is_dynamic_image, texmap.Semantic ()), false);

      new_texmaps.push_back (new_texmap);
    }
    
      //����������� ����� ���������
    
    TagHashArray new_tag_hashes (material.TagHashes (), material.TagHashes () + material.TagsCount ());
    
    if (new_tag_hashes.empty ())
      impl->log.Printf ("Warning: material '%s' has no tags. Will not be displayed", material.Name ());
      
      //��������� ������� ������������ ������� ���������
    
    ProgramParametersLayoutPtr new_layout = impl->device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_Material, new_properties.Layout ());
    
      //����������� ����������

    if (new_program != impl->program)
    {
      new_program.AttachCacheHolder (*impl);

      impl->program.DetachCacheHolder (*impl);
      
      impl->program = new_program;      
    }

    impl->properties.SetProperties (new_properties);

    impl->texmaps.swap (new_texmaps);
    impl->tags.swap (new_tag_hashes);

    impl->material_properties_layout = new_layout;
    impl->has_dynamic_textures       = new_has_dynamic_textures;
    
      //���������� ���� � ������������� (��������� ����� ���������� ��������� ����� � ������������ �������)
      
    impl->InvalidateCache ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::MaterialImpl::Update");
    throw;
  }
}

/*
    ���������� ��������� �������
*/

void MaterialImpl::SetTexmap (const char* semantic, const char* image_name, const char* sampler)
{
  try
  {
    if (!semantic)
      throw xtl::make_null_argument_exception ("", "semantic");

    size_t semantic_hash = common::strhash (semantic);

    if (!sampler && !image_name)
    {
      for (TexmapArray::iterator iter=impl->texmaps.begin (), end=impl->texmaps.end (); iter!=end; ++iter)
        if ((*iter)->semantic_hash == semantic_hash)
        {
          impl->texmaps.erase (iter);
          break;
        }

      return;
    }

    if (!sampler)
    {
      TexmapPtr texmap = impl->FindTexmapBySemantic (semantic);

      if (!texmap)
        throw xtl::format_operation_exception ("", "Sampler not found for semantic '%s'", semantic);

      sampler = texmap->sampler.Name ();
    }

    if (!image_name)
    {
      TexmapPtr texmap = impl->FindTexmapBySemantic (semantic);

      if (!texmap)
        throw xtl::format_operation_exception ("", "Image not found for semantic '%s'", semantic);

      image_name = texmap->texture.Name ();
    }

      //������������ �����

    bool is_dynamic_image = impl->texture_manager->IsDynamicTexture (image_name);

    TexmapPtr new_texmap (new Texmap (*impl, impl->texture_manager->GetTextureProxy (image_name), impl->texture_manager->GetSamplerProxy (sampler), is_dynamic_image, semantic), false);

      //���������� ������� ����

    bool found = false;

    for (TexmapArray::iterator iter=impl->texmaps.begin (), end=impl->texmaps.end (); iter!=end; ++iter)
      if ((*iter)->semantic_hash == semantic_hash)
      {
        *iter = new_texmap;
        found = true;

        break;
      }

    if (!found)
      impl->texmaps.push_back (new_texmap);

      //���������� ���� � ������������� (��������� ����� ���������� ��������� ����� � ������������ �������)

    if (is_dynamic_image)
      impl->has_dynamic_textures = true;
      
    impl->InvalidateCache ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::MaterialImpl::SetTexmap");
    throw;
  }
}

const char* MaterialImpl::TexmapImage (const char* semantic)
{
  TexmapPtr texmap = impl->FindTexmapBySemantic (semantic);

  if (!texmap)
    return 0;

  return texmap->texture.Name ();
}

const char* MaterialImpl::TexmapSampler (const char* semantic)
{
  TexmapPtr texmap = impl->FindTexmapBySemantic (semantic);

  if (!texmap)
    return 0;

  return texmap->sampler.Name ();
}

/*
    ��������� ������� ������������ ���������� ��������� ��������
*/

ProgramParametersLayoutPtr MaterialImpl::ParametersLayout ()
{
  try
  {
    UpdateCache ();
    
    return impl->cached_properties_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::MaterialImpl::ParametersLayout");
    throw;
  }
}

/*
    ���������� ������������
*/

void MaterialImpl::UpdateCacheCore ()
{
    //���������� ��������� ����� (������� �� Impl::InvalidateCacheDependencies)

  InvalidateCacheDependencies ();
}

void MaterialImpl::ResetCacheCore ()
{
  impl->ResetCache ();
  
  for (TexmapArray::iterator iter=impl->texmaps.begin (), end=impl->texmaps.end (); iter!=end; ++iter)
    (*iter)->ResetCache ();
}
