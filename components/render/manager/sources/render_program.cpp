#include "shared.h"

using namespace render;
using namespace render::manager;

//TODO: optimize program creation with different options order (different order same options)
//TODO: minimize derived programs count according to different options order

namespace
{

/*
    Общие данные программы
*/

struct OptionsCacheCombinationKey                                                                      
{                                                                                                      
  size_t hash1, hash2;                                                                                 
                                                                                                       
  OptionsCacheCombinationKey (size_t in_hash1, size_t in_hash2) : hash1 (in_hash1), hash2 (in_hash2) {}
                                                                                                       
  bool operator == (const OptionsCacheCombinationKey& key) const                                       
  {                                                                                                    
    return hash1 == key.hash1 && hash2 == key.hash2;                                                   
  }                                                                                                    
};

inline size_t hash (const OptionsCacheCombinationKey& key)
{                                                  
  return key.hash1 * key.hash2;                    
}                       

struct RemovePred
{
  bool operator () (const ProgramPtr& program) const { return program->use_count () == 1; }
};                           
                                                                                                     
typedef stl::vector<media::rfx::Shader>                               ShaderArray;
typedef stl::vector<TexmapDesc>                                       TexmapDescArray;
typedef CacheMap<OptionsCacheCombinationKey, ProgramPtr, RemovePred>  ProgramMap;

struct DefaultSamplerHolder: public xtl::reference_counter, public CacheSource
{
  SamplerProxy            sampler;        //сэмплер текстуры
  unsigned int            channel;        //номер канала
  LowLevelSamplerStatePtr cached_sampler; //закэшированный сэмплер

/// Конструктор
  DefaultSamplerHolder (CacheHolder& owner, const TextureManagerPtr& texture_manager, const char* name, unsigned int in_channel)
    : sampler (texture_manager->GetSamplerProxy (name))
    , channel (in_channel)
  {
    owner.AttachCacheSource   (*this);
    sampler.AttachCacheHolder (*this);    
  }

  using CacheSource::UpdateCache;
  using CacheSource::ResetCache;

/// Работа с кэшем
  void ResetCacheCore ()
  {
    cached_sampler = LowLevelSamplerStatePtr ();
  }

  void UpdateCacheCore ()
  {
    cached_sampler = sampler.Resource ();

    InvalidateCache ();
  }
};

typedef xtl::intrusive_ptr<DefaultSamplerHolder> DefaultSamplerHolderPtr;
typedef stl::vector<DefaultSamplerHolderPtr>     DefaultSamplerHolderArray;

struct ProgramCommonData: public xtl::reference_counter, public CacheSource, public DebugIdHolder
{
  DeviceManagerPtr            device_manager;               //менеджер устройства отрисовки
  TextureManagerPtr           texture_manager;              //менеджер текстур
  stl::string                 name;                         //имя программы
  ShaderArray                 shaders;                      //шейдеры
  TexmapDescArray             texmaps;                      //текстурные карты
  DefaultSamplerHolderArray   default_samplers;             //сэмплеры по умолчанию
  stl::string                 static_options;               //статические опции компиляции шейдеров
  stl::string                 dynamic_options;              //имена динамических опций
  ShaderOptionsLayout         dynamic_options_layout;       //расположение динамических опций
  Log                         log;                          //протокол
  bool                        has_framemaps;                //программа ссылается на контекстные карты кадра
  ProgramParametersLayoutPtr  parameters_layout;            //расположение параметров программы
  PropertyBuffer              properties;                   //свойства программы
  LowLevelStateBlockPtr       cached_state_block;           //блок данных параметров
  size_t                      cached_state_block_mask_hash; //хэш закэшированной маски блока состояний программы
  ProgramMap                  derived_programs;             //производные программы
  
///Конструктор
  ProgramCommonData (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager, const char* in_name)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
    , dynamic_options_layout (&in_device_manager->CacheManager ())
    , has_framemaps (false)
    , properties (in_device_manager)
    , cached_state_block_mask_hash (0)
    , derived_programs (&device_manager->CacheManager ())
  {
    try
    {        
      name = in_name;        

      if (device_manager->Settings ().HasDebugLog ())
        log.Printf ("Program '%s' common data created (id=%u)", name.c_str (), Id ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::ProgramCommonData::ProgramCommonData");
      throw;
    }
  }
  
///Деструктор
  ~ProgramCommonData ()
  {
    cached_state_block = LowLevelStateBlockPtr ();

    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Program '%s' common data destroyed (id=%u)", name.c_str (), Id ());    
  }
  
  using CacheSource::InvalidateCache;
  using CacheSource::ResetCache;
  using CacheSource::UpdateCache;

///Работа с кэшем
  void ResetCacheCore ()
  {
    cached_state_block_mask_hash = 0;
    cached_state_block           = LowLevelStateBlockPtr ();

    for (DefaultSamplerHolderArray::iterator iter=default_samplers.begin (), end=default_samplers.end (); iter!=end; ++iter)
      (*iter)->ResetCache ();
  }

  void UpdateCacheCore ()
  {
    try
    {
      bool has_debug_log = device_manager->Settings ().HasDebugLog ();
      
      if (has_debug_log)
        log.Printf ("Update program '%s' common data cache (id=%u)", name.c_str (), Id ());

      has_framemaps = false;
        
      for (TexmapDescArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
        if (iter->is_framemap)
        {
          has_framemaps = true;
          break;
        }
        
      common::PropertyMap new_properties;
      
      for (TexmapDescArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
      {
        TexmapDesc& desc = *iter;        
        
        new_properties.SetProperty (desc.param_name.c_str (), (int)desc.channel);
      }

      render::low_level::StateBlockMask mask;

      low_level::IDeviceContext& context = device_manager->ImmediateContext ();

      for (DefaultSamplerHolderArray::iterator iter=default_samplers.begin (), end=default_samplers.end (); iter!=end; ++iter)
      {
        DefaultSamplerHolder& sampler_holder = **iter;

        sampler_holder.UpdateCache ();

        mask.ss_samplers [sampler_holder.channel] = true;

        context.SSSetSampler (sampler_holder.channel, sampler_holder.cached_sampler.get ());
      }

      mask.ss_constant_buffers [ProgramParametersSlot_Program] = true;

      size_t state_block_mask_hash = mask.Hash ();

      if (!cached_state_block || cached_state_block_mask_hash != state_block_mask_hash)
      {
        cached_state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);

        if (has_debug_log)
          log.Printf ("...create state block for program common data");
      }

      ProgramParametersLayoutPtr new_layout = device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_Program, new_properties.Layout ());
      
      properties.SetProperties (new_properties);
      
      context.SSSetConstantBuffer (ProgramParametersSlot_Program, properties.Buffer ().get ());
      
      cached_state_block->Capture (&context);
      
      parameters_layout = new_layout;

      InvalidateCache ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::ProgramCommonData::UpdateCacheCore");
      throw;
    }
  }
};

typedef xtl::intrusive_ptr<ProgramCommonData> ProgramCommonDataPtr;

}

/*
    Описание реализации программы
*/

struct Program::Impl: public DebugIdHolder
{
  ProgramCommonDataPtr common_data;       //общие данные программы
  ShaderOptions        options;           //опции данного экземпляра программы
  LowLevelProgramPtr   low_level_program; //низкоуровневая программа
  
///Конструктор
  Impl (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const char* name, const char* static_options, const char* dynamic_options)
  {
    try
    {
      common_data = ProgramCommonDataPtr (new ProgramCommonData (device_manager, texture_manager, name), false);      
              
      common_data->static_options  = static_options;      
      common_data->dynamic_options = dynamic_options;
      
      common::StringArray option_array = common::split (dynamic_options);
      
      ShaderOptionsLayout& layout = common_data->dynamic_options_layout;
      
      for (size_t i=0, count=option_array.Size (); i<count; i++)
        layout.Add (option_array [i]);
        
      if (common_data->device_manager->Settings ().HasDebugLog ())
        common_data->log.Printf ("Program '%s' created (id=%u)", common_data->name.c_str (), Id ());        
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::Program::Impl::Impl");
      throw;
    }
  }
  
  Impl (const Impl& impl, const ShaderOptions& in_options)
    : common_data (impl.common_data)
    , options (impl.options)
  {
    options.options      += " ";
    options.options      += in_options.options;
    options.options_hash  = common::strhash (options.options.c_str ());
    
    if (common_data->device_manager->Settings ().HasDebugLog ())
      common_data->log.Printf ("Program '%s' created for options = '%s' (id=%u)", common_data->name.c_str (), options.options.c_str (), Id ());    
  }
  
///Деструктор
  ~Impl ()
  {
    if (common_data->device_manager->Settings ().HasDebugLog ())
      common_data->log.Printf ("Program '%s' destroyed for options = '%s' (id=%u)", common_data->name.c_str (), options.options.c_str (), Id ());
  }
};

/*
    Конструкторы / деструктор
*/

Program::Program (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const char* name, const char* static_options, const char* dynamic_options)
{
  try
  {
    if (!static_options)
      throw xtl::make_null_argument_exception ("", "static_options");
      
    if (!dynamic_options)
      throw xtl::make_null_argument_exception ("", "dynamic_options");

    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");

    if (!texture_manager)
      throw xtl::make_null_argument_exception ("", "texture_manager");
      
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    impl = new Impl (device_manager, texture_manager, name, static_options, dynamic_options);

    AttachCacheSource (*impl->common_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Program::Program");
    throw;
  }
}

Program::Program (Program& parent, const ShaderOptions& options)
  : impl (new Impl (*parent.impl, options))
{
  AttachCacheSource (*impl->common_data);
}

Program::~Program ()
{
}

/*
   Имя программы
*/

const char* Program::Name ()
{
  return impl->common_data->name.c_str ();
}

/*
    Шейдеры программы
*/

void Program::Attach (const media::rfx::Shader& shader)
{
  impl->common_data->shaders.push_back (shader);
}

void Program::Detach (const media::rfx::Shader& shader)
{
  for (ShaderArray::iterator iter=impl->common_data->shaders.begin (), end=impl->common_data->shaders.end (); iter<end;)
    if (iter->Id () == shader.Id ())
    {
      ShaderArray::iterator next = iter;
      
      ++next;
      
      impl->common_data->shaders.erase (iter);
      
      iter = next;
      --end;
    }
    else ++iter;
}

void Program::DetachAllShaders ()
{
  impl->common_data->shaders.clear ();
}

unsigned int Program::ShadersCount ()
{
  return (unsigned int)impl->common_data->shaders.size ();
}

/*
    Опции данной программы
*/

const char* Program::StaticOptions ()
{
  return impl->common_data->static_options.c_str ();
}

const char* Program::DynamicOptions ()
{
  return impl->common_data->dynamic_options.c_str ();
}

/*
    Отображение семантики текстурной карты на номер канала и имя параметра
*/

unsigned int Program::TexmapsCount ()
{
  return (unsigned int)impl->common_data->texmaps.size ();
}

bool Program::HasFramemaps ()
{
  try
  { 
    UpdateCache ();

    return impl->common_data->has_framemaps;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Program::HasFramemaps");
    throw;
  }
}

const TexmapDesc* Program::Texmaps ()
{
  if (impl->common_data->texmaps.empty ())
    return 0;
    
  return &impl->common_data->texmaps [0];
}

const TexmapDesc& Program::Texmap (unsigned int index)
{
  if (index >= impl->common_data->texmaps.size ())
    throw xtl::make_range_exception ("render::manager::Program::Texmap", "index", index, impl->common_data->texmaps.size ());
    
  return impl->common_data->texmaps [index];
}

const TexmapDesc* Program::FindTexmapBySemantic (size_t semantic_hash)
{
  for (TexmapDescArray::iterator iter = impl->common_data->texmaps.begin (), end = impl->common_data->texmaps.end (); iter != end; ++iter)
  {
    if (semantic_hash == iter->semantic_hash)
      return &*iter;
  }

  return 0;
}

const TexmapDesc* Program::FindTexmapBySemantic (const char* semantic)
{
  if (!semantic)
    return 0;

  return FindTexmapBySemantic (common::strhash (semantic));
}

void Program::SetTexmap (unsigned int index, unsigned int channel, const char* semantic, const char* param_name, const char* default_sampler, bool is_framemap)
{
  static const char* METHOD_NAME = "render::manager::Program::SetTexmap";

  if (!semantic)
    throw xtl::make_null_argument_exception (METHOD_NAME, "semantic");
    
  if (!param_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "param_name");

  if (!default_sampler)
    throw xtl::make_null_argument_exception (METHOD_NAME, "default_sampler");

  if (index >= impl->common_data->texmaps.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->common_data->texmaps.size ());

  if (channel >= render::low_level::DEVICE_SAMPLER_SLOTS_COUNT)
    throw xtl::make_range_exception (METHOD_NAME, "channel", channel, render::low_level::DEVICE_SAMPLER_SLOTS_COUNT);

  TexmapDesc& desc = impl->common_data->texmaps [index];

  desc.channel       = channel;
  desc.semantic      = semantic;
  desc.param_name    = param_name;
  desc.is_framemap   = is_framemap;
  desc.semantic_hash = common::strhash (semantic);

  if (default_sampler != desc.default_sampler)
  {
      //обновление дефолтного сэмплера

    DefaultSamplerHolderPtr default_sampler_holder = DefaultSamplerHolderPtr (new DefaultSamplerHolder (*impl->common_data, impl->common_data->texture_manager, default_sampler, channel), false);

    bool found = false;

    for (DefaultSamplerHolderArray::iterator iter=impl->common_data->default_samplers.begin (), end=impl->common_data->default_samplers.end (); iter!=end; ++iter)
      if ((*iter)->channel == channel)
      {
        found = true;
        *iter = default_sampler_holder;

        break;
      }

    if (!found)
      impl->common_data->default_samplers.push_back (default_sampler_holder);

    desc.default_sampler = default_sampler;
  }

  impl->common_data->InvalidateCache ();
}

unsigned int Program::AddTexmap (unsigned int channel, const char* semantic, const char* param_name, const char* default_sampler, bool is_framemap)
{
  impl->common_data->texmaps.push_back ();

  unsigned int index = (unsigned int)impl->common_data->texmaps.size () - 1;
  
  try
  {
    SetTexmap (index, channel, semantic, param_name, default_sampler, is_framemap);

    return index;
  }
  catch (...)
  {
    impl->common_data->texmaps.pop_back ();
    throw;
  }
}

void Program::RemoveTexmap (unsigned int index)
{
  if (index >= impl->common_data->texmaps.size ())
    return;

  unsigned int channel = impl->common_data->texmaps [index].channel;

  for (DefaultSamplerHolderArray::iterator iter=impl->common_data->default_samplers.begin (), end=impl->common_data->default_samplers.end (); iter!=end; ++iter)
    if (channel == (*iter)->channel)
    {
      impl->common_data->default_samplers.erase (iter);
      break;
    }

  impl->common_data->texmaps.erase (impl->common_data->texmaps.begin () + index);
  
  impl->common_data->InvalidateCache ();
}

void Program::RemoveAllTexmaps ()
{
  impl->common_data->texmaps.clear ();
  impl->common_data->default_samplers.clear ();
  
  impl->common_data->InvalidateCache ();
}

/*
    Получение производной программы
*/

Program& Program::DerivedProgram (const ShaderOptions& options)
{
  try
  {
    if (options.options.empty ())
      return *this;

    OptionsCacheCombinationKey key (impl->options.options_hash, options.options_hash);

    ProgramMap& derived_programs = impl->common_data->derived_programs;
      
    if (ProgramPtr* program = derived_programs.Find (key))
      return **program;
      
    ProgramPtr program (new Program (*this, options), false);
    
    derived_programs.Add (key, program);
    
    return *program;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Program::DerivedProgram(const ShaderOptions&)");
    throw;
  }
}

Program& Program::DerivedProgram (ShaderOptionsCache& cache)
{
  try
  {
    const ShaderOptions& options = cache.GetShaderOptions (impl->common_data->dynamic_options_layout);
    
    return DerivedProgram (options);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Program::DerivedProgram(const ShaderOptionsCache&)");
    throw;
  }
}

/*
    Получение низкоуровневой программы шейдинга
*/

namespace
{

struct ShaderCompilerLog
{
  ShaderCompilerLog (ProgramCommonData& in_common_data)
    : common_data (in_common_data)
  {
  }
  
  void operator () (const char* message)
  {
      //отсечение стандартных сообщений об отсутствии ошибок
      
    static const char* IGNORE_MESSAGES [] = {
      "*: No errors.",
      "Fragment shader(s) linked, vertex shader(s) linked*",
      "*shader was successfully compiled to run on hardware.",
      "Fragment shader(s) linked, vertex shader(s) linked."
    };
    
    static const unsigned int IGNORE_MESSAGES_COUNT = sizeof (IGNORE_MESSAGES) / sizeof (*IGNORE_MESSAGES);
    
    for (unsigned int i=0; i<IGNORE_MESSAGES_COUNT; i++)
      if (common::wcimatch (message, IGNORE_MESSAGES [i]))
        return;
      
      //вывод сообщения
    
    static const char* LINKER_PREFIX        = "linker: ";
    static size_t      LINKER_PREFIX_LENGTH = strlen (LINKER_PREFIX);
    
    if (common::wcimatch (message, "linker:*"))
    {
      common_data.log.Printf ("%s: %s", common_data.name.c_str (), message + LINKER_PREFIX_LENGTH);
      return;
    }
    
    common_data.log.Printf ("%s", message);
  }
  
  ProgramCommonData& common_data;
};    

}

const LowLevelProgramPtr& Program::LowLevelProgram ()
{
  try
  {
    if (impl->low_level_program)
      return impl->low_level_program;
      
    render::low_level::IDevice& device = impl->common_data->device_manager->Device ();
      
    stl::string options = impl->common_data->static_options + " " + impl->options.options;
      
    static const unsigned int SHADERS_RESERVE_COUNT = 4;

    stl::vector<render::low_level::ShaderDesc> shaders;
    
    shaders.reserve (SHADERS_RESERVE_COUNT);        

    for (ShaderArray::iterator iter=impl->common_data->shaders.begin (), end=impl->common_data->shaders.end (); iter!=end; ++iter)
    {
      media::rfx::Shader& shader = *iter;      
      
      render::low_level::ShaderDesc desc;
      
      memset (&desc, 0, sizeof (desc));      
      
      desc.name             = shader.Name ();  
      desc.profile          = shader.Profile ();
      desc.source_code_size = (unsigned int)shader.SourceCodeSize ();
      desc.source_code      = shader.SourceCode ();
      desc.options          = options.c_str ();      
      
      shaders.push_back (desc);
    }

    if (shaders.empty ())
      throw xtl::format_operation_exception ("", "No shaders found for shading");
      
    impl->common_data->log.Printf ("Build program '%s'...", impl->common_data->name.c_str ());

    LowLevelProgramPtr program (device.CreateProgram ((unsigned int)shaders.size (), &shaders [0], ShaderCompilerLog (*impl->common_data)), false);
    
    impl->common_data->log.Printf ("...program '%s' successfully built", impl->common_data->name.c_str ());
      
    impl->low_level_program = program;
  
    return impl->low_level_program;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Program::LowLevelProgram");
    throw;
  }
}

/*
    Блок состояний материала
*/

LowLevelStateBlockPtr Program::StateBlock ()
{
  try
  { 
    UpdateCache ();
  
    return impl->common_data->cached_state_block;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Program::StateBlock");
    throw;
  }
}

/*
    Получение объекта расположения параметров программы шейдинга
*/

ProgramParametersLayoutPtr Program::ParametersLayout ()
{
  try
  { 
    UpdateCache ();

    return impl->common_data->parameters_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Program::ParametersLayout");
    throw;
  }
}

/*
    Управление кэшированием
*/

void Program::UpdateCacheCore ()
{
  impl->common_data->UpdateCache ();

    //обновление зависимых кэшей (реакция на Impl::InvalidateCacheDependencies)

  InvalidateCacheDependencies ();
}

void Program::ResetCacheCore ()
{
  impl->common_data->ResetCache ();
}
