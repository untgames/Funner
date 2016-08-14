#include "shared.h"

using namespace syslib;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
===================================================================================================
    Устройство вывода
===================================================================================================
*/

/*
    Описание реализации устройства вывода
*/

struct Output::Impl
{
  Screen       screen;     //экран
  PropertyList properties; //свойства устройства
  
///Конструктор
  Impl (const Screen& in_screen)
    : screen (in_screen)
  {
      //заполнение свойств устройства вывода
    
    common::PropertyMap in_properties;
    
    screen.GetProperties (in_properties);
    
    for (size_t i=0, count=in_properties.Size (); i<count; i++)
    {
      stl::string value;
      
      in_properties.GetProperty (i, value);
      
      properties.AddProperty (in_properties.PropertyName (i), value.c_str ());
    }
  }
};

/*
    Конструктор / деструктор
*/

Output::Output (const syslib::Screen& screen)
{
  try
  {
    impl = new Impl (screen);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::Output");
    throw;
  }
}

Output::~Output ()
{
}

/*
    Платформо-зависимый дескриптор окна
*/

const void* Output::Handle ()
{
  try
  {
    return impl->screen.Handle ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::Handle");
    throw;
  }
}

/*
    Получение имени
*/

const char* Output::GetName ()
{
  try
  {
    return impl->screen.Name ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::GetName");
    throw;
  }
}

/*
    Получение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  try
  {
    return impl->screen.ModesCount ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::GetModesCount");
    throw;
  }
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  try
  {
    ScreenModeDesc desc;
    
    impl->screen.GetMode (mode_index, desc);
    
    mode_desc.width        = desc.width;
    mode_desc.height       = desc.height;
    mode_desc.color_bits   = desc.color_bits;
    mode_desc.refresh_rate = desc.refresh_rate;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::GetModeDesc");
    throw;
  }
}

/*
    Установка текущего видео-режима
*/

void Output::SetCurrentMode (const OutputModeDesc& desc)
{
  try
  {
    ScreenModeDesc mode_desc;
    
    mode_desc.width        = desc.width;
    mode_desc.height       = desc.height;
    mode_desc.color_bits   = desc.color_bits;
    mode_desc.refresh_rate = desc.refresh_rate;        
    
    impl->screen.SetCurrentMode (mode_desc);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::SetCurrentMode");
    throw;
  }
}

void Output::GetCurrentMode (OutputModeDesc& mode_desc)
{
  try
  {
    ScreenModeDesc desc;
    
    impl->screen.GetCurrentMode (desc);
    
    mode_desc.width        = desc.width;
    mode_desc.height       = desc.height;
    mode_desc.color_bits   = desc.color_bits;
    mode_desc.refresh_rate = desc.refresh_rate;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::GetCurrentMode");
    throw;
  }
}

void Output::RestoreDefaultMode ()
{
  try
  {
    impl->screen.RestoreDefaultMode ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::RestoreDefaultMode");
    throw;
  }
}

/*
    Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const render::low_level::Color3f table [256])
{
  try
  {
    impl->screen.SetGammaRamp ((syslib::Color3f*)table);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::SetGammaRamp");
    throw;
  }
}

void Output::GetGammaRamp (render::low_level::Color3f table [256])
{
  try
  {
    impl->screen.GetGammaRamp ((syslib::Color3f*)table);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Output::GetGammaRamp");
    throw;
  }
}

/*
    Список свойств устройства вывода
*/

IPropertyList* Output::GetProperties ()
{
  return &impl->properties;
}

/*
===================================================================================================
    Класс, инкапсулирующий работу с низкоуровневыми устройствами вывода
===================================================================================================
*/

/*
    Описание реализации менеджера устройств вывода
*/

typedef xtl::com_ptr<Output>   OutputPtr;
typedef stl::vector<OutputPtr> OutputArray;

struct OutputManager::Impl: public xtl::reference_counter
{
  OutputArray  outputs;  //устройства вывода
  static Impl* instance; //глобальный экземпляр реализации менеджера устройств вывода
  
///Конструктор
  Impl ()
  {
    size_t screens_count = ScreenManager::ScreensCount ();
    
    outputs.reserve (screens_count);
    
    for (size_t i=0; i<screens_count; i++)
      outputs.push_back (OutputPtr (new Output (ScreenManager::Screen (i)), false));
      
    instance = this;      
  }

///Деструктор
  ~Impl ()
  {
    instance = 0;
  }  
};

OutputManager::Impl* OutputManager::Impl::instance = 0;

/*
    Конструктор / деструктор
*/

OutputManager::OutputManager ()
{
  try
  {
    impl = OutputManager::Impl::instance;

    if (!impl) impl = new Impl;
    else       addref (impl);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::OutputManager::OutputManager");
    throw;
  }
}

OutputManager::~OutputManager ()
{
  release (impl);
}

/*
    Перечисление доступных устройств вывода
*/

size_t OutputManager::GetOutputsCount () const
{
  try
  {
    return impl->outputs.size ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::OutputManager::GetOutputsCount");
    throw;
  }
}

Output* OutputManager::GetOutput (size_t index) const
{
  try
  {
    if (index >= impl->outputs.size ())
      throw xtl::make_range_exception ("", "index", index, impl->outputs.size ());
      
    return impl->outputs [index].get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::OutputManager::GetOutput");
    throw;
  }
}

/*
    Поиск устройства вывода, перекрытие которого с окном максимально
*/

Output* OutputManager::FindContainingOutput (const void* native_window_handle) const
{
  try
  {
    Screen screen = Screen::ContainingScreen (native_window_handle);
    
    for (OutputArray::iterator iter=impl->outputs.begin (), end=impl->outputs.end (); iter!=end; ++iter)
      if ((*iter)->impl->screen == screen)
        return iter->get ();

    throw xtl::format_operation_exception ("", "Can't find containing output for window %p", native_window_handle);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::OutputManager::FindContainingOutput");
    throw;
  }
}
