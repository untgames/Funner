#include "shared.h"

using namespace engine;
using namespace engine::dot_net;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;

namespace Funner
{

/*
    Окно движка
*/

public ref class Window: public Form
{
  public:
    ///Конструктор
    Window (IEngine* in_engine, const char* name)
      : engine (in_engine)
      , window (0)
    {
      if (!engine)
        throw gcnew ArgumentException ("Null engine passed to constructor of Funner.Window class");
        
      if (!name)
        throw gcnew ArgumentException ("Null name passed to constructor of Funner.Window class");

      window = engine->CreateWindow (name);
    }

    ///Деструктор
    ~Window ()
    {
      delete window;
    }
  
  private:
    IEngine* engine; //указатель на движок
    IWindow* window; //указатель на окно движка
};

/*
    Класс движка
*/

public ref class Engine
{
  public:
///Создание интерфейса поддержки внешних окон
    static Window^ CreateWindow (String^ name)
    {
      return gcnew Window (GetEngine (), AutoString (name).Data ());
    }

  private:
///Запрос native API движка
    [DllImport ("funner.dll")]
    static IEngine* FunnerInit ();
    
///Получение движка
    static IEngine* GetEngine ()
    {
      if (engine)
        return engine;
      
      engine = FunnerInit ();
      
      if (!engine)
        throw gcnew InvalidOperationException ("Funner native engine has bad entries");
        
      return engine;
    }

  private:
    static IEngine* engine = 0; //указатель на объект движка
};

}
