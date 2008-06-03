#ifndef RENDER_LOW_LEVEL_TESTS_SHARED_HEADER
#define RENDER_LOW_LEVEL_TESTS_SHARED_HEADER

#include <stdio.h>

#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>

#include <common/exception.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/utils.h>

using namespace render::low_level;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4250)
#endif

//объект системы отрисовки
class ObjectImpl: virtual public IObject, public xtl::reference_counter, public xtl::trackable
{
  public:
    IPropertyList* GetProperties () { return 0; }
  
    void AddRef () { addref (this); }  
    void Release () { release (this); }
    
    xtl::trackable& GetTrackable () { return *this; }
    
    xtl::connection RegisterDestroyHandler (xtl::trackable::slot_type& handler)
    {
      return xtl::trackable::connect_tracker (handler);
    }

    xtl::connection RegisterDestroyHandler (const xtl::trackable::function_type& handler)
    {
      return xtl::trackable::connect_tracker (handler);
    }

    xtl::connection RegisterDestroyHandler (const xtl::trackable::function_type& handler, xtl::trackable& linked_trackable)
    {
      return xtl::trackable::connect_tracker (handler, linked_trackable);
    }
};

//тестовое устройство вывода
class TestOutput: public IOutput, public ObjectImpl
{
  public:
    TestOutput ()
    {
      test_mode.width        = 100;
      test_mode.height       = 100;
      test_mode.color_bits   = 24;
      test_mode.refresh_rate = 100;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени / описания устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetName        () { return "TestOutput"; }
    const char* GetDescription () { return "Test output class"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение списка видео-режимов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t GetModesCount () { return 1; }
    void   GetModeDesc   (size_t, OutputModeDesc& mode_desc)
    {
      mode_desc = test_mode;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка текущего видео-режима
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetCurrentMode (const OutputModeDesc& mode_desc)
    {
      test_mode = mode_desc;
    }

    void GetCurrentMode (OutputModeDesc& mode_desc)
    {
      mode_desc = test_mode;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление гамма-коррекцией
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetGammaRamp (const Color3f table [256]) { }
    void GetGammaRamp (Color3f table [256]) { }

  private:
    OutputModeDesc test_mode;
};

//тестовый драйвер
class TestDriver: public IDriver, public ObjectImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* GetDescription () { return "TestDriver"; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление доступных устройств вывода
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   GetOutputsCount () { return 1; }
    IOutput* GetOutput       (size_t index) { return &output; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (const SwapChainDesc&)
    {
      raise_not_implemented ("TestDriver::CreateSwapChain");
      return 0;
    }

    ISwapChain* CreateSwapChain (IOutput*, const SwapChainDesc&)
    {
      raise_not_implemented ("TestDriver::CreateSwapChain");
      return 0;
    }    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (ISwapChain*, const char*)
    {
      raise_not_implemented ("TestDriver::CreateDevice");
      return 0;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка функции отладочного протоколирования драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////   
    void SetDebugLog (const LogFunction& in_log)
    {
      log_fn = in_log;
    }
    
    const LogFunction& GetDebugLog ()
    {
      return log_fn;
    }
    
  private:
    LogFunction log_fn;
    TestOutput  output;
};

#endif
