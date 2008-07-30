#ifndef TOOLS_UI_WINDOWS_FORMS_TESTS_SHARED_HEADER
#define TOOLS_UI_WINDOWS_FORMS_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <syslib/application.h>

#include <tools/ui/main_window.h>
#include <tools/ui/custom_window_system.h>

using namespace tools::ui;

//тестовый сервер приложения
class MyApplicationServer: public IApplicationServer, public xtl::reference_counter
{
  public:
    MyApplicationServer ()
    {
      printf ("MyApplicationServer::MyApplicationServer\n");
    }
    
    ~MyApplicationServer ()
    {
      printf ("MyApplicationServer::~MyApplicationServer\n");
    }

    void ExecuteCommand (const char* command)
    {
      if (!xtl::xstrcmp (command, "close"))
        syslib::Application::Exit (0);
      
      printf ("MyApplicationServer::ExecuteCommand(%s)\n", command);
    }

    void SetProperty (const char* name, const stl::string& value)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::SetProperty");
    } 
    
    void GetProperty (const char* name, stl::string& value)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::GetProperty");
    }
    
    bool IsPropertyPresent (const char* name)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::IsPropertyPresent");
    }

    bool IsPropertyReadOnly (const char* name)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::IsPropertyPresent");
    }    

    void RegisterPropertyListener (const char* name_wc_mask, IPropertyListener* listener)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::RegisterPropertyListener");
    }

    void UnregisterPropertyListener (const char* name_wc_mask, IPropertyListener* listener)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterPropertyListener");
    }
    
    void UnregisterAllPropertyListeners (const char* name_wc_mask)
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterAllPropertyListeners");
    }
    
    void UnregisterAllPropertyListeners ()
    {
      throw xtl::make_not_implemented_exception ("MyApplicationServer::UnregisterAllPropertyListeners");    
    }

    void AddRef  () { addref (this); }
    void Release () { release (this); }
};

//класс тестового приложения
struct Test
{
  Test ()
  {
      //регистрация сервера приложения

    tools::ui::WindowSystemManager::RegisterApplicationServer ("MyApplicationServer",
      xtl::com_ptr<MyApplicationServer> (new MyApplicationServer, false).get ());
      
      //создание главного окна
      
    MainWindow ("MyApplicationServer", "WindowsForms").Swap (main_window);
  }

  MainWindow main_window;
};

#endif
