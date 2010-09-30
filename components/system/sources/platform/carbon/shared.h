#include <stl/string>

#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/file.h>
#include <common/log.h>

#include <platform/platform.h>

#include <syslib/application.h>

#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>

namespace syslib
{

//Проверка ошибок Carbon
void check_event_manager_error  (OSStatus error_code, const char* source, const char* message);
void check_window_manager_error (OSStatus error_code, const char* source, const char* message);
void check_quartz_error         (CGError  error_code, const char* source, const char* message);

//Преобразование процесса к приложению
void transform_process_type ();

}
