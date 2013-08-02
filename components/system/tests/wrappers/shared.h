#include <cstdio>
#include <cmath>

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/ref.h>
#include <xtl/signal.h>
#include <xtl/trackable_ptr.h>

#include <common/action_queue.h>
#include <common/file.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/time.h>
#include <common/utf_converter.h>

#include <syslib/application.h>
#include <syslib/cookie.h>
#include <syslib/dll.h>
#include <syslib/screen.h>
#include <syslib/screen_keyboard.h>
#include <syslib/sensor.h>
#include <syslib/timer.h>
#include <syslib/web_view.h>
#include <syslib/window.h>

using namespace syslib;
using namespace xtl;
