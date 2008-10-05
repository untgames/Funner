#ifndef DEMOS_RENDER2D_SHARED_HEADER
#define DEMOS_RENDER2D_SHARED_HEADER

#include <cstdio>
#include <ctime>

#include <stl/auto_ptr.h>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/function.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>

#include <common/file.h>
#include <common/log.h>
#include <common/strlib.h>
#include <common/var_registry.h>

#include <sg/scene.h>
#include <sg/camera.h>
#include <sg/sprite.h>

#include <render/scene_render.h>

#include <render/mid_level/window_driver.h>

#include <syslib/application.h>
#include <syslib/window.h>

#include <sound/manager.h>
#include <sound/scene_player.h>

#include <script/bind_libraries.h>
#include <script/environment.h>
#include <script/bind.h>
#include <script/shell.h>

#include <input/low_level/device.h>
#include <input/low_level/driver.h>
#include <input/low_level/window_driver.h>

#include <input/translation_map.h>

using namespace script;
using namespace script::binds;
using namespace scene_graph;
using namespace render;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Общий код для работы тестов двумерного рендера
///////////////////////////////////////////////////////////////////////////////////////////////////
class TestApplication
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    TestApplication  (const char* start_script_name);
    ~TestApplication ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перерисовка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PostRedraw ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Запуск приложения
///////////////////////////////////////////////////////////////////////////////////////////////////
    int Run ();

  private:
    TestApplication (const TestApplication&); //no impl
    TestApplication& operator = (const TestApplication&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

#endif
