#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

const size_t DEFAULT_OUTPUT_ARRAY_RESERVE = 8; //резервируемое количество устройств вывода

/*
    Описание реализации OutputManager
*/

typedef xtl::com_ptr<Output>   OutputPtr;
typedef stl::vector<OutputPtr> OutputArray;

struct OutputManager::Impl
{
  OutputArray outputs; //устройства вывода
};

/*
    Конструктор / деструктор
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
    Перечисление доступных устройств вывода
*/

size_t OutputManager::GetOutputsCount () const
{
  return impl->outputs.size ();
}

IOutput* OutputManager::GetOutput (size_t index) const
{
  if (index >= impl->outputs.size ())
    RaiseOutOfRange ("render::low_level::opengl::OutputManager::GetOutput", "index", index, impl->outputs.size ());
    
  return get_pointer (impl->outputs [index]);
}

/*
    Поиск устройства вывода, перекрытие которого с окном максимально
*/

namespace
{

struct OutputSearchData
{
  OutputPtr    containing_output;        //устройств вывода с максимальной областью перекрытия
  size_t       intersection_region_size; //размер области перекрытия
  OutputArray& outputs;
  
  OutputSearchData (OutputArray& in_outputs) : intersection_region_size (0), outputs (in_outputs) {}
};

BOOL CALLBACK MonitorEnumProc (HMONITOR monitor, HDC dc, LPRECT intersection_region, LPARAM data)
{
  OutputSearchData* search_data = reinterpret_cast<OutputSearchData*> (data);

  if (!search_data || !dc)
    return FALSE;

  MONITORINFOEX info;

  memset (&info, 0, sizeof (info));

  info.cbSize = sizeof (info);

  if (!GetMonitorInfo (monitor, &info))
    return FALSE;

  size_t intersection_region_size = (intersection_region->right - intersection_region->left) *
                                    (intersection_region->bottom - intersection_region->top);
                                    
  if (intersection_region_size > search_data->intersection_region_size)
  {
    for (OutputArray::iterator iter=search_data->outputs.begin (), end=search_data->outputs.end (); iter != end; ++iter)
    {
      OutputPtr output = *iter;      
      
      if (!strcmp (output->GetWinName (), info.szDevice))
      {
        search_data->intersection_region_size = intersection_region_size;
        search_data->containing_output        = output;
        
        break;
      }
    }
  }
  
  return TRUE;
}

}

IOutput* OutputManager::FindContainingOutput (void* window_handle) const
{
  HWND window = (HWND)window_handle;

  if (!window)
    return 0;

  HDC dc = GetWindowDC (window);

  if (!dc)
    return 0;
    
  OutputSearchData search_data (impl->outputs);
    
  if (!EnumDisplayMonitors (dc, 0, &MonitorEnumProc, reinterpret_cast<DWORD> (&search_data)))
  {
    ReleaseDC (window, dc);
    return 0;
  }

  ReleaseDC (window, dc);  

  return get_pointer (search_data.containing_output);
}
