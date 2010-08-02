#ifdef _WIN32
  #include <windows.h>
#endif

#include <string>
#include <sstream>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"

#include "BrowserObjectAPI.h"
#include "variant_list.h"
#include "DOM/JSAPI_DOMDocument.h"
#include "NpapiTypes.h"

#include "FunnerEngineAPI.h"
#include "FunnerEngine.h"