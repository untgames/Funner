#include <cstdio>
#include <exception>

#include <xtl/any_reference.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <common/log.h>

#include <sg/scene.h>
#include <sg/physics.h>

using namespace scene_graph;
using namespace scene_graph::physics;

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

