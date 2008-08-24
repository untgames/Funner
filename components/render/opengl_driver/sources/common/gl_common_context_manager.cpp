#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
===================================================================================================
    Утилиты
===================================================================================================    
*/

struct OpenGLException: virtual public xtl::exception {};

/*
===================================================================================================
    Описание реализации контекста OpenGL
===================================================================================================
*/

class ContextImpl: public xtl::reference_counter, private IContextListener
{
  public:
///Конструктор
    ContextImpl (ISwapChain* swap_chain, IContext* shared_context) : master_swap_chain (swap_chain)
    {
      try
      {
          //создание контекста

        context = ContextPtr (PlatformManager::CreateContext (swap_chain, shared_context), false);        
        
          //подписка на события контекста

        context->AttachListener (this);

          //выбор активного контекста          
        
        context->MakeCurrent (master_swap_chain.get ());        
        
          //инициализация таблицы точек входа OpenGL 1.1
          
        gl_entries.Init (context->GetLibrary ());
        
          //отключение буферов отрисовки          

        glDrawBuffer (GL_NONE);
        glReadBuffer (GL_NONE);

          //определение поддержки расширений        

        extensions_string = reinterpret_cast<const char*> (glGetString (GL_EXTENSIONS));
        version_string    = reinterpret_cast<const char*> (glGetString (GL_VERSION));
        vendor_string     = reinterpret_cast<const char*> (glGetString (GL_VENDOR));
        renderer_string   = reinterpret_cast<const char*> (glGetString (GL_RENDERER));
        
          //добавление не переносимых расширений 
          
        IPropertyList* swap_chain_properties = swap_chain->GetProperties ();
        
        if (swap_chain_properties)
        {
          for (size_t i=0, count=swap_chain_properties->GetSize (); i<count; i++)
            if (!xtl::xstricmp (swap_chain_properties->GetKey (i), "gl_extensions"))
            {
              const char* extensions = swap_chain_properties->GetValue (i);

              if (*extensions)
              {
                extensions_string += ' ';
                extensions_string += extensions;
              }
              
              break;
            }
        }

          //определение багов работы OpenGL

        detect_opengl_bugs (extensions_string);

          //установка флагов поддержки расширений

        extensions.SetGroup (extensions_string.c_str (), true);

          //определение поддержки версий

          //!!!добавить определение поддержки версий!!!                    
          
/*        static Extension Version_1_1 ("GL_VERSION_1_1"), Version_1_2 ("GL_VERSION_1_2"), Version_1_3 ("GL_VERSION_1_3"),
                         Version_1_4 ("GL_VERSION_1_4"), Version_1_5 ("GL_VERSION_1_5"), Version_2_0 ("GL_VERSION_2_0"),
                         Version_2_1 ("GL_VERSION_2_1");

        extensions.Set (Version_1_1, GLEW_VERSION_1_1 != 0);
        extensions.Set (Version_1_2, GLEW_VERSION_1_2 != 0);
        extensions.Set (Version_1_3, GLEW_VERSION_1_3 != 0);
        extensions.Set (Version_1_4, GLEW_VERSION_1_4 != 0);
        extensions.Set (Version_1_5, GLEW_VERSION_1_5 != 0);
        extensions.Set (Version_2_0, GLEW_VERSION_2_0 != 0);
        extensions.Set (Version_2_1, GLEW_VERSION_2_1 != 0);*/
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ContextImpl::ContextImpl");        
        throw;
      }
    }

///Получение контекста и главной цепочки обмена
    IContext&   GetContext         () const { return *context; }
    ISwapChain& GetMasterSwapChain () const { return *master_swap_chain; }
    
///Получение точек входа OpenGL 1.1
    const GlEntries& GetGlEntries () const { return gl_entries; }
    
///Получение флагов поддержки исключений
    ExtensionSet& GetExtensions () { return extensions; } 
 
///Получение аппаратно-поддерживаемых возможностей контекста
    const ContextCaps& GetCaps () const
    {
      return context_caps;
    }

///Получение информации о реализации OpenGL
    const char* GetExtensionsString () const { return extensions_string.c_str (); }
    const char* GetVersionString    () const { return version_string.c_str (); }
    const char* GetVendorString     () const { return vendor_string.c_str (); }
    const char* GetRendererString   () const { return renderer_string.c_str (); }

///Получение таблицы локальных данных контекста
    ContextDataTable& GetContextDataTable (Stage table_id)
    {
      if (table_id < 0 || table_id >= Stage_Num)
        throw xtl::make_range_exception ("render::low_level::opengl::ContextImpl::GetContextDataTable", "table_id", table_id, 0, Stage_Num);

      return context_data_table [table_id];
    }

///Инициализация таблицы возможностей контекста
    void InitContextCaps (const ExtensionSet& enabled_extensions)
    {
      context_caps.Init (extensions, enabled_extensions);
    }
    
///Получение текущего контекста
    static ContextImpl* GetCurrentContext ()
    {
      return current_context;
    }
    
  private:
///Контекст стал текущим
    void OnSetCurrent ()
    {
      current_context = this;
    } 

///Контекст перестал быть текущим
    virtual void OnLostCurrent ()
    {
      current_context = 0;
    }

  private:
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;
    typedef xtl::com_ptr<IContext>   ContextPtr;
    
  private:
    ContextPtr       context;                        //контекст OpenGL
    GlEntries        gl_entries;                     //таблица точек входа OpenGL 1.1
    SwapChainPtr     master_swap_chain;              //главная цепочка обмена, связанная с контекстом
    ExtensionSet     extensions;                     //флаги поддерживаемых исключений
    stl::string      extensions_string;              //строка поддерживаемых расширений
    stl::string      version_string;                 //версия OpenGL
    stl::string      vendor_string;                  //производитель реализации OpenGL
    stl::string      renderer_string;                //имя устройства отрисовки OpenGL
    ContextDataTable context_data_table [Stage_Num]; //таблицы локальных данных контекста
    ContextCaps      context_caps;                   //аппаратно поддерживаемые возможности контекста

  private:
    static ContextImpl* current_context; //указатель на текущий контекст        
};

ContextImpl* ContextImpl::current_context = 0;

}

/*
===================================================================================================
    Описание реализации ContextManager
===================================================================================================
*/

struct ContextManager::Impl: public xtl::reference_counter
{
  public:  
///Конструктор
    Impl (const LogHandler& in_log_handler, const char* init_string) :
      log_handler (in_log_handler),
      current_context (0),
      current_swap_chain (0),
      current_context_id (0),
      next_context_id (1),
      check_gl_errors (true),
      on_destroy_swap_chain (xtl::bind (&Impl::RestoreContext, this))
    {
        //разбор строки инициализации устройства

      if (!init_string)
        throw xtl::make_null_argument_exception ("render::low_level::opengl::ContextManager::ContextManager", "init_string");          

      enabled_extensions.Set (true);      

      common::parse_init_string (init_string, xtl::bind (&Impl::AddInitProperty, this, _1, _2));

        //инициализация флагов ребиндинга уровней

      for (size_t stage=0; stage<Stage_Num; stage++)
        need_stage_rebind [stage] = true;
    }    
    
///Создание контекста
    size_t CreateContext (ISwapChain* swap_chain)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::CreateContext";         

        //проверка корректности аргументов

      if (!swap_chain)
        throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain");

        //создание контекста

      IContext*      shared_context = context_map.empty () ? (IContext*)0 : &context_map.begin ()->second->GetContext (); 
      ContextImplPtr new_context (new ContextImpl (swap_chain, shared_context), false);

        //проверка поддержки требуемых расширений и версий
      
      ExtensionSet& context_extensions = new_context->GetExtensions ();

      context_extensions &= enabled_extensions;
      
      for (size_t id=0, count=ExtensionSet::Size (); id<count; id++)
        if (required_extensions.Get (id) && !context_extensions.Get (id))
        {
          throw xtl::format_not_supported_exception (METHOD_NAME, "Could not create new context. Reason: required extension '%s' not supported",
            get_extension_name (id));
        }        
        
      if (!min_version.empty () && xtl::xstrcmp (new_context->GetVersionString (), min_version.c_str ()) < 0)
      {
        throw xtl::format_not_supported_exception (METHOD_NAME, "Could not create new context. Reason: OpenGL version '%s' not supported "
          "(version='%s')", min_version.c_str (), new_context->GetVersionString ());
      }      

      if (!max_version.empty ())
      {
        static const char*  version_string [] = {"1.1", "1.2", "1.3", "1.4", "1.5", "2.0", "2.1"};        
        static const size_t versions_count    = sizeof (version_string) / sizeof (*version_string);

        static Extension extension [] = {
          Extension ("GL_VERSION_1_1"),
          Extension ("GL_VERSION_1_2"),
          Extension ("GL_VERSION_1_3"),
          Extension ("GL_VERSION_1_4"),
          Extension ("GL_VERSION_1_5"),
          Extension ("GL_VERSION_2_0"),
          Extension ("GL_VERSION_2_1"),
        };

        for (size_t i=0; i<versions_count; i++)
        {
          if (xtl::xstrcmp (max_version.c_str (), version_string [i]) < 0)
            context_extensions.Set (extension [i], false);
        }        
      }
      
        //инициализация таблицы возможностей контекста
      
      new_context->InitContextCaps (enabled_extensions);

        //регистрация контекста

      size_t new_context_id = next_context_id;

      context_map.insert_pair (new_context_id, new_context);

      next_context_id++;

      return new_context_id;
    }
    
///Удаление контекста
    void DeleteContext (size_t context_id)
    {
      context_map.erase (context_id);

      if (context_id == current_context_id)
        RestoreContext ();
    }
    
///Установка текущего контекста
    void SetContext (size_t context_id, ISwapChain* swap_chain)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::SetContext";
      
      if (context_id == current_context_id && swap_chain == current_swap_chain)
        return;
        
      if (!context_id && !swap_chain)
        throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain && context_id");

      if (!context_id) //поиск совместимого контекста
      {
        try
        {
          context_id = GetCompatibleContextId (swap_chain);
        }
        catch (xtl::exception& exception)
        {
          exception.touch (METHOD_NAME);
          
          throw;
        }
      }

        //поиск контекста по его идентификатору
        
      ContextMap::iterator iter = context_map.find (context_id);
      
      if (iter == context_map.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "context_id", context_id);

      ContextImplPtr context = iter->second;
      
      if (!swap_chain) //использование первичной цепочки обмена
        swap_chain = &context->GetMasterSwapChain ();        

        //проверка совместимости контекста с цепочками обмена

      if (!context->GetContext ().IsCompatible (swap_chain))
        throw xtl::make_argument_exception (METHOD_NAME, "swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

        //регистрация обработчиков удаления цепочек обмена

      swap_chain->RegisterDestroyHandler (on_destroy_swap_chain);

        //обновление данных

      current_context    = &*iter->second;
      current_swap_chain = swap_chain;
      current_context_id = context_id;

        //оповещение о необходимости ребиндинга выходного уровня

      StageRebindNotify (Stage_Output);
    }
    
///Активация текущего контекста
    void MakeContextCurrent (bool clear_errors)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::MakeContextCurrent";
      
        //если текущий контекст не изменился - нет необходимости переключения      

      if (!current_context)
        throw xtl::format_operation_exception (METHOD_NAME, "Null active context");

      try
      {
          //сделать кэширование установки!!!
        
        current_context->GetContext ().MakeCurrent (current_swap_chain);

          //очистка текущей ошибки
          
        if (clear_errors)
          ClearErrors ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);

        throw;
      }
    }    
    
///Получение текущего контекста и цепочек обмена
    ISwapChain*  GetSwapChain () const { return current_swap_chain; }
    size_t       GetContextId () const { return current_context_id; }
    ContextImpl* GetContext   () const { return current_context; }
    
///Создание цепочки обмена совместимой с контекстом
    ISwapChain* CreateCompatibleSwapChain (size_t context_id)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::CreateCompatibleSwapChain(size_t)";

        //поиск контекста

      ContextMap::iterator iter = context_map.find (context_id);

      if (iter == context_map.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "context_id", context_id);

      ContextImplPtr context = iter->second;
      
        //создание цепочки обмена
        
      return PlatformManager::CreatePBuffer (&context->GetMasterSwapChain ());
    }
    
///Создание совместимой цепочки обмена
    ISwapChain* CreateCompatibleSwapChain (ISwapChain* swap_chain)
    {
      if (!swap_chain)
        throw xtl::make_null_argument_exception ("render::low_level::opengl::ContextManager::CreateCompatibleSwapChain(ISwapChain*)", "swap_chain");
      
      return PlatformManager::CreatePBuffer (swap_chain);
    }
    
///Создание совместимой цепочки обмена
    ISwapChain* CreateCompatibleSwapChain (ISwapChain* swap_chain, const SwapChainDesc& desc)
    {
      if (!swap_chain)
        throw xtl::make_null_argument_exception ("render::low_level::opengl::ContextManager::CreateCompatibleSwapChain(ISwapChain*,const SwapChainDesc&)", "swap_chain");
      
      return PlatformManager::CreatePBuffer (swap_chain, &desc);
    }    
    
///Проверка совместимости контекста и цепочки обмена
    bool IsCompatible (size_t context_id, ISwapChain* swap_chain)
    {
        //поиск контекста

      ContextMap::iterator iter = context_map.find (context_id);

      if (iter == context_map.end ())
        return false;

      return iter->second->GetContext ().IsCompatible (swap_chain);
    }
    
///Определение поддержки расширения контекстом
    bool IsSupported (size_t context_id, const Extension& extension) const
    {
        //поиск контекста

      ContextMap::const_iterator iter = context_map.find (context_id);

      if (iter == context_map.end ())
        return false;
        
      return iter->second->GetExtensions ().Get (extension);
    }
      
    bool IsSupported (const Extension& extension) const
    {
      if (!current_context)
        return false;
        
      return current_context->GetExtensions ().Get (extension);
    }
    
///Протоколирование
    void LogPrint (const char* message)
    {
      try
      {
        log_handler (message);
      }
      catch (...)
      {
        //подавляем все исключения
      }
    }
    
///Включение / отключение проверки ошибок
    void SetValidationState (bool state)
    {
      check_gl_errors = state;
    }
    
    bool GetValidationState () const { return check_gl_errors; }
    
//Проверка ошибок OpenGL
    void CheckErrors (const char* source)
    {
      if (!check_gl_errors)
        return;        

      if (!source)
        source = "render::low_level::ContextManager::Impl::CheckErrors";

      MakeContextCurrent (false);

      GLenum error = glGetError ();

      switch (error)
      {
        case GL_NO_ERROR:
          break;
        case GL_INVALID_ENUM:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: invalid enum");
        case GL_INVALID_VALUE:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: invalid value");
        case GL_INVALID_OPERATION:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: invalid operation");
        case GL_STACK_OVERFLOW:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: stack overflow");
        case GL_STACK_UNDERFLOW:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: stack underflow");
        case GL_OUT_OF_MEMORY:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: out of memory");
        case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: invalid framebuffer operation");
        default:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: code=0x%04x", error);
      }      
    }
    
///Очистка текущей ошибки OpenGL
    void ClearErrors ()
    {
      if (!check_gl_errors)
        return;

      while (glGetError () != GL_NO_ERROR);
    }
    
///Оповещение о необходимости ребинда уровня
    void StageRebindNotify (Stage stage)
    {
      if (stage < 0 || stage >= Stage_Num)
        throw xtl::make_argument_exception ("render::low_level::opengl::ContextManager::Impl::StageRebindNotify", "stage", stage);

      need_stage_rebind [stage] = true;
    }
    
///Очистка флагов ребиндинга
    void ResetRebindNotifications ()
    {
      for (size_t stage=0; stage<Stage_Num; stage++)
        need_stage_rebind [stage] = false;
    }
    
///Проверка необходимости ребинда уровня
    bool NeedStageRebind (Stage stage)
    {
      if (stage < 0 || stage >= Stage_Num)
        return false;

      return need_stage_rebind [stage];
    }  
    
  private:
///Добавление свойства
    void AddInitProperty (const char* name, const char* value)
    {
      if (!xtl::xstricmp (name, "require"))
      {
        required_extensions.SetGroup (value, true);
      }
      else if (!xtl::xstricmp (name, "disable"))
      {     
        enabled_extensions.SetGroup (value, false);
      }
      else if (!xtl::xstricmp (name, "enable"))
      {
        enabled_extensions.SetGroup (value, true);
      }
      else if (!xtl::xstrnicmp (name, "gl_", 3))
      {
        if (!xtl::xstrcmp (value, "0"))
        {
          enabled_extensions.Set (name, false);
        }
        else if (!xtl::xstrcmp (value, "1"))
        {
          enabled_extensions.Set (name, true);
        }
        else if (!xtl::xstrcmp (value, "!"))
        {
          required_extensions.Set (name, true);
        }
        else if (!xtl::xstrcmp (value, "1!"))
        {
          enabled_extensions.Set (name, true);
          required_extensions.Set (name, true);
        }
      }
      else if (!xtl::xstricmp (name, "min_version"))
      {
        min_version = value;
      }
      else if (!xtl::xstricmp (name, "max_version"))
      {
        max_version = value;
      }
      else if (!xtl::xstricmp (name, "check_gl_errors"))
      {
        check_gl_errors = atoi (value) != 0;
      }
    }
    
//Поиск контекста, совместимого с указанной цепочкой обмена
    size_t GetCompatibleContextId (ISwapChain* swap_chain)
    {
        //наиболее выгодным является выбор текущего контекста, если это возможно
      
      if (current_context && current_context->GetContext ().IsCompatible (swap_chain))
        return current_context_id;
        
        //поиск контекста среди существующих
        
      for (ContextMap::iterator iter=context_map.begin (), end=context_map.end (); iter != end; ++iter)
        if (iter->second->GetContext ().IsCompatible (swap_chain))
          return iter->first;
          
        //совместимый контекст не найден
        
      throw xtl::format_not_supported_exception ("render::low_level::opengl::ContextManager::Impl::GetCompatibleContextId",
        "No context compatible with swap_chain");

      return 0;
    }
    
///Восставление контекста (после удаления контекста или цепочки обмена)
    void RestoreContext ()
    {
        //оповещение о необходимости ребиндинга выходного уровня

      StageRebindNotify (Stage_Output);      

        //если текущий контекст не удалён - устанавливаем в качестве текущих цепочек обмена основную цепочку обмена контекста
      
      ContextMap::iterator iter = context_map.find (current_context_id);

      if (iter != context_map.end ())
      {
        current_swap_chain = &current_context->GetMasterSwapChain ();

        return;
      }

        //если был удалён текущий контекст
        
      if (context_map.empty ())
      {
          //если удалены все контексты - восставление не возможно

        current_swap_chain = 0;
        current_context    = 0;
        current_context_id = 0;

        return;
      }

        //выбираем в качестве текущего первый контекст из списка созданных вместе с его основной цепочкой обмена

      ContextImplPtr context = context_map.begin ()->second;

      current_context    = &*context;
      current_swap_chain = &context->GetMasterSwapChain ();
      current_context_id = context_map.begin ()->first;
    }

  private:
    typedef xtl::intrusive_ptr<ContextImpl>       ContextImplPtr;  //указатель на реализацию контекста
    typedef stl::hash_map<size_t, ContextImplPtr> ContextMap;      //карта соответствия контекста OpenGL и цепочки обмена
  
  private:
    LogHandler                log_handler;                   //обработчик протоколирования
    ExtensionSet              required_extensions;           //расширения, затребованные в строке иициализации
    ExtensionSet              enabled_extensions;            //расширения, разрешенные к использованию в строке инициализаии
    stl::string               min_version;                   //минимальная требуемой версии OpenGL
    stl::string               max_version;                   //максимальная необходимая версия OpenGL
    bool                      check_gl_errors;               //нужно ли проверять ошибки OpenGL
    ContextMap                context_map;                   //карта отображения цепочки обмена на контекст
    ContextImpl*              current_context;               //текущий контекст
    ISwapChain*               current_swap_chain;            //текущая цепочка обмена
    size_t                    current_context_id;            //идентификатор текущего контекста (0 - отсутствует)
    size_t                    next_context_id;               //номер следующего создаваемого контекста
    xtl::trackable::slot_type on_destroy_swap_chain;         //обработчик удаления цепочки обмена
    bool                      need_stage_rebind [Stage_Num]; //флаги, определяющие необходимость ребиндинга уровня
};

/*
    Конструкторы / деструктор / присваивание
*/

ContextManager::ContextManager (const LogHandler& log_handler, const char* init_string)
  : impl (new Impl (log_handler, init_string))
{
}

ContextManager::ContextManager (const ContextManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

ContextManager::~ContextManager ()
{
  release (impl);
}

ContextManager& ContextManager::operator = (const ContextManager& manager)
{  
  addref (manager.impl);
  release (impl);

  impl = manager.impl;

  return *this;
}

/*
    Создание / удаление контекста
*/

size_t ContextManager::CreateContext (ISwapChain* swap_chain)
{
  return impl->CreateContext (swap_chain);
}

void ContextManager::DeleteContext (size_t context_id)
{
  impl->DeleteContext (context_id);
}

/*
    Создание цепочки обмена совместимой с контекстом
*/

ISwapChain* ContextManager::CreateCompatibleSwapChain (size_t context_id)
{
  return impl->CreateCompatibleSwapChain (context_id);
}

ISwapChain* ContextManager::CreateCompatibleSwapChain (ISwapChain* swap_chain)
{
  return impl->CreateCompatibleSwapChain (swap_chain);
}

ISwapChain* ContextManager::CreateCompatibleSwapChain (ISwapChain* swap_chain, const SwapChainDesc& desc)
{
  return impl->CreateCompatibleSwapChain (swap_chain, desc);
}

/*
    Выбор текущего контекста
*/

void ContextManager::SetContext (size_t context_id, ISwapChain* swap_chain)
{
  impl->SetContext (context_id, swap_chain);
}

ISwapChain* ContextManager::GetSwapChain () const
{
  return impl->GetSwapChain ();
}

size_t ContextManager::GetContextId () const
{
  return impl->GetContextId ();
}

/*
    Активация текущего контекста
*/

void ContextManager::MakeContextCurrent () const
{
  impl->MakeContextCurrent (true);
}

/*
    Работа с таблицами локальных данных контекста
*/

const ContextDataTable& ContextManager::GetContextDataTable (Stage table_id) const
{
  if (!impl->GetContext ())
    throw xtl::format_operation_exception ("render::low_level::opengl::ContextManager::GetContextDataTable", "Null active context");

  return impl->GetContext ()->GetContextDataTable (table_id);
}

ContextDataTable& ContextManager::GetContextDataTable (Stage table_id)
{
  return const_cast<ContextDataTable&> (const_cast<const ContextManager&> (*this).GetContextDataTable (table_id));
}

/*
    Получение информации о текущей реализации OpenGL
*/

const char* ContextManager::GetExtensions () const
{
  if (!impl->GetContext ())
    throw xtl::format_operation_exception ("render::low_level::opengl::ContextManager::GetExtensions", "Null active context");
    
  return impl->GetContext ()->GetExtensionsString ();
}

const char* ContextManager::GetVersion () const
{
  if (!impl->GetContext ())
    throw xtl::format_operation_exception ("render::low_level::opengl::ContextManager::GetVersion", "Null active context");
    
  return impl->GetContext ()->GetVersionString ();
}

const char* ContextManager::GetVendor () const
{
  if (!impl->GetContext ())
    throw xtl::format_operation_exception ("render::low_level::opengl::ContextManager::GetVendor", "Null active context");

  return impl->GetContext ()->GetVendorString ();
}

const char* ContextManager::GetRenderer () const
{
  if (!impl->GetContext ())
    throw xtl::format_operation_exception ("render::low_level::opengl::ContextManager::GetRenderer", "Null active context");

  return impl->GetContext ()->GetRendererString ();
}

/*
    Проверка совместимости контекста и цепочки обмена
*/

bool ContextManager::IsCompatible (size_t context_id, ISwapChain* swap_chain) const
{
  return impl->IsCompatible (context_id, swap_chain);
}

/*
    Проверка совместимости менеджеров контекстов
*/

bool ContextManager::IsCompatible (const ContextManager& manager) const
{
  return impl == manager.impl;
}

/*
    Определение поддержки расширения контекстом
*/

bool ContextManager::IsSupported (size_t context_id, const Extension& extension) const
{
  return impl->IsSupported (context_id, extension);
}

bool ContextManager::IsSupported (const Extension& extension) const
{
  return impl->IsSupported (extension);
}

/*
   Поулчение аппаратно поддерживаемых возможностей контекста
*/

const ContextCaps& ContextManager::GetCaps () const
{
  if (!impl->GetContext ())
    throw xtl::format_operation_exception ("render::low_level::opengl::ContextManager::GetCaps", "Null active context");
    
  return impl->GetContext ()->GetCaps ();
}

/*
    Протоколирование
*/

void ContextManager::LogPrintf (const char* format, ...) const
{
  va_list list;
  
  va_start   (list, format);  
  LogVPrintf (format, list);
}

void ContextManager::LogVPrintf (const char* format, va_list args) const
{
  if (!format)
    return;

  impl->LogPrint (common::vformat (format, args).c_str ());
}

/*
    Проверка ошибок OpenGL
*/

void ContextManager::SetValidationState (bool state)
{
  impl->SetValidationState (state);  
}

bool ContextManager::GetValidationState () const
{
  return impl->GetValidationState ();
}

void ContextManager::CheckErrors (const char* source) const
{
  impl->CheckErrors (source);
}

void ContextManager::RaiseError (const char* source) const
{
  if (!source)
    return;
    
  CheckErrors (source);
  
  throw xtl::format_operation_exception (source, "Invalid operation (internal error)");
}

void ContextManager::ClearErrors () const
{
  impl->ClearErrors ();
}

/*
    Оповещение о необходимости ребинда уровня / очистка флагов ребиндинга / проверка необходимости ребинда уровней
*/

void ContextManager::StageRebindNotify (Stage stage)
{
  impl->StageRebindNotify (stage);
}

void ContextManager::ResetRebindNotifications ()
{
  impl->ResetRebindNotifications ();
}

bool ContextManager::NeedStageRebind (Stage stage) const
{
  return impl->NeedStageRebind (stage);
}

namespace render
{

namespace low_level
{

namespace opengl
{

/*
    Получение таблицы точек входа для текущего контекста
*/

const GlEntries* get_gl_entries ()
{
  return &ContextImpl::GetCurrentContext ()->GetGlEntries ();
}

}

}

}
