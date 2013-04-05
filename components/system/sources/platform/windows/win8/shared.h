#include "../shared.h"

#include <wrl/client.h>
#include <ppltasks.h>
#include <agile.h>

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <locale.h>
#include <wrl/client.h>

#include <xtl/bind.h>
#include <xtl/utility>

#include <common/strconv.h>
#include <common/utf_converter.h>

#include <syslib/mutex.h>

namespace syslib
{

namespace win8
{

/// Контекст приложения
interface class IApplicationContext
{
  /// Получение главного окна
  Windows::UI::Core::CoreWindow^ MainWindow ();
};

/// Получение контекста приложения
IApplicationContext^ get_context ();

/// Преобразования строк
stl::string tostring (Platform::String^ input);

/// Выброс исключения с сообщением HRESULT
void raise_com_error (const char* method, HRESULT error);

/// Преобразование кодов клавиш
Key get_key_code (Windows::System::VirtualKey key);

}

}
