#include <stl/string>
#include <stl/hash_map>

#include "shared.h"

using namespace stl;

using namespace common;

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

void get_base_profile (const char* source, string& profile)
{
  char* point = (char*)strchr (source, '.');

  if (point)
    profile.assign (source, point - source);
  else
    profile.assign (source);
}

}

/*
    Описание реализации ShaderStage
*/

struct ShaderStage::Impl: public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор/Деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl  (const ContextManager& context_manager);
    ~Impl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание шейдеров
///////////////////////////////////////////////////////////////////////////////////////////////////
    IShaderParametersLayout* CreateShaderParametersLayout (const ShaderParametersLayoutDesc&);
    IShader*                 CreateShader                 (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log);
    IBuffer*                 CreateConstantBuffer         (const BufferDesc& desc);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetShader                 (IShader* shader);
    void SetShaderParametersLayout (IShaderParametersLayout* parameters_layout);
    void SetConstantBuffer         (size_t buffer_slot, IBuffer* buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    IShaderParametersLayout* GetShaderParametersLayout () const;
    IShader*                 GetShader                 () const;
    IBuffer*                 GetConstantBuffer         (size_t buffer_slot) const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление менеджера и его профилей
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddShaderManager (ShaderManager*);

  private:    
    typedef xtl::trackable_ptr<ShaderParametersLayout>                ShaderParametersLayoutPtr;
    typedef xtl::trackable_ptr<Shader>                                ShaderPtr;
    typedef stl::vector <ShaderManager*>                              ShaderManagerArray;
    typedef stl::hash_map<stl::hash_key<const char*>, ShaderManager*> ShaderManagerMap;

  public:
    ShaderParametersLayoutPtr parameters_layout;                                    //расположение параметров шейдера
    ShaderPtr                 shader;                                               //шейдер
    ConstantBufferPtr         constant_buffers[DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //константные буферы шейдера
    ShaderManagerArray        shader_managers;                                      //массив менеджеров
    ShaderManagerMap          shader_managers_map;                                  //соответствие профиль/менеджер
};


/*
   Конструктор / Деструктор
*/

ShaderStage::Impl::Impl (const ContextManager& context_manager)
  : ContextObject (context_manager)
{
  AddShaderManager (create_glsl_shader_manager (context_manager));
}

ShaderStage::Impl::~Impl ()
{
  for (ShaderManagerArray::iterator i = shader_managers.begin (); i != shader_managers.end (); i++)
    delete *i;
}

/*
   Биндинг состояния, вьюпорта и отсечения
*/

void ShaderStage::Impl::Bind ()
{
  if (!shader)
    RaiseInvalidOperation ("render::low_level::opengl::ShaderStage::Impl::Bind", "Null shader");

  shader->Bind (constant_buffers, parameters_layout.get ());
}

/*
   Создание шейдеров
*/

IShaderParametersLayout* ShaderStage::Impl::CreateShaderParametersLayout (const ShaderParametersLayoutDesc& desc)
{
  IShaderParametersLayout* ret_value = new ShaderParametersLayout (GetContextManager ());

  ret_value->SetDesc (desc);

  return ret_value;
}

IShader* ShaderStage::Impl::CreateShader (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::CreateShader";

  if (!shader_descs)
    RaiseNullArgument (METHOD_NAME, "shader_descs");

  if (!shaders_count)
    RaiseNullArgument (METHOD_NAME, "shaders_count");

  string        profile;
  ShaderManager *manager = NULL;

  ShaderManagerMap::iterator search_result;

  for (size_t i = 0; i < shaders_count; i++)
  {
    get_base_profile (shader_descs[i].profile, profile);

    search_result = shader_managers_map.find (profile.c_str ());

    if (search_result == shader_managers_map.end ())
      RaiseNotSupported (METHOD_NAME, "Shader profile '%s' not supported (no registered shader manager for this profile)", profile.c_str ());

    if (manager && (search_result->second != manager))
      RaiseInvalidArgument (METHOD_NAME, "Can't create shader, detected profiles of different shader managers");

    manager = search_result->second;
  }

  return manager->CreateShader (shaders_count, shader_descs, error_log);
}

IBuffer* ShaderStage::Impl::CreateConstantBuffer (const BufferDesc& desc)
{
  return new ConstantBuffer (GetContextManager (), desc);
}
    
/*
   Установка состояния, вьюпорта и отсечения
*/

void ShaderStage::Impl::SetShaderParametersLayout (IShaderParametersLayout* in_parameters_layout)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetShaderParametersLayout";

  if (!in_parameters_layout)
    RaiseNullArgument (METHOD_NAME, "in_parameters_layout");

  parameters_layout = cast_object <ShaderParametersLayout> (in_parameters_layout, METHOD_NAME, "in_parameters_layout");
}

void ShaderStage::Impl::SetShader (IShader* in_shader)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetShader";

  if (!in_shader)
    RaiseNullArgument (METHOD_NAME, "in_shader");

  shader = cast_object <Shader> (in_shader, METHOD_NAME, "in_shader");
}

void ShaderStage::Impl::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::SetConstantBuffer";

  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");

  if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
    RaiseNotSupported (METHOD_NAME, "Can't set constant buffer to slot %u (maximum supported slots %u)", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

  constant_buffers[buffer_slot] = cast_object <ConstantBuffer> (buffer, METHOD_NAME, "buffer");
}

/*
   Получение состояния, вьюпорта и отсечения
*/

IShaderParametersLayout* ShaderStage::Impl::GetShaderParametersLayout () const
{
  return parameters_layout.get ();
}

IShader* ShaderStage::Impl::GetShader () const
{
  return shader.get ();
}

IBuffer* ShaderStage::Impl::GetConstantBuffer (size_t buffer_slot) const
{
  if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
    RaiseNotSupported ("render::low_level::opengl::ShaderStage::Impl::GetConstantBuffer", "Can't get constant buffer from slot %u (maximum supported slots %u)", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

  return constant_buffers[buffer_slot].get ();
}

/*
   Добавление менеджера и его профилей
*/

void ShaderStage::Impl::AddShaderManager (ShaderManager* manager)
{
  if (!manager)
    RaiseNullArgument ("render::low_level::opengl::ShaderStage::Impl::AddShaderManager", "manager");

  pair<ShaderManagerMap, bool> insert_result;

  shader_managers.push_back (manager);

  for (size_t i = 0; i < manager->GetProfilesCount (); i++)
    shader_managers_map[manager->GetProfile (i)] = manager;    //проверка на исключения!!!
}

/*
   Конструктор / деструктор
*/

ShaderStage::ShaderStage (const ContextManager& context_manager)
  : impl (new Impl (context_manager))
  {}

ShaderStage::~ShaderStage ()
{
}

/*
   Биндинг состояния, вьюпорта и отсечения
*/

void ShaderStage::Bind ()
{
  impl->Bind ();
}

/*
   Создание шейдеров
*/

IShaderParametersLayout* ShaderStage::CreateShaderParametersLayout (const ShaderParametersLayoutDesc& desc)
{
  return impl->CreateShaderParametersLayout (desc);
}

IShader* ShaderStage::CreateShader (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  return impl->CreateShader (shaders_count, shader_descs, error_log);
}

IBuffer* ShaderStage::CreateConstantBuffer (const BufferDesc& desc)
{
  return impl->CreateConstantBuffer (desc);
}
    
/*
   Установка состояния, вьюпорта и отсечения
*/

void ShaderStage::SetShader (IShader* shader)
{
  impl->SetShader (shader);
}

void ShaderStage::SetShaderParametersLayout (IShaderParametersLayout* parameters_layout)
{
  impl->SetShaderParametersLayout (parameters_layout);
}

void ShaderStage::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  impl->SetConstantBuffer (buffer_slot, buffer);
}

/*
   Получение состояния, вьюпорта и отсечения
*/

IShaderParametersLayout* ShaderStage::GetShaderParametersLayout () const
{
  return impl->GetShaderParametersLayout ();
}

IShader* ShaderStage::GetShader () const
{
  return impl->GetShader ();
}

IBuffer* ShaderStage::GetConstantBuffer (size_t buffer_slot) const
{
  return impl->GetConstantBuffer (buffer_slot);
}
