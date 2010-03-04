#include "shared.h"

using namespace syslib;

namespace
{

/*
    Константы
*/

const wchar_t* APPLICATION_NAME   = "TestApplication";                   //имя приложения
const wchar_t* APPLICATION_ID     = L"aospd00017";                       //идентификатор приложения
const wchar_t* APPLICATION_SECRET = L"AF60E5CB8B43BC9650412311E01BAB6C"; //секретный код приложения

/*
    Bada-приложение
*/

class BasicApp: public Osp::App::Application
{
  public:
///Конструктор
    BasicApp ()
    {
    }
    
///Деструктор
    ~BasicApp ()
    {
    }
  
///Создание экземпляра приложения
    static Osp::App::Application* CreateInstance ()
    {
    }

///Имя приложения    
    Osp::Base::String GetAppName(void) const
    {
      static String app_name (APPLICATION_NAME);

      return app_name;
    }
    
///Оповещение о старте приложения
    bool OnAppInitializing (Osp::App::AppRegistry& app_registry)
    {
    }

///Оповещение о завершении приложения
    bool OnAppTerminating (Osp::App::AppRegistry& app_registry, bool forcedTermination)
    {
    }

///Оповещение о перемещении фрейма приложения на передний план
    void OnForeground ()
    {
    }

///Оповещение о перемещении фрейма приложения на задний план
    void OnBackground ()
    {
    }

///Оповещение о нехватке памяти
    void OnLowMemory ()
    {
    }

///Оповещение о низком уровне батареи
    void OnBatteryLevelChanged (Osp::System::BatteryLevel batteryLevel)
    {
    }

  protected:
///Идентификатор приложения
    Osp::App::AppId GetAppId () const
    {
      static AppId app_id (APPLICATION_ID);

      return app_id;      
    }

///Секретный код приложения
    Osp::App::AppSecret GetAppSecret () const
    {
      static AppSecret app_secret (APPLICATION_SECRET);

      return app_secret;
    }
};

}

/*
    Работа с очередью сообщений
*/

bool Platform::IsMessageQueueEmpty ()
{
  throw xtl::format_not_supported_exception ("syslib::BadaPlatform::IsMessageQueueEmpty", "Message queue doesn't supported");
}

void Platform::DoNextEvent ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::DoNextEvent", "Message queue doesn't supported");
}

void Platform::WaitMessage ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::WaitMessage", "Message queue doesn't supported");
}

void Platform::UpdateMessageQueue ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::UpdateMessageQueue");
}

/*
    Запуск приложения
*/

void Platform::RunLoop (IRunLoopContext* context)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::RunLoop");
/*  if (!context)
    throw xtl::make_null_argument_exception ("syslib::BadaPlatform::RunLoop", "context");

  context->OnEnterRunLoop ();

  context->DoCustomRunLoop ();*/
}
