/**********************************************************\

  Auto-generated FunnerEngineAPI.cpp

\**********************************************************/

#include "BrowserObjectAPI.h"
#include "variant_list.h"
#include "DOM/JSAPI_DOMDocument.h"

#include "FunnerEngineAPI.h"

FunnerEngineAPI::FunnerEngineAPI(FB::BrowserHostWrapper *host) : m_host(host)
{
    registerMethod("echo",      make_method(this, &FunnerEngineAPI::echo));
    registerMethod("testEvent", make_method(this, &FunnerEngineAPI::testEvent));

    // Read-write property
    registerProperty("testString",
                     make_property(this,
                        &FunnerEngineAPI::get_testString,
                        &FunnerEngineAPI::set_testString));

    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &FunnerEngineAPI::get_version));
    
    
    registerEvent("onfired");    
}

FunnerEngineAPI::~FunnerEngineAPI()
{
}

// Read/Write property testString
std::string FunnerEngineAPI::get_testString()
{
    return m_testString;
}
void FunnerEngineAPI::set_testString(const std::string& val)
{
    m_testString = val;
}

// Read-only property version
std::string FunnerEngineAPI::get_version()
{
    return "CURRENT_VERSION";
}

// Method echo
FB::variant FunnerEngineAPI::echo(const FB::variant& msg)
{
    return msg;
}

void FunnerEngineAPI::testEvent(const FB::variant& var)
{
    FireEvent("onfired", FB::variant_list_of(var));
}

