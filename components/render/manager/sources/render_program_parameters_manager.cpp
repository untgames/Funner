#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    Описание реализации менеджера параметров программ шэйдинга
*/

typedef stl::hash_multimap<size_t, ProgramParametersLayout*> LayoutMap;
typedef stl::hash_map<size_t, ProgramParametersLayout*>      PairLayoutMap;

struct ProgramParametersManager::Impl: public xtl::trackable
{
  LowLevelDevicePtr device;       //устройство визуализации
  LayoutMap         layouts;      //лэйауты параметров различной конфигурации
  PairLayoutMap     pair_layouts; //парные лэйауты параметров различной конфигурации  

///Конструктор
  Impl (const LowLevelDevicePtr& in_device) : device (in_device) {}

///Деструктор
  ~Impl ()
  {
    layouts.clear ();
    pair_layouts.clear ();
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
  
  void RemovePairLayout (size_t hash)
  {
    pair_layouts.erase (hash);
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

ProgramParametersLayoutPtr ProgramParametersManager::GetParameters (const ProgramParametersLayout& layout1, const ProgramParametersLayout& layout2)
{
  try
  {
    const ProgramParametersLayout *layout1_ptr = &layout1, *layout2_ptr = &layout2;

    size_t hash = common::crc32 (&layout1_ptr, sizeof (layout1_ptr), common::crc32 (&layout2_ptr, sizeof (layout2_ptr)));
    
    PairLayoutMap::iterator iter = impl->pair_layouts.find (hash);
    
    if (iter != impl->pair_layouts.end ())
      return iter->second;
    
    ProgramParametersLayoutPtr result_layout (new ProgramParametersLayout (impl->device), false);

    result_layout->Attach (layout1);
    result_layout->Attach (layout2);

    result_layout->connect_tracker (xtl::bind (&Impl::RemovePairLayout, &*impl, hash), *impl);
    
    impl->pair_layouts.insert_pair (hash, result_layout.get ());

    return result_layout;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersManager::GetParameters(const common::ProgramParametersLayout&,const ProgramParametersLayout&)");
    throw;
  }
}
