#include <stl/string>
#include <stl/hash_map>

#include <xtl/bind.h>

#include <common/hash.h>

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    Состояние уровня шейдинга
*/

class ShaderStageState: public IStageState
{
  public:  
      //конструкторы
    ShaderStageState (ShaderStageState* in_main_state) : owner (0), main_state (in_main_state) {}
    ShaderStageState (ContextObject* in_owner) : owner (in_owner), main_state (0) {}    

      //установка программы
    void SetProgram (ICompiledProgram* in_program)
    {
      if (in_program == program)
        return;

      program = in_program;

      UpdateNotify ();
    }

      //получение программы
    ICompiledProgram* GetProgram () const
    {
      return program.get ();
    }

      //установка константного буффера
    void SetConstantBuffer (size_t buffer_slot, IBindableBuffer* buffer)
    {
      if (buffer == constant_buffers [buffer_slot])
        return;
      
      constant_buffers [buffer_slot] = buffer;

      UpdateNotify ();
    }

      //получение константного буффера
    IBindableBuffer* GetConstantBuffer (size_t buffer_slot) const
    {
      return constant_buffers [buffer_slot].get ();
    }
    
      //получение массива указателей на константные буферы
    ConstantBufferPtr* GetConstantBuffers ()
    {
      return &constant_buffers [0];
    }
    
      //установка расположения параметров шейдера
    void SetProgramParametersLayout (ProgramParametersLayout* in_parameters_layout)
    {
      if (in_parameters_layout == parameters_layout)
        return;
      
      parameters_layout = in_parameters_layout;
      
      UpdateNotify ();
    }

      //получение расположения параметров шейдера
    ProgramParametersLayout* GetProgramParametersLayout () const
    {
      return parameters_layout.get ();
    }

      //захват состояния
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }
    
      //восстановление состояния
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }

  private:
      //копирование
    void Copy (const ShaderStageState& source, const StateBlockMask& mask)
    {
      if (mask.ss_program)
        SetProgram (source.GetProgram ());

      if (mask.ss_program_parameters_layout)
        SetProgramParametersLayout (source.GetProgramParametersLayout ());

      for (size_t i = 0; i < DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
        if (mask.ss_constant_buffers [i])
          SetConstantBuffer (i, source.GetConstantBuffer (i));
    }

      //оповещение об изменении состояния
    void UpdateNotify ()
    {
      if (!owner)
        return;

      owner->GetContextManager ().StageRebindNotify (Stage_Shading);
    }

  private:
    typedef xtl::trackable_ptr<ShaderStageState>        ShaderStageStatePtr;
    typedef xtl::trackable_ptr<ProgramParametersLayout> ProgramParametersLayoutPtr;
    typedef xtl::trackable_ptr<ICompiledProgram>        ProgramPtr;

  private:
    ContextObject*             owner;                                                 //владелец состояния
    ShaderStageStatePtr        main_state;                                            //основное состояние уровня
    ProgramParametersLayoutPtr parameters_layout;                                     //расположение параметров шейдера
    ProgramPtr                 program;                                               //шейдер
    ConstantBufferPtr          constant_buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //константные буферы шейдера
};

/*
    Программа шейдинга с параметрами
*/

struct ProgramWithParameters
{
  ICompiledProgram*        program;           //программа шейдинга
  ProgramParametersLayout* parameters_layout; //расположение параметров
  
///Конструктор
  ProgramWithParameters (ICompiledProgram* in_program, ProgramParametersLayout* layout) : program (in_program), parameters_layout (layout) {}
  
///Сравнение
  bool operator == (const ProgramWithParameters& p) const { return program == p.program && parameters_layout == p.parameters_layout; }  
};

//получение хэша
size_t hash (const ProgramWithParameters& p)
{
  return stl::hash (p.program, stl::hash (p.parameters_layout));
}

}

/*
    Описание реализации ShaderStage
*/

struct ShaderStage::Impl: public ContextObject, public ShaderStageState
{
  public:
///Конструктор
    Impl (const ContextManager& context_manager)
      : ContextObject (context_manager),
        ShaderStageState (static_cast<ContextObject*> (this))
    {
      try
      {
          //добавление менеджеров шейдеров
          
#ifndef OPENGL_ES_SUPPORT

        if (GetCaps ().has_arb_shading_language_100)
          RegisterManager (ShaderManagerPtr (create_glsl_shader_manager (context_manager), false));

#endif

        if (GetCaps ().has_ffp)
        {
          RegisterManager (ShaderManagerPtr (create_ffp_shader_manager (context_manager), false));

            //регистрация программы "по умолчанию"

          ShaderDesc shader_desc;

          memset (&shader_desc, 0, sizeof (shader_desc));

          shader_desc.name             = "Default shader-stage program";
          shader_desc.source_code_size = ~0;
          shader_desc.source_code      = "";
          shader_desc.profile          = "ffp";
          
          default_program = ProgramPtr (CreateProgram (1, &shader_desc, xtl::bind (&Impl::LogShaderMessage, this, _1)), false);
        }

#ifndef OPENGL_ES_SUPPORT

        if (!default_program && GetCaps ().has_arb_shading_language_100)
        {
            //регистрация программы "по умолчанию"

            const char* PIXEL_SHADER  = "void main (void) {gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0);}";
            const char* VERTEX_SHADER = "void main (void) {gl_Position = gl_Vertex;}";

            ShaderDesc shader_descs [] = {
              {"Default shader-stage pixel shader", strlen (PIXEL_SHADER), PIXEL_SHADER, "glsl.ps", ""},
              {"Default shader-stage vertex shader", strlen (VERTEX_SHADER), VERTEX_SHADER, "glsl.vs", ""}
            };

            default_program = ProgramPtr (CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, xtl::bind (&Impl::LogShaderMessage, this, _1)), false);
        }

#endif

        if (!default_program)
          throw ("", "Can't create default program, no supported profiles");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::Impl");
        throw;
      }
    }

///Установка состояния в контекст OpenGL
    void Bind ()
    {
      try
      {
          //получение программы, устанавливаемой в контекст OpenGL

        IBindableProgram& bindable_program = GetBindableProgram ();

          //установка программы в контекст OpenGL

        bindable_program.Bind (GetConstantBuffers ());
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::Bind");
        throw;
      }
    }       

///Создание программы
    ICompiledProgram* CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
    {
      try
      {
          //проверка корректности аргументов        

        if (!shader_descs)
          throw xtl::make_null_argument_exception ("", "shader_descs");

        if (!shaders_count)
          throw xtl::make_null_argument_exception ("", "shaders_count");

        IShaderManager* manager = 0;

        for (size_t i=0; i<shaders_count; i++)
        {
          const ShaderDesc& desc = shader_descs [i];

          if (!desc.source_code)
            throw xtl::format_exception<xtl::null_argument_exception> ("", "shader_descs[%u].source_code", i);

            //получение имени базового профиля

          if (!desc.profile)
            throw xtl::format_exception<xtl::null_argument_exception> ("", "Null argument 'shader_descs[%u].profile'", i);

            //поиск менеджера шейдеров по имени базового профиля

          ProfileMap::iterator iter = profiles.find (desc.profile);

          if (iter == profiles.end ())
            throw xtl::format_not_supported_exception ("", "Shader profile '%s' not supported", desc.profile);

          if (manager && iter->second != manager)
            throw xtl::format_exception<xtl::argument_exception> ("", "Invalid argument 'shader_descs[%u].profile'='%s'. "
              "Incompatible shader profiles (previous profile is '%s')", i, desc.profile, shader_descs [0].profile);
              
            //сохранение менеджера

          manager = iter->second.get ();
        }
        
          //оповещение о необходимости ребиндинга уровня

        StageRebindNotify (Stage_Shading);        

          //компиляция шейдеров        

        stl::vector<ShaderPtr> program_shaders;

        program_shaders.reserve (shaders_count);

        for (size_t i=0; i<shaders_count; i++)
        {
            //корректировка описания шейдера

          ShaderDesc desc = shader_descs [i];

          if (!desc.name)
            desc.name = "__unnamed__";

          if (!desc.options)
            desc.options = "";

          if (desc.source_code_size == size_t (~0))
            desc.source_code_size = strlen (desc.source_code);

            //добавление скомпилированного шейдера к шейдерам программы

          program_shaders.push_back (GetShader (desc, *manager, error_log));
        }

          //формирование массива указателей на шейдеры

        stl::vector<IShader*> program_shader_pointers;

        program_shader_pointers.resize (program_shaders.size ());

        for (size_t i=0; i<program_shaders.size (); i++)
          program_shader_pointers [i] = program_shaders [i].get ();

          //создание программы шейдинга

        return manager->CreateProgram (program_shader_pointers.size (), &program_shader_pointers [0], error_log);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::CreateProgram");
        throw;
      }
    }
    
///Получение строки поддерживаемых профилей шейдера
    const char* GetShaderProfilesString () const
    {
      return supported_profiles.c_str ();
    }
    
  private:
    typedef xtl::com_ptr<IShaderManager>                                ShaderManagerPtr;
    typedef xtl::com_ptr<IShader>                                       ShaderPtr;
    typedef stl::hash_map<size_t, IShader*>                             ShaderMap;
    typedef xtl::com_ptr<ICompiledProgram>                              ProgramPtr;
    typedef xtl::com_ptr<IBindableProgram>                              BindableProgramPtr;
    typedef stl::hash_map<ProgramWithParameters, BindableProgramPtr>    BindableProgramMap;
    typedef stl::hash_map<stl::hash_key<const char*>, ShaderManagerPtr> ProfileMap;

  private:
///Регистрация менеджера шейдеров
    void RegisterManager (const ShaderManagerPtr& manager)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ShaderStage::Impl::RegisterManager";

        //проверка корректности аргументов

      if (!manager)
        throw xtl::make_null_argument_exception (METHOD_NAME, "manager");

        //регистрация профилей
        
      size_t start_profiles_string_size = supported_profiles.size ();

      for (size_t i=0; i<manager->GetProfilesCount (); i++)
      {
        try
        {
          const char* profile = manager->GetProfile (i);
          
          if (!profile)
            throw xtl::format_operation_exception (METHOD_NAME, "Null profile");

          ProfileMap::iterator iter = profiles.find (profile);

          if (iter != profiles.end ())
            throw xtl::format_operation_exception (METHOD_NAME, "Profile '%s' already registered", profile);

          profiles [profile] = manager.get ();

          if (!supported_profiles.empty ())
            supported_profiles += ' ';

          supported_profiles += profile;
        }
        catch (...)
        {
          while (i--)
            profiles.erase (manager->GetProfile (i));

          supported_profiles.resize (start_profiles_string_size);

          throw;
        }
      }      
    }  
  
///Получение хэша шейдера
    size_t GetShaderHash (const ShaderDesc& desc)
    {
      return common::crc32 (desc.source_code, desc.source_code_size, common::strhash (desc.profile, common::strhash (desc.options)));
    }

///Получение шейдера
    ShaderPtr GetShader (const ShaderDesc& desc, IShaderManager& manager, const LogFunction& error_log)
    {
      try
      {
          //получение хэша шейдера

        size_t shader_hash = GetShaderHash (desc);

          //поиск шейдера в карте скомпилированных шейдеров

        ShaderMap::iterator iter = shaders.find (shader_hash);

        if (iter != shaders.end ())
          return iter->second;
          
          //создание нового шейдера

        ShaderPtr shader (manager.CreateShader (desc, error_log), false);

          //регистрация скомпилированного шейдера

        shader->RegisterDestroyHandler (xtl::bind (&Impl::RemoveShaderByHash, this, shader_hash), GetTrackable ());

        shaders.insert_pair (shader_hash, shader.get ());

        return shader;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::GetShader");
        throw;
      }
    }    
    
///Удаление шейдера по хэшу
    void RemoveShaderByHash (size_t shader_hash)
    {
      shaders.erase (shader_hash);
    }
    
///Получение программы с возможностью установки в контекст OpenGL
    IBindableProgram& GetBindableProgram (ICompiledProgram* program, ProgramParametersLayout* layout)
    {
      try
      {
          //поиск программы в карте

        ProgramWithParameters parametrized_program (program, layout);

        BindableProgramMap::iterator iter = bindable_programs.find (parametrized_program);

        if (iter != bindable_programs.end ())
          return *iter->second;

          //проверка корректности аргументов

        if (!program)
          throw xtl::make_null_argument_exception ("", "program");
          
          //создание программы

        BindableProgramPtr bindable_program (program->CreateBindableProgram (layout), false);
        
          //регистрация в карте          

        iter = bindable_programs.insert_pair (parametrized_program, bindable_program).first;
        
          //регистрация обработчиков оповещения об удалении программы
          
        xtl::connection c;

        try
        {
          xtl::trackable::function_type on_destroy_program (xtl::bind (&Impl::RemoveBindableProgram, this, program, (ProgramParametersLayout*)0));
          xtl::trackable::function_type on_destroy_layout (xtl::bind (&Impl::RemoveBindableProgram, this, (ICompiledProgram*)0, layout));

          c = program->RegisterDestroyHandler (on_destroy_program, GetTrackable ());

          if (layout)
            layout->RegisterDestroyHandler (on_destroy_layout, GetTrackable ());
        }
        catch (...)
        {
          c.disconnect ();
          bindable_programs.erase (iter);
          throw;
        }

        return *bindable_program;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ShaderStage::Impl::CreateShadingState");
        throw;
      }
    }

    IBindableProgram& GetBindableProgram ()
    {
      ICompiledProgram* program = GetProgram ();
      
      if (!program)
        program = default_program.get ();

      return GetBindableProgram (program, GetProgramParametersLayout ());
    }
    
///Удаление программы, устанавливаемой в контекст OpenGL
    void RemoveBindableProgram (ICompiledProgram* program, ProgramParametersLayout* layout)
    {
      for (BindableProgramMap::iterator iter = bindable_programs.begin (), end = bindable_programs.end (); iter != end;)
      {
        if (iter->first.program == program || iter->first.parameters_layout == layout)
        {
          BindableProgramMap::iterator next = iter;

          ++next;

          bindable_programs.erase (iter);

          iter = next;
        }
        else
          ++iter;
      }
    }

///Протоколирование ошибок шейдера
    void LogShaderMessage (const char* message)
    {
      if (!message || !*message)
        return;

      LogPrintf ("%s", message);
    }

  private:
    ShaderMap          shaders;            //скомпилированные шейдеры
    BindableProgramMap bindable_programs;  //карта программ шейдинга с возможностью установки в контекст OpenGL
    ProfileMap         profiles;           //соответствие профиль -> менеджер шейдеров
    ProgramPtr         default_program;    //программа "по умолчанию"
    stl::string        supported_profiles; //поддерживаемые профили шейдеров
};

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
    Создание объекта состояния уровня
*/

IStageState* ShaderStage::CreateStageState ()
{
  return new ShaderStageState (static_cast<ShaderStageState*> (&*impl));
}

/*
    Установка состояния уровня в контекст OpenGL
*/

void ShaderStage::Bind ()
{
  impl->Bind ();
}

/*
    Создание ресурсов уровня шейдинга
*/

IProgramParametersLayout* ShaderStage::CreateProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
{
  try
  {    
    return new ProgramParametersLayout (impl->GetContextManager (), desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::ShaderStage::CreateProgramParametersLayout");
    throw;
  }
}

IProgram* ShaderStage::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  return impl->CreateProgram (shaders_count, shader_descs, error_log);
}
   
/*
    Установка параметров уровня шейдинга
*/

void ShaderStage::SetProgram (IProgram* program)
{
  try
  {
      //проверка совместимости контекстов

    ContextObject* program_object = cast_object<ContextObject> (program, "", "program");

    if (program_object && !program_object->IsCompatible (*impl))
      throw xtl::format_exception<xtl::bad_argument> ("", "Argument 'program' is incompatible with target IDevice");

      //установка программы

    impl->SetProgram (cast_object<ICompiledProgram> (program, "", "program"));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::ShaderStage::SetProgram");
    throw;
  }
}

void ShaderStage::SetProgramParametersLayout (IProgramParametersLayout* parameters_layout)
{
  impl->SetProgramParametersLayout (cast_object<ProgramParametersLayout> (*impl, parameters_layout, "render::low_level::opengl::ShaderStage::SetProgramParametersLayout", "parameters_layout"));
}

void ShaderStage::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  try
  {
      //проверка корректности индекса константного буфера
    
    if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "buffer_slot", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

      //проверка совместимости контекстов

    ContextObject* buffer_object = cast_object<ContextObject> (buffer, "", "buffer");      

    if (buffer_object && !buffer_object->IsCompatible (*impl))
      throw xtl::format_exception<xtl::bad_argument> ("", "Argument 'buffer' is incompatible with target IDevice");

      //установка буфера

    impl->SetConstantBuffer (buffer_slot, cast_object<IBindableBuffer> (buffer, "", "buffer"));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::ShaderStage::SetConstantBuffer");
    throw;
  }
}

/*
    Получение параметров уровня шейдинга
*/

IProgramParametersLayout* ShaderStage::GetProgramParametersLayout () const
{
  return impl->GetProgramParametersLayout ();
}

IProgram* ShaderStage::GetProgram () const
{
  return impl->GetProgram ();
}

IBuffer* ShaderStage::GetConstantBuffer (size_t buffer_slot) const
{
  if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
    throw xtl::make_range_exception ("render::low_level::opengl::ShaderStage::GetConstantBuffer", "buffer_slot", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

  return impl->GetConstantBuffer (buffer_slot);
}

/*
    Получение строки поддерживаемых контекстом профилей шейдеров
*/

const char* ShaderStage::GetShaderProfilesString () const
{
  return impl->GetShaderProfilesString ();
}
