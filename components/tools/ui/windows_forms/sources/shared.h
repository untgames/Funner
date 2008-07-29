/////////////!!!сделать обёртку над IApplicationServer (протоколирование и ExecuteCommand)


#ifndef TOOLS_UI_WINDOWS_FORMS_SHARED_HEADER
#define TOOLS_UI_WINDOWS_FORMS_SHARED_HEADER

#include <stl/string>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>

#include <common/component.h>
#include <common/file.h>
#include <common/parser.h>
#include <common/strlib.h>

#include <script/shell.h>
#include <script/environment.h>
#include <script/bind.h>

#include <tools/ui/custom_window_system.h>

#using <mscorlib.dll>
#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>

#include <msclr/gcroot.h>
#include <msclr/event.h>

using namespace System;
using namespace System::Windows::Forms;

namespace tools
{

namespace ui
{

namespace windows_forms
{

typedef xtl::com_ptr<IApplicationServer> ApplicationServerPtr;

#include "event_handler.h"
#include "menu.h"
#include "form.h"
#include "control_registry.h"
#include "windows_system.h"
#include "configuration_parser.h"

}

}

}

#endif
