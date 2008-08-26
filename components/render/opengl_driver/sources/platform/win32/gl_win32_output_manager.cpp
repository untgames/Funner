#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::windows;

namespace
{

/*
     онстанты
*/

const size_t DEFAULT_OUTPUT_ARRAY_RESERVE = 8; //резервируемое количество устройств вывода

/*
    ќсновные определени€ типов
*/

typedef xtl::com_ptr<Output>   OutputPtr;
typedef stl::vector<OutputPtr> OutputArray;

/*
    ѕостроение списка устройств вывода
*/

struct OutputArrayBuilder
{
  OutputArray& outputs; //массив устройств вывода

  OutputArrayBuilder (OutputArray& in_outputs) : outputs (in_outputs)
  {
    HDC dc = GetDC (0);
    
    EnumDisplayMonitors (dc, 0, &OutputArrayBuilder::MonitorEnumProc, reinterpret_cast<DWORD> (this));
    
    ReleaseDC (0, dc);
  }

  static BOOL CALLBACK MonitorEnumProc (HMONITOR monitor, HDC dc, LPRECT, LPARAM data)
  {
    OutputArrayBuilder* builder = reinterpret_cast<OutputArrayBuilder*> (data);

    if (!builder || !dc)
      return FALSE;

    MONITORINFOEX info;

    memset (&info, 0, sizeof (info));

    info.cbSize = sizeof (info);

    if (!GetMonitorInfo (monitor, &info))
      return FALSE;

    DISPLAY_DEVICE device_info;

    memset (&device_info, 0, sizeof device_info);

    device_info.cb = sizeof device_info;
      
    for (size_t i=0; EnumDisplayDevices (info.szDevice, i, &device_info, 0); i++)
      builder->outputs.push_back (OutputPtr (new Output (device_info), false));

    return TRUE;
  }
};

}

/*
    ќписание реализации OutputManager
*/

struct OutputManager::Impl: public xtl::reference_counter
{
  OutputArray  outputs;  //устройства вывода  
  static Impl* instance; //глобальный экземпл€р реализации менеджера устройств вывода
  
  Impl ()
  {
    outputs.reserve (DEFAULT_OUTPUT_ARRAY_RESERVE);  

      //построение массива устройств вывода

    OutputArrayBuilder builder (outputs);

    instance = this;
  }
  
  ~Impl ()
  {
    instance = 0;
  }
};

OutputManager::Impl* OutputManager::Impl::instance = 0;

/*
     онструктор / деструктор
*/

OutputManager::OutputManager ()
  : impl (OutputManager::Impl::instance)
{
  if (!impl) impl = new Impl;
  else       addref (impl);
}

OutputManager::~OutputManager ()
{
  release (impl);
}

/*
    ѕеречисление доступных устройств вывода
*/

size_t OutputManager::GetOutputsCount () const
{
  return impl->outputs.size ();
}

Output* OutputManager::GetOutput (size_t index) const
{
  if (index >= impl->outputs.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::windows::OutputManager::GetOutput", "index", index, impl->outputs.size ());
    
  return get_pointer (impl->outputs [index]);
}

/*
    ѕоиск устройства вывода, перекрытие которого с окном максимально
*/

namespace
{

struct OutputSearcher
{
  OutputPtr    containing_output;        //устройств вывода с максимальной областью перекрыти€
  size_t       intersection_region_size; //размер области перекрыти€
  OutputArray& outputs;
  
  OutputSearcher (HDC dc, OutputArray& in_outputs) : intersection_region_size (0), outputs (in_outputs)
  {
    EnumDisplayMonitors (dc, 0, &OutputSearcher::MonitorEnumProc, reinterpret_cast<DWORD> (this));
  }
  
  static BOOL CALLBACK MonitorEnumProc (HMONITOR monitor, HDC dc, LPRECT intersection_region, LPARAM data)
  {
    OutputSearcher* search_data = reinterpret_cast<OutputSearcher*> (data);

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
};

}

Output* OutputManager::FindContainingOutput (HWND window) const
{
  if (!window)
    return 0;

  HDC dc = GetWindowDC (window);

  if (!dc)
    return 0;

  OutputSearcher searcher (dc, impl->outputs);    

  ReleaseDC (window, dc);

  return get_pointer (searcher.containing_output);
}
