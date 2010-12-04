#include <stdexcept>
#include <cstdio>
#include <memory.h>
#include <cstring>

#include "NpapiTypes.h"
#include "APITypes.h"
#include "AutoPtr.h"
#include "NpapiPluginModule.h"
#include "NPJavascriptObject.h"
#include "NPObjectAPI.h"
#include "DOM/JSAPI_DOMDocument.h"
#include "DOM/JSAPI_DOMWindow.h"
#include "variant_list.h"

#include "NpapiStream.h"
#include "NpapiBrowserHost.h"

#include "NpapiPlugin.h"
#include "NpapiStream.h"
#include "FactoryDefinitions.h"
#include "PluginCore.h"
#include "BrowserHostWrapper.h"
#include "config.h"

#ifdef _WINDOWS_
#  include "Win/NpapiBrowserHostAsyncWin.h"
#endif

#include "NPJavascriptObject.h"
#include "BrowserObjectAPI.h"
#include "NPObjectAPI.h"
#include "NpapiBrowserHost.h"
