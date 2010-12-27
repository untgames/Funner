#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    Описание реализации первичной цепочки обмена
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef Output::Pointer       OutputPtr;

struct PrimarySwapChain::Impl
{
  Log               log;         //протокол
  AdapterPtr        adapter;     //адаптер, которому принадлежит устройство
  AdapterLibraryPtr library;     //библиотека адаптера
  Display*          display;     //соединение с дисплеем
  Window            window;      //окно
  SwapChainDesc     desc;        //дескриптор цепочки обмена
  PropertyList      properties;  //свойства цепочки обмена

///Конструктор
  Impl (Adapter* in_adapter, const SwapChainDesc& in_desc)
    : adapter (in_adapter)
    , library (in_adapter->GetLibrary ())
    , display ((Display*)syslib::x11::DisplayManager::DisplayHandle ())
    , window ((Window) in_desc.window_handle)
    , desc (in_desc)
  {
    try
    {
    }
    catch (...)
    {
      throw;
    }
  }
  
///Деструктор
  ~Impl ()
  {

  }  
};

/*
    Конструктор / деструктор
*/

PrimarySwapChain::PrimarySwapChain (Adapter* adapter, const SwapChainDesc& desc)
{
  try
  {
    if (!adapter)
      throw xtl::make_null_argument_exception ("", "adapter");
    
    impl = new Impl (adapter, desc);        

    impl->log.Printf ("...swap chain successfully created");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::PrimarySwapChain");
    throw;
  }
}

PrimarySwapChain::~PrimarySwapChain ()
{
}

/*
    Получение адаптера
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    Получение дескриптора
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::GetContainingOutput");
}

/*
    Установка / взятие состояния full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::SetFullscreenState");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::GetFullscreenState");
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void PrimarySwapChain::Present ()
{
  try
  {
    impl->library->SwapBuffers (impl->display, impl->window);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::Present");
    throw;
  }
}

/*
    Список свойств устройства вывода
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}
