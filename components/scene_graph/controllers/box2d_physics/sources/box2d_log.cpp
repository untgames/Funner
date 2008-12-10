#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::physics;
using namespace scene_graph::physics::box2d;

namespace
{

/*
    Константы
*/

const char* LOG_NAME = "scene_graph.controllers.box2d";

}

/*
    Конструктор
*/

Log::Log ()
  : log (LOG_NAME)
{  
}

/*
    Печать сообщений
*/

void Log::Print (const char* message) const
{
  log.Print (message);
}

void Log::Printf  (const char* message, ...) const
{
  va_list list;
  
  va_start (list, message);  
  VPrintf  (message, list);
}

void Log::VPrintf (const char* message, va_list list) const
{
  log.VPrintf (message, list);
}
