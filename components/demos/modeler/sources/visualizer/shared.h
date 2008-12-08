#ifndef MODELER_VISUALIZER_SHARED_HEADER
#define MODELER_VISUALIZER_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <stl/auto_ptr.h>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>
#include <xtl/shared_ptr.h>
#include <xtl/string.h>

#include <common/console.h>
#include <common/file.h>

#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/visual_model.h>

#include <media/rms/manager.h>

#include <render/low_level/device.h>
#include <render/low_level/utils.h>

#include <render/scene_render.h>

#include <engine/attachments.h>

class RenderableModel
{
  public:
    RenderableModel (render::low_level::IDevice& device, const char* file_name);
    ~RenderableModel ();

    void Draw (render::low_level::IDevice& device);

  private:
     RenderableModel (const RenderableModel&);
     RenderableModel& operator = (const RenderableModel&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

//проверка ошибок
const char* get_spawn_error_name ();

#endif
