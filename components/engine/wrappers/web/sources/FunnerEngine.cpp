/**********************************************************\

  Auto-generated FunnerEngine.cpp

  This file contains the auto-generated main plugin object
  implementation for the Funner Engine project

\**********************************************************/

#include "NpapiTypes.h"
#include "FunnerEngineAPI.h"

#include "FunnerEngine.h"

void FunnerEngine::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void FunnerEngine::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


FunnerEngine::FunnerEngine()
{
}

FunnerEngine::~FunnerEngine()
{
}

FB::JSAPI* FunnerEngine::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    return new FunnerEngineAPI(m_host);
}

bool FunnerEngine::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool FunnerEngine::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}

bool FunnerEngine::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    //printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
    return false;
}
bool FunnerEngine::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool FunnerEngine::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}
