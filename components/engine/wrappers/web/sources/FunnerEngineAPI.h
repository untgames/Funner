/**********************************************************\

  Auto-generated FunnerEngineAPI.h

\**********************************************************/

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHostWrapper.h"

#ifndef H_FunnerEngineAPI
#define H_FunnerEngineAPI

class FunnerEngineAPI : public FB::JSAPIAuto
{
public:
    FunnerEngineAPI(FB::BrowserHostWrapper *host);
    virtual ~FunnerEngineAPI();

    // Read/Write property ${PROPERTY.ident}
    std::string get_testString();
    void set_testString(const std::string& val);

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);
    
    // Method test-event
    void testEvent(const FB::variant& s);

private:
    FB::AutoPtr<FB::BrowserHostWrapper> m_host;

    std::string m_testString;
};

#endif // H_FunnerEngineAPI
