#include "shared.h"

using namespace render;

//TODO: CacheHolder & linked layouts

namespace
{

/*
    Константы
*/

const size_t RESERVED_LAYOUTS_COUNT = 4; //резервируемое количество связанных лэйаутов

}

/*
    Описание реализации расположения параметра
*/

typedef stl::vector<render::low_level::ProgramParameter> ParameterArray;
typedef stl::vector<ProgramParametersLayoutPtr>          LayoutArray;

struct ProgramParametersLayout::Impl: public CacheHolder
{
  render::low_level::ProgramParametersLayoutDesc  desc;                              //описание параметров
  ParameterArray                                  parameters;                        //параметры
  common::PropertyLayout                          slots [ProgramParametersSlot_Num]; //слоты параметров
  LayoutArray                                     layouts;                           //связанные расположения свойств
  size_t                                          hash;                              //хэш блока параметров
  bool                                            need_rebuild;                      //необходимо ли обновление возвращаемых структур
  LowLevelDevicePtr                               device;                            //устройство отрисовки
  LowLevelProgramParametersLayoutPtr              device_layout;                     //объект расположения параметров устройства отрисовки
  
///Конструктор
  Impl (const LowLevelDevicePtr& in_device)
    : need_rebuild (true)
    , device (in_device)
  {
    if (!device)
      throw xtl::make_null_argument_exception ("render::ProgramParametersLayout::Impl::Impl", "device");

    layouts.reserve (RESERVED_LAYOUTS_COUNT);
  }

///Построение списка параметров
  void Rebuild ()
  {
    static render::low_level::ProgramParameter default_parameter;
    
    if (!need_rebuild)
      return;          
      
      //резервирование памяти для хранения параметров
    
    size_t parameters_count = 0;
    
    for (LayoutArray::iterator iter=layouts.begin (), end=layouts.end (); iter!=end; ++iter)
      parameters_count += (*iter)->Parameters ().parameters_count;
      
    for (int i=0; i<ProgramParametersSlot_Num; i++)
      parameters_count += slots [i].Size ();
      
    ParameterArray new_parameters;

    new_parameters.reserve (parameters_count);
    
      //построение списка      
      
    if (parameters_count)
    {
      for (LayoutArray::iterator iter=layouts.begin (), end=layouts.end (); iter!=end; ++iter)
      {
        const render::low_level::ProgramParametersLayoutDesc& layout_desc = (*iter)->Parameters ();

        new_parameters.insert (new_parameters.end (), layout_desc.parameters, layout_desc.parameters + layout_desc.parameters_count);
      }            

      for (int i=0; i<ProgramParametersSlot_Num; i++)
      {
        const common::PropertyLayout& layout = slots [i];

        if (!layout.Size ())
          continue;

        const common::PropertyDesc* property = layout.Properties ();
        size_t                      count    = layout.Size ();
        
        for (size_t j=0;count--; property++, j++)
        {
          render::low_level::ProgramParameter parameter;

          memset (&parameter, 0, sizeof (parameter));

          parameter.name   = property->name;
          parameter.slot   = i;
          parameter.count  = property->elements_count;
          parameter.offset = property->offset;
          
          switch (property->type)
          {            
            case common::PropertyType_String:
              continue; //строки не поддерживаются рендером
            case common::PropertyType_Integer:
              parameter.type = render::low_level::ProgramParameterType_Int;
              break;
            case common::PropertyType_Float:
              parameter.type = render::low_level::ProgramParameterType_Float;
              break;            
            case common::PropertyType_Vector:
              parameter.type = render::low_level::ProgramParameterType_Float4;
              break;            
            case common::PropertyType_Matrix:
              parameter.type = render::low_level::ProgramParameterType_Float4x4;
              break;            
            default:
              throw xtl::format_operation_exception ("render::ProgramParametersLayout", "Unexpected property[%u] type %s at parsing", j, get_name (property->type));
          }

          new_parameters.push_back (parameter);          
        }
      }

      memset (&desc, 0, sizeof (desc));

      desc.parameters_count = new_parameters.size ();
      desc.parameters       = new_parameters.empty () ? &default_parameter : &parameters [0];

        //расчёт хэша

      hash = common::crc32 (&parameters [0], new_parameters.size () * sizeof (render::low_level::ProgramParameter));
    }
    else
    {
        //обработка частного случая отсутствия параметров      

      desc.parameters_count = 0;
      desc.parameters       = &default_parameter;
      hash                  = 0xffffffff;
    }
    
    parameters.swap (new_parameters);
        
    need_rebuild  = false;
    device_layout = LowLevelProgramParametersLayoutPtr ();
  }
  
///Работа с кэшем
  void ResetCacheCore ()
  {
  }
  
  void UpdateCacheCore ()
  {
    try
    {
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::ProgramParametersLayout::Impl::UpdateCacheCore");
      throw;
    }
  }  
};

/*
    Конструктор / деструктор
*/

ProgramParametersLayout::ProgramParametersLayout (const LowLevelDevicePtr& device)
  : impl (new Impl (device))
{
}

ProgramParametersLayout::~ProgramParametersLayout ()
{
}

/*
    Перечисление параметров
*/

const render::low_level::ProgramParametersLayoutDesc& ProgramParametersLayout::Parameters ()
{
  if (impl->need_rebuild)
    impl->Rebuild ();
    
  return impl->desc;
}

/*
    Хэш
*/

size_t ProgramParametersLayout::Hash () const
{
  if (impl->need_rebuild)
    impl->Rebuild ();
    
  return impl->hash;
}

/*
    Установка параметров в соответствующие слоты
*/

void ProgramParametersLayout::SetSlot (ProgramParametersSlot slot, const common::PropertyLayout& layout)
{
  if (slot < 0 || slot >= ProgramParametersSlot_Num)
    throw xtl::make_range_exception ("render::ProgramParametersLayout::ResetSlot", "slot", slot, 0, ProgramParametersSlot_Num);
    
  impl->slots [slot] = layout;
  
  impl->slots [slot].Capture ();
  
  impl->need_rebuild = true;
}

void ProgramParametersLayout::ResetSlot (ProgramParametersSlot slot)
{
  if (slot < 0 || slot >= ProgramParametersSlot_Num)
    throw xtl::make_range_exception ("render::ProgramParametersLayout::ResetSlot", "slot", slot, 0, ProgramParametersSlot_Num);
    
  impl->slots [slot].Clear ();
  
  impl->need_rebuild = true;
}

void ProgramParametersLayout::ResetAllSlots ()
{
  for (int i=0; i<ProgramParametersSlot_Num; i++)
    impl->slots [i].Clear ();

  impl->need_rebuild = true;
}

/*
    Связывание параметров
*/

void ProgramParametersLayout::Attach (const ProgramParametersLayout& layout)
{
  impl->layouts.push_back (&const_cast<ProgramParametersLayout&> (layout));
  
  impl->need_rebuild = true;
}

void ProgramParametersLayout::Detach (const ProgramParametersLayout& layout)
{
  impl->layouts.erase (stl::remove (impl->layouts.begin (), impl->layouts.end (), &const_cast<ProgramParametersLayout&> (layout)), impl->layouts.end ());

  impl->need_rebuild = true;
}

void ProgramParametersLayout::DetachAll ()
{
  impl->layouts.clear ();
  
  impl->need_rebuild = true;
}

/*
    Получение объекта расположения параметров низкоуровневого устройства отрисовки
*/

LowLevelProgramParametersLayoutPtr& ProgramParametersLayout::DeviceLayout ()
{
  try
  {
    if (impl->need_rebuild)
      impl->Rebuild ();
    
    if (impl->device_layout)
      return impl->device_layout;

    impl->device_layout = LowLevelProgramParametersLayoutPtr (impl->device->CreateProgramParametersLayout (impl->desc), false);                    
      
    return impl->device_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersLayout::DeviceLayout");
    throw;
  }      
}
