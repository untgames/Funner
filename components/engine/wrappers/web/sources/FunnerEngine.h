/**********************************************************\

  Auto-generated FunnerEngine.cpp

  This file contains the auto-generated main plugin object
  implementation for the Funner Engine project

\**********************************************************/
#ifndef H_FunnerEnginePLUGIN
#define H_FunnerEnginePLUGIN

#include "PluginWindow.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/AttachedEvent.h"

#include "PluginCore.h"

#ifdef CreateWindow
#undef CreateWindow
#endif

#include <memory>

#include <engine/engine.h>

class FunnerEngine : public FB::PluginCore
{
  public:
    FunnerEngine  ();
    ~FunnerEngine ();

  public:
    FB::JSAPI* createJSAPI ();
    bool IsWindowless () { return false; }

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *);
    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/

    static void StaticInitialize   ();
    static void StaticDeinitialize ();  

  private:
    std::auto_ptr<engine::IWindow> window;
};

#endif
