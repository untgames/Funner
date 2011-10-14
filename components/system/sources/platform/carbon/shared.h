#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <common/component.h>
#include <common/file.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/xml_writer.h>

#include <shared/platform.h>

#include <syslib/application.h>
#include <syslib/timer.h>

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
