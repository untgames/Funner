#include <stl/auto_ptr.h>

#include <render/low_level/device.h>
#include <render/low_level/utils.h>

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
