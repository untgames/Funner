#include "shared.h"

using namespace render::low_level::opengl::windows;

namespace
{

/*
    Дескриптор формата пикселей контекста устройства
*/

struct DcPixelFormat
{
  HDC            dc;           //контекст устройства
  int            pixel_format; //формат пикселей
  DcPixelFormat* prev;         //предыдущий дескриптор
  DcPixelFormat* next;         //следующий дескриптор  
};

DcPixelFormat* first = 0;

}

/*
    Перенаправление вызовов
*/

void PixelFormatManager::RedirectApiCalls (HMODULE module, void* describe_pixel_format_fn)
{
  redirect_dll_call (module, "gdi32.dll", &::SetPixelFormat,      &PixelFormatManager::SetPixelFormat);
  redirect_dll_call (module, "gdi32.dll", &::GetPixelFormat,      &PixelFormatManager::GetPixelFormat);
  redirect_dll_call (module, "gdi32.dll", &::DescribePixelFormat, describe_pixel_format_fn);
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

BOOL WINAPI PixelFormatManager::SetPixelFormat (HDC dc, int pixel_format, PIXELFORMATDESCRIPTOR*)
{
  try
  {
      //проверка корректности аргументов

    if (!dc || !pixel_format)
      return FALSE;

      //поиск дескриптора
      
    if (GetPixelFormat (dc))
      return FALSE;         
        
      //добавление нового дескриптора
      
    DcPixelFormat* desc = new DcPixelFormat;
    
    desc->dc           = dc;
    desc->pixel_format = pixel_format;
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

/*
    Очистка ресурсов контекста устройства
*/

void WINAPI PixelFormatManager::ReleasePixelFormat (HDC dc)
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
