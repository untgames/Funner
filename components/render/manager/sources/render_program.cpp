#include "shared.h"

//TODO: cache flush, common cache for all render objects

using namespace render;

namespace
{

/*
    Общие данные программы
*/

typedef stl::vector<media::rfx::Shader> ShaderArray;
typedef stl::vector<TexmapDesc>         TexmapDescArray;

struct ProgramCommonData: public xtl::reference_counter
{
  stl::string         name;                   //имя программы
  ShaderArray         shaders;                //шейдеры
  TexmapDescArray     texmaps;                //текстурные карты  
  stl::string         static_options;         //статические опции компиляции шейдеров
  stl::string         dynamic_options;        //имена динамических опций
  ShaderOptionsLayout dynamic_options_layout; //расположение динамических опций
  Log                 log;                    //протокол
};

typedef xtl::intrusive_ptr<ProgramCommonData> ProgramCommonDataPtr;

}

/*
    Описание реализации программы
*/

typedef stl::hash_map<size_t, ProgramPtr> ProgramMap;

struct Program::Impl
{
  ProgramCommonDataPtr common_data;           //общие данные программы
  ShaderOptions        options;               //опции данного экземпляра программы
  ProgramMap           derived_programs;      //производные программы  
  LowLevelProgramPtr   low_level_program;     //низкоуровневая программа
  
///Конструктор
  Impl (const char* name, const char* static_options, const char* dynamic_options)
    : common_data (new ProgramCommonData, false)
  {
    try
    {
      if (!name)
        throw xtl::make_null_argument_exception ("", "name");
      
      if (!static_options)
        throw xtl::make_null_argument_exception ("", "static_options");
        
      if (!dynamic_options)
        throw xtl::make_null_argument_exception ("", "dynamic_options");
        
      common_data->name            = name;
      common_data->static_options  = static_options;      
      common_data->dynamic_options = dynamic_options;
      
      common::StringArray option_array = common::split (dynamic_options);
      
      ShaderOptionsLayout& layout = common_data->dynamic_options_layout;
      
      for (size_t i=0, count=option_array.Size (); i<count; i++)
        layout.Add (option_array [i]);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::Program::Impl::Impl");
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
  }
};

/*
    Конструкторы / деструктор
*/

Program::Program (const char* name, const char* static_options, const char* dynamic_options)
  : impl (new Impl (name, static_options, dynamic_options))
{
}

Program::Program (Program& parent, const ShaderOptions& options)
  : impl (new Impl (*parent.impl, options))
{
}

Program::~Program ()
{
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

size_t Program::TexmapsCount ()
{
  return impl->common_data->texmaps.size ();
}

const TexmapDesc* Program::Texmaps ()
{
  if (impl->common_data->texmaps.empty ())
    return 0;
    
  return &impl->common_data->texmaps [0];
}

const TexmapDesc& Program::Texmap (size_t index)
{
  if (index >= impl->common_data->texmaps.size ())
    throw xtl::make_range_exception ("render::Program::Texmap", "index", index, impl->common_data->texmaps.size ());
    
  return impl->common_data->texmaps [index];
}

void Program::SetTexmap (size_t index, size_t channel, const char* semantic, const char* param_name)
{
  static const char* METHOD_NAME = "render::Program::SetTexmap";

  if (!semantic)
    throw xtl::make_null_argument_exception (METHOD_NAME, "semantic");
    
  if (!param_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "param_name");

  if (index >= impl->common_data->texmaps.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->common_data->texmaps.size ());
    
  TexmapDesc& desc = impl->common_data->texmaps [index];
  
  desc.channel    = channel;
  desc.semantic   = semantic;
  desc.param_name = param_name;
}

void Program::RemoveTexmap (size_t index)
{
  if (index >= impl->common_data->texmaps.size ())
    return;

  impl->common_data->texmaps.erase (impl->common_data->texmaps.begin () + index);
}

void Program::RemoveAllTexmaps ()
{
  impl->common_data->texmaps.clear ();
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
      
    ProgramMap::iterator iter = impl->derived_programs.find (options.options_hash);
    
    if (iter != impl->derived_programs.end ())
      return *iter->second;
      
    ProgramPtr program (new Program (*this, options), false);
    
    impl->derived_programs.insert_pair (options.options_hash, program);
    
    return *program;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::DerivedProgram(const ShaderOptions&)");
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
    e.touch ("render::Program::DerivedProgram(const ShaderOptionsCache&)");
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
      "*shader was successfully compiled to run on hardware."
    };
    
    static const size_t IGNORE_MESSAGES_COUNT = sizeof (IGNORE_MESSAGES) / sizeof (*IGNORE_MESSAGES);
    
    for (size_t i=0; i<IGNORE_MESSAGES_COUNT; i++)
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

const LowLevelProgramPtr& Program::LowLevelProgram (render::low_level::IDevice& device)
{
  try
  {
    if (impl->low_level_program)
      return impl->low_level_program;
      
    stl::string options = impl->common_data->static_options + " " + impl->options.options;
      
    static const size_t SHADERS_RESERVE_COUNT = 4;

    stl::vector<render::low_level::ShaderDesc> shaders;
    
    shaders.reserve (SHADERS_RESERVE_COUNT);        

    for (ShaderArray::iterator iter=impl->common_data->shaders.begin (), end=impl->common_data->shaders.end (); iter!=end; ++iter)
    {
      media::rfx::Shader& shader = *iter;      
      
      render::low_level::ShaderDesc desc;
      
      memset (&desc, 0, sizeof (desc));      
      
      desc.name             = shader.Name ();  
      desc.source_code_size = shader.SourceCodeSize ();
      desc.source_code      = shader.SourceCode ();
      desc.options          = options.c_str ();
      
      shaders.push_back (desc);
    }

    if (shaders.empty ())
      throw xtl::format_operation_exception ("", "No shaders found for shading");
      
    impl->common_data->log.Printf ("Build program '%s'...", impl->common_data->name.c_str ());

    LowLevelProgramPtr program (device.CreateProgram (shaders.size (), &shaders [0], ShaderCompilerLog (*impl->common_data)), false);
      
    impl->low_level_program = program;
  
    return impl->low_level_program;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::LowLevelProgram");
    throw;
  }
}
