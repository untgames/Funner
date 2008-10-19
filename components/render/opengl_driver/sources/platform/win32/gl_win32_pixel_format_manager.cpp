#include "shared.h"

using namespace render::low_level::opengl::windows;

namespace
{

/*
    Дескриптор формата пикселей контекста устройства
*/

struct DcPixelFormat
{
  HDC                                       dc;           //контекст устройства
  int                                       pixel_format; //формат пикселей
  PixelFormatManager::DescribePixelFormatFn describe;     //функция, описывающая формат пикселей
  DcPixelFormat*                            prev;         //предыдущий дескриптор
  DcPixelFormat*                            next;         //следующий дескриптор
};

DcPixelFormat*   first           = 0; //первый элемент в списке форматов пикселей
IAdapterLibrary* default_library = 0; //библиотека, используемая для описания формата пикселей "по умолчанию"

//перехват функции получения адреса
PROC WINAPI GetProcAddressRedirect (HMODULE module, const LPCSTR name)
{
  void* proc = (void*)GetProcAddress (module, name);
  
  if (proc == &::SetPixelFormat)
    return reinterpret_cast<PROC> (xtl::implicit_cast<BOOL (WINAPI*)(HDC, int, LPPIXELFORMATDESCRIPTOR)> (&PixelFormatManager::SetPixelFormat));
    
  if (proc == &::GetPixelFormat)
    return reinterpret_cast<PROC> (&PixelFormatManager::GetPixelFormat);
  
  if (proc == &::DescribePixelFormat)
    return reinterpret_cast<PROC> (&PixelFormatManager::DescribePixelFormat);
    
  if (proc == &::GetProcAddress)
    return reinterpret_cast<PROC> (&GetProcAddressRedirect);
    
  return (PROC)proc;
}

}

/*
    Перенаправление вызовов
*/

void PixelFormatManager::RedirectApiCalls (HMODULE module)
{
  redirect_dll_call (module, "gdi32.dll",    (void*)&::SetPixelFormat,      (void*)xtl::implicit_cast<BOOL (WINAPI*)(HDC, int, LPPIXELFORMATDESCRIPTOR)> (&PixelFormatManager::SetPixelFormat));
  redirect_dll_call (module, "gdi32.dll",    (void*)&::GetPixelFormat,      (void*)&PixelFormatManager::GetPixelFormat);
  redirect_dll_call (module, "gdi32.dll",    (void*)&::DescribePixelFormat, (void*)&PixelFormatManager::DescribePixelFormat);
  redirect_dll_call (module, "kernel32.dll", (void*)&::GetProcAddress,      (void*)&GetProcAddressRedirect);
}

/*
    Получение формата пикселей
*/

int WINAPI PixelFormatManager::GetPixelFormat (HDC dc)
{
  for (DcPixelFormat* iter=first; iter; iter=iter->next)
    if (iter->dc == dc)
      return iter->pixel_format;

  return 0;
}


/*
    Установка формата пикселей
*/

BOOL WINAPI PixelFormatManager::SetPixelFormat (HDC dc, int pixel_format, DescribePixelFormatFn describe)
{
  try
  {
      //проверка корректности аргументов

    if (!dc || !pixel_format || !describe)
      return FALSE;

      //поиск дескриптора
      
    if (GetPixelFormat (dc))
      return FALSE;         
        
      //добавление нового дескриптора

    DcPixelFormat* desc = new DcPixelFormat;

    desc->dc           = dc;
    desc->pixel_format = pixel_format;
    desc->describe     = describe;
    desc->prev         = 0;
    desc->next         = first;

    if (first) first->prev = desc;

    first = desc;

    return TRUE;
  }
  catch (...)
  {
    //подавление всех исключений

    return FALSE;
  }
}

BOOL WINAPI PixelFormatManager::SetPixelFormat (HDC dc, int pixel_format, PIXELFORMATDESCRIPTOR*)
{
  return FALSE;
}

/*
    Описание формата пикселей
*/

int WINAPI PixelFormatManager::DescribePixelFormat (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd)
{
    //поиск вхождения контекста устройства

  DcPixelFormat* iter = first;

  for (;iter && iter->dc != dc; iter=iter->next);

    //если контекст не найден - описание формата невозможно

  if (!iter)
  {
    if (default_library)
      return default_library->DescribePixelFormat (dc, pixel_format, size, pfd);

    return 0;        
  }

  return iter->describe (dc, pixel_format, size, pfd);
}

/*
    Очистка ресурсов контекста устройства
*/

void PixelFormatManager::ReleasePixelFormat (HDC dc)
{
  for (DcPixelFormat* iter=first; iter;)
    if (iter->dc == dc)
    {
      DcPixelFormat* next = iter->next;
      
      if (iter->prev) iter->prev->next = iter->next;
      else            first            = iter->next;

      if (iter->next) iter->next->prev = iter->prev;

      delete iter;
      
      iter = next;
    }
    else iter = iter->next;
}

/*
    Установка библиотеки "по умолчанию"
*/

void PixelFormatManager::SetDefaultLibrary (IAdapterLibrary* library)
{
  default_library = library;
}
