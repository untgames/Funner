#ifndef TOOLS_UI_WINDOWS_FORMS_SHARED_HEADER
#define TOOLS_UI_WINDOWS_FORMS_SHARED_HEADER

#define _WIN32_WINNT 0x0600 //for CoInitializeEx
#include <cor.h>

#include <stl/string>
#include <stl/algorithm>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/implicit_cast.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>

#include <common/component.h>
#include <common/console.h>
#include <common/file.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/strlib.h>
#include <common/var_registry.h>

#include <syslib/application.h>

#include <script/shell.h>
#include <script/environment.h>
#include <script/bind.h>

#include <tools/ui/custom_window_system.h>

#using <mscorlib.dll>
#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>
#using <System.Xml.dll>
#using <WeifenLuo.WinFormsUI.Docking.dll>

#include <msclr/gcroot.h>
#include <msclr/event.h>

#pragma comment(linker,"-nodefaultlib:libcmt.lib")

using namespace System;

namespace tools
{

namespace ui
{

namespace windows_forms
{

typedef xtl::com_ptr<tools::ui::IApplicationServer> ApplicationServerPtr;

class WindowSystem;

#include "auto_string.h"
#include "exception.h"
#include "event_handler.h"
#include "menu.h"
#include "form.h"
#include "main_form.h"
#include "child_form.h"
#include "control_registry.h"
#include "plugin_manager.h"
#include "windows_system.h"
#include "configuration_parser.h"

String^ get_node_path (System::Xml::XmlNode^ node);

}

}

}

#endif
