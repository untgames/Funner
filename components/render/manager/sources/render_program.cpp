#include "shared.h"

//TODO: cache flush, common cache for all render objects

using namespace render;

namespace
{

/*
    Общие данные программы
*/

typedef stl::vector<media::rfx::Shader> ShaderArray;
typedef stl::vector<TexmapDesc> TexmapDescArray;

struct ProgramCommonData: public xtl::reference_counter
{
  ShaderArray     shaders;              //шейдеры
  bool            need_shaders_rebuild; //шейдеры должны быть перекомпилированы
  TexmapDescArray texmaps;              //текстурные карты  
  
  ProgramCommonData () : need_shaders_rebuild (true) {}
};

typedef xtl::intrusive_ptr<ProgramCommonData> ProgramCommonDataPtr;

}

/*
    Описание реализации 
*/

struct Program::Impl
{
  ProgramCommonDataPtr common_data; //общие данные программы
  
///Конструктор
  Impl ()
    : common_data (new ProgramCommonData, false)
  {
  }
  
  Impl (const Impl& impl, const ShaderOptions& options)
    : common_data (impl.common_data)
  {
    //??????????????
    throw xtl::make_not_implemented_exception ("render::Program::Impl::Impl");    
  }
};

/*
    Конструкторы / деструктор
*/

Program::Program ()
  : impl (new Impl)
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

Program& Program::DerivedProgram (const ShaderOptions&)
{
  throw xtl::make_not_implemented_exception ("render::Program::DerivedProgram");
}

Program& Program::DerivedProgram (const ShaderOptionsCache&)
{
  throw xtl::make_not_implemented_exception ("render::Program::DerivedProgram");
}

/*
    Получение низкоуровневой программы шейдинга
*/

const LowLevelProgramPtr& Program::LowLevelProgram (render::low_level::IDevice&)
{
  throw xtl::make_not_implemented_exception ("render::Program::LowLevelProgram");
}
