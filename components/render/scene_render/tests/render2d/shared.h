#ifndef SCENE_RENDER_RENDER2D_TESTS_SHARED_HEADER
#define SCENE_RENDER_RENDER2D_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <xtl/function.h>
#include <xtl/connection.h>

#include <common/component.h>
#include <common/log.h>

#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/sprite.h>
#include <sg/text_line.h>

#include <render/scene_render.h>

using namespace render;
using namespace scene_graph;

void print (const char*, const char* message)
{
  printf ("%s\n", message);
}

void log_print (const char* message)
{
  printf ("%s\n", message);
}

#endif
