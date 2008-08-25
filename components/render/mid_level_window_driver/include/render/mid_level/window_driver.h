#ifndef RENDER_MID_LEVEL_WINDOW_DRIVER_HEADER
#define RENDER_MID_LEVEL_WINDOW_DRIVER_HEADER

namespace render
{

namespace mid_level
{

//forward declaration
class IDriver;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowDriver
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* Name ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IDriver* Driver ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterWindow       (const char* renderer_name, syslib::Window& window, const char* configuration_branch);
    static void UnregisterWindow     (const char* renderer_name, syslib::Window& window);
    static void UnregisterAllWindows (const char* renderer_name);
    static void UnregisterAllWindows ();
};

}

}

#endif
