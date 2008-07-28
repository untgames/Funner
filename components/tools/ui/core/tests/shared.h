#ifndef TOOLS_UI_CORE_TESTS_HEADER
#define TOOLS_UI_CORE_TESTS_HEADER

#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/reference_counter.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>

#include <tools/ui/main_window.h>
#include <tools/ui/custom_window_system.h>

using namespace tools::ui;

typedef xtl::com_ptr<IApplicationServer> ApplicationServerPtr;

//пользовательский сервер приложени€
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

    void ExecuteCommand (const char* command) {}
    void SetProperty    (const char* name, const stl::string& value) {} 
    void GetProperty    (const char* name, stl::string& value) { value = ""; }
    bool IsPropertyPresent  (const char* name) { return false; }
    bool IsPropertyReadOnly (const char* name) { return false; }
    void RegisterPropertyListener       (const char* name_wc_mask, IPropertyListener* listener) {}
    void UnregisterPropertyListener     (const char* name_wc_mask, IPropertyListener* listener) {}
    void UnregisterAllPropertyListeners (const char* name_wc_mask) {}
    void UnregisterAllPropertyListeners () {}

    void AddRef  () { addref (this); }
    void Release () { release (this); }
};

//пользовательска€ оконна€ система
class MyWindowSystem: public ICustomWindowSystem, public xtl::reference_counter
{
  public:
    MyWindowSystem (IApplicationServer* in_server) : server (in_server)
    {
      printf ("MyWindowSystem::MyWindowSystem\n");
    }
    
    ~MyWindowSystem ()
    {
      printf ("MyWindowSystem::~MyWindowSystem\n");
    }
    
///¬ыполнение команды на стороне оконной системы
    void ExecuteCommand (const char* command)
    {
      printf ("MyWindowSystem::ExecuteCommand(%s)\n", command);
    }

///ѕодсчЄт ссылок
    void AddRef () { addref (this); }
    void Release () { release (this); }    
    
  private:
    ApplicationServerPtr server;
};

#endif
