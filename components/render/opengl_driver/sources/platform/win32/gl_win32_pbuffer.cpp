#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::windows;
using namespace common;

/*
    Описание реализации PBuffer
*/

typedef xtl::com_ptr<PrimarySwapChain> SwapChainPtr;

struct PBuffer::Impl
{
  Log                        log;                   //протокол
  SwapChainPtr               primary_swap_chain;    //основная цепочка обмена
  SwapChainDesc              desc;                  //дескриптор буфера
  HPBUFFERARB                pbuffer;               //дескриптор PBuffer'а
  HDC                        output_context;        //контекст устройства вывода
  int                        pixel_format_index;    //индекс формата пикселей
  bool                       create_largest_flag;   //флаг, сигнализирующий о необходимости создания максимально возможного pbuffer'а
  xtl::auto_connection       cds_connection;        //соединение с сигналом, оповещающим об изменении видео-режима
  const WglExtensionEntries* wgl_extension_entries; //таблица WGL-расширений  

///Конструктор / деструктор
  Impl (PrimarySwapChain* swap_chain)
    : primary_swap_chain (swap_chain),
      pbuffer (0),
      output_context (0)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::windows::PBuffer::Impl::Impl";

    if (!swap_chain)
      throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain");

    cds_connection = swap_chain->RegisterDisplayModeChangeHandler (xtl::bind (&PBuffer::Impl::OnDisplayModeChange, this));

    wgl_extension_entries = &swap_chain->GetWglExtensionEntries ();
    
    pixel_format_index = swap_chain->GetPixelFormat ();

    if (!has_extension (get_wgl_extensions_string (*wgl_extension_entries, swap_chain->GetDC ()).c_str (), "WGL_ARB_pbuffer"))
      throw xtl::format_not_supported_exception (METHOD_NAME, "PBuffer does not supported");        

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

    if (!wgl_extension_entries->QueryPbufferARB (pbuffer, WGL_PBUFFER_LOST_ARB, &is_lost))
      raise_error ("wglQueryPBufferARB");

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
      
      HDC primary_device_context = primary_swap_chain->GetDC ();
        
        //поиск подходящего формата пикселей
        
      int pixel_format = primary_swap_chain->GetPixelFormat ();
      
        //создание PBuffer

      int  pbuffer_attributes []  = {0, 0, WGL_PBUFFER_LARGEST_ARB, 1, 0, 0};
      int* pbuffer_attributes_ptr = create_largest_flag ? pbuffer_attributes + 2 : pbuffer_attributes;
      
      log.Printf ("...call wglCreatePbufferARB");

      pbuffer = wgl_extension_entries->CreatePbufferARB (primary_device_context, pixel_format, desc.frame_buffer.width, desc.frame_buffer.height,
                                                         pbuffer_attributes_ptr);

      if (!pbuffer)
        raise_error ("wglCreatePbufferARB");
        
        //получение контекста вывода
        
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
    }
    catch (...)
    {
      log.Printf ("...PBuffer creation failed");

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

      throw;
    }
  }

  void Destroy ()
  {
    try
    {
      log.Printf ("Destroy PBuffer...");

      log.Printf ("...call wglReleasePbufferDCARB");

      wgl_extension_entries->ReleasePbufferDCARB (pbuffer, output_context);

      log.Printf ("...call wglDestroyPbufferARB");

      wgl_extension_entries->DestroyPbufferARB (pbuffer);
    }
    catch (...)
    {
      //подавляем все исключения
    }

    output_context = 0;
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
    Получение драйвера
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
HDC PBuffer::GetDC ()
{
  return impl->output_context;
}

//получение формата пикселей цепочки обмена
int PBuffer::GetPixelFormat ()
{
  return impl->pixel_format_index;
}

//есть ли вертикальная синхронизация
bool PBuffer::HasVSync ()
{
  return false;
}

//получение точек входа
const WglExtensionEntries& PBuffer::GetWglExtensionEntries ()
{
  return *impl->wgl_extension_entries;
}
