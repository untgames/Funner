#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

ShaderLibrary::ShaderLibrary (const DeviceManager& device_manager)
  : DeviceObject (device_manager)
{
}

ShaderLibrary::~ShaderLibrary ()
{
}

/*
    Создание шейдера
*/

ShaderPtr ShaderLibrary::CreateShader (const ShaderDesc& desc, const LogFunction& error_log)
{
  try
  {
      //проверка корректности аргументов

    if (!desc.name)
      throw xtl::make_null_argument_exception ("", "desc.name");

    if (!desc.source_code)
      throw xtl::make_null_argument_exception ("", "desc.source_code");

    if (!desc.source_code_size)
      throw xtl::make_null_argument_exception ("", "desc.source_code_size");

    if (!desc.profile)
      throw xtl::make_null_argument_exception ("", "desc.profile");

    const char* options = desc.options ? desc.options : "";

    ShaderType type = (ShaderType)-1;

    struct Profile2ShaderType
    {
      const char* profile;
      ShaderType  type;
      const char* dx_profile;
    };

    static const Profile2ShaderType type_map [] = {
      {"hlsl.vs", ShaderType_Vertex,   "vs_1_1"},
      {"hlsl.ps", ShaderType_Pixel,    "ps_1_1"},
      {"hlsl.gs", ShaderType_Geometry, "gs_1_1"},
      {"hlsl.hs", ShaderType_Hull,     "hs_1_1"},
      {"hlsl.ds", ShaderType_Domain,   "ds_1_1"},
      {"hlsl.cs", ShaderType_Compute,  "cs_1_1"},
    };

    static const size_t type_map_size = sizeof (type_map) / sizeof (*type_map);

    const char* dx_profile = "";

    for (size_t i=0; i<type_map_size; i++)
      if (!strcmp (type_map [i].profile, desc.profile))
      {
        type       = type_map [i].type;
        dx_profile = type_map [i].dx_profile;
        break;
      }

    if (type == (ShaderType)-1)
      throw xtl::make_argument_exception ("", "desc.profile", desc.profile, "Unknown shader profile");

      //формирование хэша для поиска в библиотеке

    size_t source_code_size = desc.source_code_size == (size_t)-1 ? strlen (desc.source_code) : desc.source_code_size,
           hash             = common::crc32 (desc.source_code, source_code_size, common::strhash (options));

      //поиск уже существующего шейдера

    ShaderMap::iterator iter = shaders.find (hash);

    if (iter != shaders.end ())
      return iter->second;

      //создание нового шейдера

    ShaderCodePtr code (new ShaderCode (GetDeviceManager (), desc.name, dx_profile, desc.source_code, source_code_size, options, error_log), false);
    ShaderPtr     shader (new Shader (type, code, *this), false);

     //регистрация шейдера в библиотеке

    shader->RegisterDestroyHandler (xtl::bind (&ShaderLibrary::RemoveShaderByHash, this, hash), GetTrackable ());

    shaders.insert_pair (hash, shader.get ());

    return shader;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderLibrary::CreateShader");
    throw;
  }
}

void ShaderLibrary::RemoveShaderByHash (size_t hash)
{
  shaders.erase (hash);
}

/*
    Поиск лэйаута константного буфера по хэшу
*/

ConstantBufferLayoutPtr ShaderLibrary::FindConstantBufferLayout (size_t hash) const
{
  BufferLayoutMap::const_iterator iter = buffer_layouts.find (hash);

  if (iter != buffer_layouts.end ())
    return iter->second;

  return ConstantBufferLayoutPtr ();
}

/*
    Добавление / удаление лэйаута константного буфера по хэшу
*/

ConstantBufferLayoutPtr ShaderLibrary::AddConstantBufferLayout (const ConstantBufferLayoutPtr& layout)
{
  try
  {
    if (!layout)
      throw xtl::make_null_argument_exception ("", "layout");

    size_t hash = layout->GetHash ();

    BufferLayoutMap::const_iterator iter = buffer_layouts.find (hash);

    if (iter != buffer_layouts.end ())
      return iter->second;

    buffer_layouts.insert_pair (hash, layout.get ());

    try
    {
      layout->connect_tracker (xtl::bind (&ShaderLibrary::RemoveConstantBufferLayout, this, hash), GetTrackable ());
    }
    catch (...)
    {
      buffer_layouts.erase (hash);
      throw;
    }

    return layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderLibrary::AddConstantBufferLayout");
    throw;
  }
}

void ShaderLibrary::RemoveConstantBufferLayout (size_t hash)
{
  buffer_layouts.erase (hash);
}

/*
    Получение синхронизатора
*/

ShaderBuffersSynchronizer& ShaderLibrary::GetSynchronizer (const ProgramParametersLayout& src_layout, const ConstantBufferLayout& dst_layout)
{
  try
  {
    SyncLayoutPair key (&src_layout, &dst_layout);

    SyncLayoutMap::iterator iter = layout_syncs.find (key);

    if (iter != layout_syncs.end ())
      return *iter->second;

    ShaderBuffersSynchronizerPtr synchronizer (new ShaderBuffersSynchronizer (src_layout, dst_layout), false);

    layout_syncs.insert_pair (key, synchronizer);

    try
    {
      xtl::trackable::function_type track_handler (xtl::bind (&ShaderLibrary::RemoveSynchronizer, this, key));

      const_cast<ProgramParametersLayout&> (src_layout).RegisterDestroyHandler (track_handler, GetTrackable ());
      const_cast<ConstantBufferLayout&> (dst_layout).connect_tracker (track_handler, GetTrackable ());
    }
    catch (...)
    {
      layout_syncs.erase (key);
      throw;
    }

    return *synchronizer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderLibrary::GetSynchronizer");
    throw;
  }
}

void ShaderLibrary::RemoveSynchronizer (const SyncLayoutPair& p)
{
  layout_syncs.erase (p);
}

/*
    Получение целевого константного буфера
*/

TargetConstantBufferPtr ShaderLibrary::FindConstantBuffer (size_t hash) const
{
  BufferMap::const_iterator iter = buffers.find (hash);

  if (iter != buffers.end ())
    return iter->second;

  return TargetConstantBufferPtr ();
}

/*
    Регистрация и удаление константного буфера
*/

void ShaderLibrary::AddConstantBuffer (size_t hash, const TargetConstantBufferPtr& buffer)
{
  buffers.insert_pair (hash, buffer);
}

void ShaderLibrary::RemoveConstantBuffer (size_t hash)
{
  buffers.erase (hash);
}

/*
    Получение программы, устанавливаемой в контекст
*/

BindableProgram& ShaderLibrary::GetBindableProgram (const Program& program, const ProgramParametersLayout& layout)
{
  try
  {
    BindableProgramKey key (&program, &layout);

    BindableProgramMap::iterator iter = bindable_programs.find (key);

    if (iter != bindable_programs.end ())
      return *iter->second;

    BindableProgramPtr bindable_program (new BindableProgram (*this, program, layout), false);

    xtl::trackable::function_type tracker (xtl::bind (&ShaderLibrary::RemoveBindableProgram, this, key));

    bindable_programs.insert_pair (key, bindable_program);    

    try
    {    
      const_cast<Program&> (program).RegisterDestroyHandler (tracker, GetTrackable ());
      const_cast<ProgramParametersLayout&> (layout).RegisterDestroyHandler (tracker, GetTrackable ());
    }
    catch (...)
    {
      bindable_programs.erase (key);
      throw;
    }

    return *bindable_program;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderLibrary::GetBindableProgram");
    throw;
  }
}

void ShaderLibrary::RemoveBindableProgram (const BindableProgramKey& key)
{
  bindable_programs.erase (key);
}

/*
    Поиск входного лэйаута
*/

DxInputLayoutPtr ShaderLibrary::FindInputLayout (size_t input_hash, size_t shader_hash) const
{
  InputLayoutKey key (input_hash, shader_hash);

  InputLayoutMap::const_iterator iter = input_layouts.find (key);

  if (iter != input_layouts.end ())
    return iter->second;

  return DxInputLayoutPtr ();
}

/*
    Регистрация и удаление входного лэйаута
*/

void ShaderLibrary::AddInputLayout (size_t input_hash, size_t shader_hash, const DxInputLayoutPtr& layout)
{
  InputLayoutKey key (input_hash, shader_hash);

  InputLayoutMap::iterator iter = input_layouts.find (key);

  if (iter != input_layouts.end ())
    throw xtl::format_operation_exception ("render::low_level::dx11:ShaderLibrary::AddInputLayout", "InputLayout for this shader has been already added");

  input_layouts.insert_pair (key, layout);
}

void ShaderLibrary::RemoveInputLayout (size_t input_hash, size_t shader_hash)
{
  input_layouts.erase (InputLayoutKey (input_hash, shader_hash));
}
