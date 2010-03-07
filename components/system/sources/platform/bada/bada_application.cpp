#include "shared.h"

using namespace syslib;
using namespace syslib::bada;

namespace
{

/*
    Константы
*/

//const char*    LOG_NAME           = "system.bada.application"; //поток протоколирования
const wchar_t* APPLICATION_NAME   = DEFAULT_APP_NAME;          //имя приложения
const wchar_t* APPLICATION_ID     = DEFAULT_APP_ID;            //идентификатор приложения
const wchar_t* APPLICATION_SECRET = DEFAULT_APP_SECRET;        //секретный код приложения

/*
    Описание реализации bada-приложения
*/

struct ApplicationDelegate: public IApplicationDelegate
{
  IApplicationListener* listener;
  bool                  idle_state;
  
///Конструктор
  ApplicationDelegate ()
  {
    listener   = 0;
    idle_state = false;
  }

///Запуск цикла обработки сообщений
  void Run ()
  {
    Osp::Base::Collection::ArrayList* args = new Osp::Base::Collection::ArrayList;

    args->Construct ();    
    
    int          argc = get_osp_main_argc ();
    const char** argv = get_osp_main_argv ();
    
    for (int i= 0; i<argc; i++)
      args->Add (*(new Osp::Base::String (argv [i])));

    result res = Osp::App::Application::Execute (DefaultApplication::CreateInstance, args);
    
    if (IsFailed (res))
    {
      args->RemoveAll (true);

      delete args;

      raise ("syslib::bada::ApplicationImpl::Run", res);
    }

    args->RemoveAll(true);

    delete args;    
  }

///Выход из приложения
  void Exit (int code)
  {
  }
  
///Установка необходимости вызова событий idle
  void SetIdleState (bool state)
  {
  }

///Установка слушателя событий приложения
  void SetListener (IApplicationListener* in_listener)
  {
    listener = in_listener;
  }
  
///Подсчёт ссылок
  void AddRef ()
  {
  }
  
  void Release ()
  {
  }  
  
///Оповещение о старте приложения  
  bool OnAppInitializing (Osp::App::Application& app, Osp::App::AppRegistry& app_registry)
  {
    if (listener)
      listener->OnInitialized ();

//    app.GetAppFrame ()->GetFrame ()->AddControl (*form);

    return true;    
  }
  
///Оповещение о завершении приложения
  bool OnAppTerminating (Osp::App::Application& app, Osp::App::AppRegistry& app_registry, bool forced_termination)
  {
    return true;
  }
  
///Оповещение о перемещении фрейма приложения на передний план
  void OnForeground (Osp::App::Application& app)
  {
  }
  
///Оповещение о перемещении фрейма приложения на задний план
  void OnBackground (Osp::App::Application& app)
  {
  }
  
///Оповещение о нехватке памяти
  void OnLowMemory (Osp::App::Application& app)
  {
  }
  
///Оповещение о низком уровне батареи
  void OnBatteryLevelChanged (Osp::App::Application& app, Osp::System::BatteryLevel battery_level)
  {
  }
};

//текущее приложение
DefaultApplication* current_application = 0;

}

typedef common::Singleton<ApplicationDelegate> ApplicationSingleton;

/*
    Bada-приложение
*/

///Конструктор
DefaultApplication::DefaultApplication ()
{
  if (current_application)
    throw xtl::format_operation_exception ("syslib::bada::DefaultApplication::DefaultApplication", "Application has already created");
    
  current_application = this;
}
    
///Деструктор
DefaultApplication::~DefaultApplication ()
{
  current_application = 0;
}
  
///Создание экземпляра приложения
Osp::App::Application* DefaultApplication::CreateInstance ()
{
  if (current_application)
    return 0;

  return new DefaultApplication;
}

///Имя приложения    
Osp::Base::String DefaultApplication::GetAppName(void) const
{
  static Osp::Base::String app_name (APPLICATION_NAME);

  return app_name;
}

///Оповещение о старте приложения
bool DefaultApplication::OnAppInitializing (Osp::App::AppRegistry& app_registry)
{
  return ApplicationSingleton::Instance ()->OnAppInitializing (*this, app_registry);
}

///Оповещение о завершении приложения
bool DefaultApplication::OnAppTerminating (Osp::App::AppRegistry& app_registry, bool forced_termination)
{
  return ApplicationSingleton::Instance ()->OnAppTerminating (*this, app_registry, forced_termination);
}

///Оповещение о перемещении фрейма приложения на передний план
void DefaultApplication::OnForeground ()
{
  ApplicationSingleton::Instance ()->OnForeground (*this);
}

///Оповещение о перемещении фрейма приложения на задний план
void DefaultApplication::OnBackground ()
{
  ApplicationSingleton::Instance ()->OnBackground (*this);
}

///Оповещение о нехватке памяти
void DefaultApplication::OnLowMemory ()
{
  ApplicationSingleton::Instance ()->OnLowMemory (*this);
}

///Оповещение о низком уровне батареи
void DefaultApplication::OnBatteryLevelChanged (Osp::System::BatteryLevel battery_level)
{
  ApplicationSingleton::Instance ()->OnBatteryLevelChanged (*this, battery_level);
}

///Идентификатор приложения
Osp::App::AppId DefaultApplication::GetAppId () const
{
  static Osp::App::AppId app_id (APPLICATION_ID);

  return app_id;      
}

///Секретный код приложения
Osp::App::AppSecret DefaultApplication::GetAppSecret () const
{
  static Osp::App::AppSecret app_secret (APPLICATION_SECRET);

  return app_secret;
}

/*
    Создание объекта приложения
*/

IApplicationDelegate* Platform::CreateDefaultApplicationDelegate ()
{
  return &*ApplicationSingleton::Instance ();
}

/*
    Получение экземпляра текущего приложения
*/

namespace syslib
{

namespace bada
{

Osp::App::Application& get_application ()
{
  if (!current_application)
    throw xtl::format_operation_exception ("syslib::bada::get_application", "Application has not initialized");
    
  return *current_application;
}

}

}
