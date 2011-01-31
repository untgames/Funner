#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    Описание реализации PBuffer
*/

typedef xtl::com_ptr<PrimarySwapChain> SwapChainPtr;

struct PBuffer::Impl
{
  Log                        log;                   //протокол
  SwapChainPtr               primary_swap_chain;    //основная цепочка обмена
  AdapterLibraryPtr          library;               //библиотека OpenGL
  SwapChainDesc              desc;                  //дескриптор буфера
  Display*                   display;               //соединение с дисплеем
  GLXPbuffer                 pbuffer;               //дескриптор PBuffer'а
  GLXFBConfig                fbconfig;              //конфигурация буфера кадра
  bool                       create_largest_flag;   //флаг необходимости создания максимально возможного pbuffer'а
  xtl::auto_connection       cds_connection;        //соединение с сигналом, оповещающим об изменении видео-режима
  const GlxExtensionEntries* glx_extension_entries; //таблица GLX-расширений  

///Конструктор / деструктор
  Impl (PrimarySwapChain* swap_chain)
    : pbuffer (0)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::glx::PBuffer::Impl::Impl";

    if (!swap_chain)
      throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain");
      
    primary_swap_chain = swap_chain;
    
    library = &swap_chain->GetAdapterImpl ()->GetLibrary ();

    display = swap_chain->GetDisplay ();
    
    fbconfig = swap_chain->GetFBConfig ();
    
//    cds_connection = swap_chain->RegisterDisplayModeChangeHandler (xtl::bind (&PBuffer::Impl::OnDisplayModeChange, this));

    glx_extension_entries = &swap_chain->GetGlxExtensionEntries ();

//    if (!has_extension (get_wgl_extensions_string (*wgl_extension_entries, swap_chain->GetDC ()).c_str (), "WGL_ARB_pbuffer"))
//      throw xtl::format_not_supported_exception (METHOD_NAME, "PBuffer does not supported");        

    swap_chain->GetDesc (desc);

    desc.vsync         = false;
    desc.fullscreen    = false;
    desc.window_handle = 0;
  }

  ~Impl ()
  {
    Destroy ();
  }

///Обработка события смены видео-режима
  void OnDisplayModeChange ()
  {
      //проверка состояния P-buffer

    int is_lost = 0;

      //если буфер потерял своё содержимое - пересоздаём его

    if (is_lost)    
    {
      Destroy ();
      Create ();
    }
  }

///Создание / уничтожение буфера
  void Create ()
  {
    try
    {
      log.Printf ("Create PBuffer...");
      
        //создание PBuffer

      int pbuffer_attributes []  = { GLX_PBUFFER_WIDTH, desc.frame_buffer.width,
                                     GLX_PBUFFER_HEIGHT, desc.frame_buffer.height,
                                     GLX_LARGEST_PBUFFER, create_largest_flag,
                                     GLX_PRESERVED_CONTENTS, False};

      log.Printf ("...call glxCreatePbuffer");

      pbuffer = library->CreatePbuffer (display, fbconfig, pbuffer_attributes);

      if (!pbuffer)
        raise_error ("glxCreatePbuffer");
        
        //получение контекста вывода
/*        
      log.Printf ("...call wglGetPbufferDCARB");
        
      output_context = wgl_extension_entries->GetPbufferDCARB (pbuffer);

      if (!output_context)
        raise_error ("wglGetPbufferDCARB");

        //установка формата пикселей

      if (!PixelFormatManager::CopyPixelFormat (primary_device_context, output_context))
        throw xtl::format_operation_exception ("render::low_level::opengl::PBuffer::Impl::Create", "PixelFormatManager::CopyPixelFormat failed");

        //получение размеров созданного PBuffer'а
        
      log.Printf ("...call wglQueryPBufferARB");
        
      int width, height;

      if (!wgl_extension_entries->QueryPbufferARB (pbuffer, WGL_PBUFFER_WIDTH_ARB, &width) || 
          !wgl_extension_entries->QueryPbufferARB (pbuffer, WGL_PBUFFER_HEIGHT_ARB, &height))
        raise_error ("wglQueryPBufferARB");

      log.Printf ("...PBuffer %ux%u successfully created", width, height);

      desc.frame_buffer.width  = (size_t)width;
      desc.frame_buffer.height = (size_t)height;
*/
    }
    catch (...)
    {
      log.Printf ("...PBuffer creation failed");
/*
      if (output_context)
      {
        log.Printf ("...call wglReleasePbufferDCARB");

        wgl_extension_entries->ReleasePbufferDCARB (pbuffer, output_context);
      }

      if (pbuffer)
      {
        log.Printf ("...call wglDestroyPbufferARB");
        
        wgl_extension_entries->DestroyPbufferARB (pbuffer);
      }
      
      output_context = 0;
      pbuffer = 0;
*/
      throw;
    }
  }

  void Destroy ()
  {
    try
    {
      log.Printf ("Destroy PBuffer...");

      log.Printf ("...call glxDestroyPbuffer");

      library->DestroyPbuffer (display, pbuffer);
    }
    catch (...)
    {
      //подавляем все исключения
    }

    pbuffer = 0;
    
    log.Printf ("...PBuffer successfully destroyed");
  }
};

/*
    Конструктор / деструктор
*/

PBuffer::PBuffer (PrimarySwapChain* swap_chain, size_t width, size_t height)
  : impl (new Impl (swap_chain))
{
  impl->create_largest_flag = false;

    //заполнение дескриптора

  impl->desc.frame_buffer.width  = width;
  impl->desc.frame_buffer.height = height;

    //создание буфера

  impl->Create ();
}

PBuffer::PBuffer (PrimarySwapChain* swap_chain)
  : impl (new Impl (swap_chain))
{
  impl->create_largest_flag = true;

    //создание буфера

  impl->Create ();
}

PBuffer::~PBuffer ()
{
}

/*
    Получение дескриптора
*/

void PBuffer::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* PBuffer::GetContainingOutput ()
{
  return impl->primary_swap_chain->GetContainingOutput ();
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void PBuffer::Present ()
{
}

/*
    Установка / взятие состояния full-screen mode
*/

void PBuffer::SetFullscreenState (bool)
{
}

bool PBuffer::GetFullscreenState ()
{
  return false;
}

/*
    Получение 
*/

IAdapter* PBuffer::GetAdapter ()
{
  return impl->primary_swap_chain->GetAdapter ();
}

/*
   Реалиация интерфейса ISwapChainImpl
*/

//получение реализации адаптера
Adapter* PBuffer::GetAdapterImpl ()
{
  return impl->primary_swap_chain->GetAdapterImpl ();
}

//получение контекста устройства вывода
Display* PBuffer::GetDisplay ()
{
  return impl->display;
}

//
Window PBuffer::GetWindow ()
{
  return impl->primary_swap_chain->GetWindow ();
}

//получение формата пикселей цепочки обмена
GLXFBConfig PBuffer::GetFBConfig ()
{
  return impl->fbconfig;
}

//получение точек входа
const GlxExtensionEntries& PBuffer::GetGlxExtensionEntries ()
{
  return *impl->glx_extension_entries;
}
