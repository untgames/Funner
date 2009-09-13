#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::iphone;

namespace
{

/*
    Реализация менеджера платформы
*/

class PlatformManagerImpl
{
  public:
    typedef render::low_level::opengl::IContext IContext;

///Конструктор
    PlatformManagerImpl ()
    {
      default_adapters.push_back (AdapterPtr (new Adapter, false));
    }

///Создание адаптера
    IAdapter* CreateAdapter (const char* name, const char* path, const char* init_string)
    {
      throw xtl::format_not_supported_exception ("render::low_level::opengl::iphone::PlatformManagerImpl::CreateAdapter", "Custom adapters not supported");
    }

///Перечисление адаптеров "по умолчанию"
    void EnumDefaultAdapters (const xtl::function<void (IAdapter*)>& handler)
    {
      for (AdapterList::iterator iter=default_adapters.begin (), end=default_adapters.end (); iter!=end; ++iter)
        handler (get_pointer (*iter));
    }

///Создание цепочки обмена
    ISwapChain* CreateSwapChain (size_t adapters_count, IAdapter** adapters, const SwapChainDesc& desc)
    {
      try
      {
          //проверка корректности аргументов

        if (adapters_count && !adapters)
          throw xtl::make_null_argument_exception ("", "adapters");

        if (adapters_count != 1)
          throw xtl::format_not_supported_exception ("", "Multiple adapters not supported");

        Adapter* adapter = dynamic_cast<Adapter*> (*adapters);

        if (!adapter)
          throw xtl::format_not_supported_exception ("", "Only default adapter supported");

        log.Printf ("Create swap chain...");

          //создание цепочки обмена

        return new PrimarySwapChain (desc, adapter);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::iphone::PlatformManagerImpl::CreateSwapChain");
        throw;
      }
    }

///Создание PBuffer'а
    ISwapChain* CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::iphone::PlatformManagerImpl::CreatePBuffer");
    }

///Создание контекста
    IContext* CreateContext (ISwapChain* swap_chain)
    {
      try
      {
        return new Context (swap_chain, &opengl_library);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::iphone::PlatformManagerImpl::CreateContext");
        throw;
      }
    }

  private:
    typedef xtl::com_ptr<Adapter> AdapterPtr;
    typedef stl::list<AdapterPtr> AdapterList;

  private:
    Log         log;              //протокол
    AdapterList default_adapters; //адаптеры
    Library     opengl_library;   //библиотека функций OpenGL
};

typedef common::Singleton<PlatformManagerImpl> PlatformManagerSingleton;

}

/*
    Обёртки над менеджером платформы
*/

IAdapter* PlatformManager::CreateAdapter (const char* name, const char* path, const char* init_string)
{
  return PlatformManagerSingleton::Instance ()->CreateAdapter (name, path, init_string);
}

void PlatformManager::EnumDefaultAdapters (const xtl::function<void (IAdapter*)>& handler)
{
  PlatformManagerSingleton::Instance ()->EnumDefaultAdapters (handler);
}

ISwapChain* PlatformManager::CreateSwapChain (size_t adapters_count, IAdapter** adapters, const SwapChainDesc& desc)
{
  return PlatformManagerSingleton::Instance ()->CreateSwapChain (adapters_count, adapters, desc);
}

ISwapChain* PlatformManager::CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc)
{
  return PlatformManagerSingleton::Instance ()->CreatePBuffer (source_chain, pbuffer_desc);
}

render::low_level::opengl::IContext* PlatformManager::CreateContext (ISwapChain* swap_chain)
{
  return PlatformManagerSingleton::Instance ()->CreateContext (swap_chain);
}
