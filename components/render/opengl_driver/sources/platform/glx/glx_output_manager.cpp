#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_OUTPUT_ARRAY_RESERVE = 8; //резервируемое количество устройств вывода

/*
    Основные определения типов
*/

typedef xtl::com_ptr<Output>   OutputPtr;
typedef stl::vector<OutputPtr> OutputArray;

}

/*
    Описание реализации OutputManager
*/

struct OutputManager::Impl: public xtl::reference_counter
{
  OutputArray  outputs;  //устройства вывода  
  static Impl* instance; //глобальный экземпляр реализации менеджера устройств вывода
  
  Impl ()
  {
    //соединение с дисплеем
    
    Display* display = (Display*) syslib::x11::DisplayManager::DisplayHandle ();
    
    size_t screens_count = 0;
    
    {
      DisplayLock lock (display);

      screens_count = XScreenCount (display);
    }
    
    outputs.reserve (screens_count);
    
    //построение массива устройств вывода
    
    for (size_t screen_number=0; screen_number<screens_count; screen_number++)
      outputs.push_back (Output::Pointer (new Output (display, screen_number), false));
  }
  
  ~Impl ()
  {
  }
};

/*
    Конструктор / деструктор
*/

OutputManager::OutputManager ()
{
  impl = new Impl ();
}

OutputManager::~OutputManager ()
{
}

/*
    Перечисление доступных устройств вывода
*/

size_t OutputManager::GetOutputsCount () const
{
  return impl->outputs.size ();
}

Output* OutputManager::GetOutput (size_t index) const
{
  if (index >= impl->outputs.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::glx::Adapter::GetOutput", "index", index, impl->outputs.size ());

  return get_pointer (impl->outputs [index]);
}

/*
    Поиск устройства вывода, перекрытие которого с окном максимально
*/

Output* OutputManager::FindContainingOutput (Window window) const
{
  if (window < 0)
    return 0;

  //соединение с дисплеем
  
  Display* display = (Display*) syslib::x11::DisplayManager::DisplayHandle ();
  
  {
    DisplayLock lock (display);

    XWindowAttributes xwa;
    
    if (XGetWindowAttributes (display, window, &xwa) < Success);
    
    int screen_number = XScreenNumberOfScreen (xwa.screen);
    
    for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter != end; ++iter)
    {
      OutputPtr output = *iter;
      
      if (screen_number == output->GetScreenNumber ())
        return output;
    }
    
    return 0;
  }
}
