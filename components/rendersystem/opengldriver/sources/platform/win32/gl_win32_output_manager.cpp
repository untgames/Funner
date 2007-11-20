#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

const size_t DEFAULT_OUTPUT_ARRAY_RESERVE = 8; //резервируемое количество устройств вывода

/*
    ќписание реализации OutputManager
*/

typedef xtl::com_ptr<IOutput>  OutputPtr;
typedef stl::vector<OutputPtr> OutputArray;

struct OutputManager::Impl
{
  OutputArray outputs; //устройства вывода
};

/*
     онструктор / деструктор
*/

OutputManager::OutputManager ()
  : impl (new Impl)
{
  impl->outputs.reserve (DEFAULT_OUTPUT_ARRAY_RESERVE);  

    //перечисление доступных устройств

  for (int device_number=0; ; device_number++)
  {
    DISPLAY_DEVICE device_info;

    memset (&device_info, 0, sizeof (device_info));

    device_info.cb = sizeof (device_info);

    if (!EnumDisplayDevices (0, device_number, &device_info, 0))
      break;
      
    impl->outputs.push_back (OutputPtr (new Output (device_info), false));
  }
}

OutputManager::~OutputManager ()
{
}

/*
    ѕеречисление доступных устройств вывода
*/

size_t OutputManager::GetOutputsCount ()
{
  return impl->outputs.size ();
}

IOutput* OutputManager::GetOutput (size_t index)
{
  if (index >= impl->outputs.size ())
    RaiseOutOfRange ("render::low_level::opengl::OutputManager::GetOutput", "index", index, impl->outputs.size ());
    
  return get_pointer (impl->outputs [index]);
}
