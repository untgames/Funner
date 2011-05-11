#include "shared.h"

using namespace render;
using namespace render::low_level;

//TODO: flush cache of composite layouts after N frames

/*
    Описание реализации менеджера параметров программ шэйдинга
*/

typedef stl::hash_multimap<size_t, ProgramParametersLayout*> LayoutMap;
typedef stl::hash_map<size_t, ProgramParametersLayoutPtr>    CompositeLayoutMap;

struct ProgramParametersManager::Impl: public xtl::trackable
{
  LowLevelDevicePtr   device;            //устройство визуализации
  LayoutMap           layouts;           //лэйауты параметров различной конфигурации
  CompositeLayoutMap  composite_layouts; //составные лэйауты параметров различной конфигурации

///Конструктор
  Impl (const LowLevelDevicePtr& in_device) : device (in_device) {}

///Деструктор
  ~Impl ()
  {
    layouts.clear ();
    composite_layouts.clear ();
  }

///Удаление лэйаута
  void RemoveLayout (size_t hash, ProgramParametersLayout* layout)
  {
    stl::pair<LayoutMap::iterator, LayoutMap::iterator> range = layouts.equal_range (hash);
    
    for (; range.first!=range.second; ++range.first)
      if (range.first->second == layout)
      {
        layouts.erase (range.first);        

        return;        
      }
  }  
};

/*
    Конструктор / деструктор
*/

ProgramParametersManager::ProgramParametersManager (const LowLevelDevicePtr& device)
  : impl (new Impl (device))
{
}

ProgramParametersManager::~ProgramParametersManager ()
{
}

/*
    Получение объекта параметров
*/

ProgramParametersLayoutPtr ProgramParametersManager::GetParameters (ProgramParametersSlot slot, const common::PropertyLayout& layout)
{
  try
  {        
    stl::pair<LayoutMap::iterator, LayoutMap::iterator> range = impl->layouts.equal_range (layout.Hash ());    
    
    for (size_t parameters_count=layout.Size (); range.first!=range.second; ++range.first)
      if (range.first->second->Parameters ().parameters_count == parameters_count)
        return range.first->second;

    ProgramParametersLayoutPtr result_layout (new ProgramParametersLayout (impl->device), false);

    result_layout->SetSlot (slot, layout);

    result_layout->connect_tracker (xtl::bind (&Impl::RemoveLayout, &*impl, layout.Hash (), &*result_layout), *impl);
    
    impl->layouts.insert_pair (layout.Hash (), result_layout.get ());

    return result_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersManager::GetParameters(const common::PropertyLayout&)");
    throw;
  }
}

ProgramParametersLayoutPtr ProgramParametersManager::GetParameters
 (const ProgramParametersLayout* layout1,
  const ProgramParametersLayout* layout2,
  const ProgramParametersLayout* layout3)
{
  try
  {
    size_t hash = 0xffffffff;

    if (layout3) hash = common::crc32 (&layout3, sizeof (layout3), hash);
    if (layout2) hash = common::crc32 (&layout2, sizeof (layout2), hash);
    if (layout1) hash = common::crc32 (&layout1, sizeof (layout1), hash);

    CompositeLayoutMap::iterator iter = impl->composite_layouts.find (hash);
    
    if (iter != impl->composite_layouts.end ())
      return iter->second;

    ProgramParametersLayoutPtr result_layout (new ProgramParametersLayout (impl->device), false);

    if (layout1) result_layout->Attach (*layout1);
    if (layout2) result_layout->Attach (*layout2);
    if (layout3) result_layout->Attach (*layout3);

    impl->composite_layouts.insert_pair (hash, result_layout);

    return result_layout;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersManager::GetParameters(const common::ProgramParametersLayout*,const ProgramParametersLayout*,const ProgramParametersLayout*)");
    throw;
  }
}
