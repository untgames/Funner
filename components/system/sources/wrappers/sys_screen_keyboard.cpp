#include "shared.h"

using namespace syslib;

/*
    �������� ���������� �������� ����������
*/

struct ScreenKeyboard::Impl
{
  screen_keyboard_t handle;   //�������������� ���������� ����������
  bool              is_shown; //�������� �� ����������

/// �����������
  Impl (syslib::Window& window, ScreenKeyboardType type, const char* platform_specific)
    : handle ()
    , is_shown ()
  {
    if (type < 0 || type > ScreenKeyboardType_PlatformSpecific)
      throw xtl::make_argument_exception ("", "type", type);

    if (!platform_specific)
      throw xtl::make_null_argument_exception ("", "platform_specific");

    handle = Platform::CreateScreenKeyboard (reinterpret_cast<window_t> (const_cast<void*> (window.InternalHandle ())), type, platform_specific);
  }

/// ����������
  ~Impl ()
  {
    try
    {
      Platform::DestroyScreenKeyboard (handle);
    }
    catch (...)
    {
    }
  }
};

/*
    ����������� / ����������
*/

ScreenKeyboard::ScreenKeyboard (syslib::Window& window, ScreenKeyboardType type, const char* platform_specific)
{
  try
  {
    impl.reset (new Impl (window, type, platform_specific));
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::ScreenKeyboard::ScreenKeyboard");
    throw;
  }
}

ScreenKeyboard::~ScreenKeyboard ()
{
  try
  {
    Hide ();
  }
  catch (...)
  {
  }
}

/*
    �������� �� ����������
*/

bool ScreenKeyboard::IsShown () const
{
  return impl->is_shown;
}

/*
    ����� � ������� ����������
*/

void ScreenKeyboard::Show ()
{
  try
  {
    if (impl->is_shown)
      return;

    Platform::ShowScreenKeyboard (impl->handle);

    impl->is_shown = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::ScreenKeyboard::Show");
    throw;
  }
}

void ScreenKeyboard::Hide ()
{
  try
  {
    if (!impl->is_shown)
      return;

    Platform::HideScreenKeyboard (impl->handle);

    impl->is_shown = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::ScreenKeyboard::Hide");
    throw;
  }
}

/*
    �������� ��������� ����������
*/

bool ScreenKeyboard::IsSupported (ScreenKeyboardType type)
{
  try
  {
    return Platform::IsScreenKeyboardSupported (type);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::ScreenKeyboard::IsSupported(ScreenKeyboardType)");
    throw;
  }
}

bool ScreenKeyboard::IsSupported (const char* platform_specific)
{
  try
  {
    return Platform::IsScreenKeyboardSupported (platform_specific);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::ScreenKeyboard::IsSupported(const char*)");
    throw;
  }
}
