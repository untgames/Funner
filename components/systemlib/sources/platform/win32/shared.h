#include <platform/platform.h>
#include <common/exception.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <stl/hash_map>

#define _WIN32_WINNT 0x0600 //��� ������������� WM_MOUSEWHEEL
#include <windows.h>

namespace syslib
{

//����������: ������ ������������� WinAPI
struct WinAPIExceptionTag;

typedef common::DerivedException<common::Exception, WinAPIExceptionTag> WinAPIException;

//�������� ������ ������������� WinAPI � ��������� ���������� � ������ �� �������
void check_errors (const char* source);
void raise_error  (const char* source);

}
