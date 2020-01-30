#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    Constants
*/

const char*  OPENGL_LIB_PATH                 = "libGL.so.1"; //OpenGL library file name (according to OpenGL® Application Binary Interface for Linux https://www.opengl.org/registry/ABI/#3)
const size_t PIXEL_FORMAT_ARRAY_RESERVE_SIZE = 256;          //reserved pixel formats count

/*
    Implementation of glx platform manager
*/

class PlatformManagerImpl
{
  public:
///Constructor
    PlatformManagerImpl ()
    {
      try
      {
          //load adapter "by default"

        LoadDefaultAdapter ("GLX", OPENGL_LIB_PATH);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::glx::PlatformManagerImpl::PlatformManagerImpl");
        throw;
      }      
    }

///Create adapter
    Adapter* CreateAdapter (const char* name, const char* path, const char* init_string)
    {
      try
      {
        if (!name)
          throw xtl::make_null_argument_exception ("", "name");

        if (!path)
          throw xtl::make_null_argument_exception ("", "path");

        if (!init_string)
          init_string = "";

        log.Printf ("Create adapter '%s' (path='%s', init_string='%s')...", name, path, init_string);

        return new Adapter (name, path, init_string);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::glx::PlatformManagerImpl::CreateAdapter");
        throw;
      }
    }
    
///Attempt to load default adapter
    void LoadDefaultAdapter (const char* name, const char* path, const char* init_string = "")
    {
      try
      {
          //create adapter

        AdapterPtr adapter (CreateAdapter (name, path, init_string), false);

          //register adapter

        default_adapters.push_front (adapter);
      }
      catch (std::exception& exception)
      {        
        log.Printf ("%s\n    at render::low_level::opengl::glx::PlatformManager::LoadDefaultAdapter('%s', '%s')", exception.what (), name, path);
      }
      catch (...)
      {
        log.Printf ("Unknown exception\n    at render::low_level::opengl::glx::PlatformManager::LoadDefaultAdapter('%s', '%s')", name, path);
      }
    }

///Enumerate default adapters
    void EnumDefaultAdapters (const xtl::function<void (IAdapter*)>& handler)
    {
      for (AdapterList::iterator iter=default_adapters.begin (), end=default_adapters.end (); iter!=end; ++iter)
        handler (get_pointer (*iter));
    }

///Create swap chain
    ISwapChain* CreateSwapChain (size_t adapters_count, IAdapter** adapters, const SwapChainDesc& desc)
    {
      try
      {
          //check arguments correctness

        if (adapters_count && !adapters)
          throw xtl::make_null_argument_exception ("", "adapters");

        if (adapters_count != 1)
          throw xtl::format_not_supported_exception ("", "Multiple adapters not supported");

        Adapter* adapter = dynamic_cast<Adapter*> (*adapters);

        if (!adapter)
          throw xtl::format_not_supported_exception ("", "Only default adapter supported");

        log.Printf ("Create primary swap chain...");
        
          //enumerate available pixel formats

        Adapter::PixelFormatArray          pixel_formats;
        Adapter::GlxExtensionsEntriesArray glx_extensions_entries;

        pixel_formats.reserve (PIXEL_FORMAT_ARRAY_RESERVE_SIZE);
        glx_extensions_entries.reserve (adapters_count);
        
        int screen = get_screen_number ((Window)desc.window_handle);
        
        for (size_t i=0; i<adapters_count; i++)
        {
          IAdapter* in_adapter = adapters [i];

          if (!in_adapter)
            throw xtl::format_exception<xtl::null_argument_exception> ("", "Null argument 'adapters[%u]'", i);
            
          Adapter* volatile adapter = dynamic_cast<Adapter*> (in_adapter);
          
          if (!adapter)
            throw xtl::format_exception<xtl::argument_exception> ("", "Invalid argument 'adapters[%u]'. Wrong type '%s'",
              i, typeid (*in_adapter).name ());
              
          try
          {
            log.Printf ("...enumerate pixel formats on adapter '%s'", adapter->GetName ());
            
            adapter->EnumPixelFormats (screen, pixel_formats, glx_extensions_entries);
          }
          catch (std::exception& exception)
          {
            log.Printf ("%s\n    at enumerate pixel formats on adapter '%s'", exception.what (), adapter->GetName ());
          }
          catch (...)
          {
            log.Printf ("Unknown exception\n    at enumerate pixel formats on adapter '%s'", adapter->GetName ());
          }
        }

          //choose best match pixel format

        const PixelFormatDesc& pixel_format = ChoosePixelFormat (pixel_formats, desc);
        
          //log choosen pixel format
          
        stl::string flags;
        
        if (pixel_format.buffers_count > 1)
        {
          flags += ", SwapMethod=";
          flags += get_name (pixel_format.swap_method);
          flags += ", DOUBLE_BUFFER";
        }

        if (pixel_format.support_draw_to_pbuffer)
        {
          if (flags.empty ()) flags += ", ";
          else                flags += " | ";          
          
          flags += "PBUFFER";
        }

        if (pixel_format.support_stereo)
        {
          if (flags.empty ()) flags += ", ";
          else                flags += " | ";          

          flags += "STEREO";
        }

        log.Printf ("...choose pixel format #%u on adapter '%s' (RGB/A: %u/%u, D/S: %u/%u, Samples: %u%s)",
          pixel_format.pixel_format_index, pixel_format.adapter->GetName (), pixel_format.color_bits,
          pixel_format.alpha_bits, pixel_format.depth_bits, pixel_format.stencil_bits, pixel_format.samples_count, flags.c_str ());

          //create swap chain

        return new PrimarySwapChain (desc, pixel_format);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::glx::PlatformManagerImpl::CreateSwapChain");
        throw;
      }      
    }

///Create offscreen buffer
    ISwapChain* CreatePBuffer (ISwapChain* source_chain, const SwapChainDesc* pbuffer_desc)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PlatformManagerImpl::CreatePBuffer");
    }

///Create context
    render::low_level::opengl::IContext* CreateContext (ISwapChain* swap_chain)
    {
      try
      {
        return new Context (swap_chain);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::glx::PlatformManagerImpl::CreateContext");
        throw;
      }
    }
    
///Compare two numeric parameters (0 - first format matches better, 1 - second format matches better)
    static int CompareFormatCounts (size_t source1, size_t source2, size_t require)
    {
      if (source1 == require)
        return 0;

      if (source2 == require)
        return 1;

      if (source1 < require)
      {
         if (source2 < require) return source1 < source2;
         else                   return 1;
      }
      else //source1 > require
      {
        if (source2 > require) return source1 > source2;
        else                   return 0;
      }
    }
    
///Compare two formats (0 - first format matches better, 1 - second format matches better)
    static int CompareFormats (const PixelFormatDesc& fmt0, const PixelFormatDesc& fmt1, const SwapChainDesc& swap_chain_desc)
    {
        //sort by existing of double buffering
        
      if (fmt0.buffers_count != fmt1.buffers_count)
      {
        if (swap_chain_desc.buffers_count < 2)
        {
          if (fmt0.buffers_count == 1)
            return 0;

          if (fmt1.buffers_count == 1)
            return 1;          
        }
        else
        {
          if (fmt0.buffers_count >= 2 && fmt1.buffers_count < 2)
            return 0;

          if (fmt0.buffers_count < 2 && fmt1.buffers_count >= 2)
            return 1;
        }
      }

        //sort by color bits count
        
      if (fmt0.color_bits != fmt1.color_bits)
        return CompareFormatCounts (fmt0.color_bits, fmt1.color_bits, swap_chain_desc.frame_buffer.color_bits);
        
        //sort by depth bits count

      if (fmt0.depth_bits != fmt1.depth_bits)
        return CompareFormatCounts (fmt0.depth_bits, fmt1.depth_bits, swap_chain_desc.frame_buffer.depth_bits);

        //sort by stencil bits count

      if (fmt0.stencil_bits != fmt1.stencil_bits)
        return CompareFormatCounts (fmt0.stencil_bits, fmt1.stencil_bits, swap_chain_desc.frame_buffer.stencil_bits);

        //sort by samples count

      if (fmt0.samples_count != fmt1.samples_count)
        return CompareFormatCounts (fmt0.samples_count, fmt1.samples_count, swap_chain_desc.samples_count);

        //sort by alpha bits count

      if (fmt0.alpha_bits != fmt1.alpha_bits)
        return CompareFormatCounts (fmt0.alpha_bits, fmt1.alpha_bits, swap_chain_desc.frame_buffer.alpha_bits);        

        //sort by swap method
        
      if (fmt0.swap_method != fmt1.swap_method)
      {
        if (fmt0.swap_method == swap_chain_desc.swap_method)
          return 0;
          
        if (fmt1.swap_method == swap_chain_desc.swap_method)
          return 1;
      }

        //sort by buffers count

      if (fmt0.buffers_count != fmt1.buffers_count)
        return CompareFormatCounts (fmt0.buffers_count, fmt1.buffers_count, swap_chain_desc.buffers_count);
        
        //sort by PBuffer support
        
      if (fmt0.support_draw_to_pbuffer != fmt1.support_draw_to_pbuffer)
        return fmt1.support_draw_to_pbuffer;
        
      if (fmt0.support_draw_to_window != fmt1.support_draw_to_window)
        return fmt1.support_draw_to_window;

        //sort by stereo mode support
        
      if (fmt0.support_stereo != fmt1.support_stereo)
        return fmt1.support_stereo;

        //first format is better if all other params are equal

      return 1;
    }
    
///Choose pixel format
    const PixelFormatDesc& ChoosePixelFormat (const Adapter::PixelFormatArray& pixel_formats, const SwapChainDesc& swap_chain_desc)
    {
        //if suitable pixel format not found - we can't create swap chain

      if (pixel_formats.empty ())
        throw xtl::format_operation_exception ("render::low_level::opengl::glx::PlatformManagerImpl::ChoosePixelFormat", "No pixel formats found");

        //find pixel format

      const PixelFormatDesc* best = &pixel_formats [0];
      
      for (Adapter::PixelFormatArray::const_iterator iter=pixel_formats.begin ()+1, end=pixel_formats.end (); iter!=end; ++iter)
      {
        if (CompareFormats (*best, *iter, swap_chain_desc))
          best = &*iter;      }

      return *best;
    }

  private:
    typedef xtl::com_ptr<Adapter> AdapterPtr;
    typedef stl::list<AdapterPtr> AdapterList;

  private:
    Log              log;              //log
    AdapterList      default_adapters; //default adapters
};

typedef common::Singleton<PlatformManagerImpl> PlatformManagerSingleton;

}

/*
    Platfomr manager wrappers
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
