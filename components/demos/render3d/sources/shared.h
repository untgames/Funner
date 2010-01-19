#ifndef RENDER_GL_DRIVER_COMPLEX_TESTS_SHARED_HEADER
#define RENDER_GL_DRIVER_COMPLEX_TESTS_SHARED_HEADER

#include <cstdio>
#include <cmath>
#include <exception>

#include <float.h>

#include <math/matrix.h>
#include <math/utility.h>

#include <syslib/window.h>
#include <syslib/application.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/debug.h>
#include <render/low_level/utils.h>

#include <stl/string>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>
#include <xtl/shared_ptr.h>
#include <xtl/uninitialized_storage.h>
#include <xtl/visitor.h>

#include <common/file.h>
#include <common/hash.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/singleton.h>
#include <common/string.h>
#include <common/strlib.h>
#include <common/time.h>

#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/visual_model.h>

#include <media/image.h>
#include <media/mesh.h>
#include <media/rfx/material_library.h>

using namespace render::low_level;
using namespace scene_graph;

/*
    Получение смещения поля в структуре (аналог offsetof, компилируемый без предупреждений на gcc)
*/

#ifdef _MSC_VER
  #define TEST_OFFSETOF(X,Y) offsetof(X,Y)
#else
  #define TEST_OFFSETOF(X,Y) (reinterpret_cast<size_t> (&(static_cast<X*> (0)->*(&X::Y))))
#endif

typedef xtl::com_ptr<IDriver>                  DriverPtr;
typedef xtl::com_ptr<ISwapChain>               SwapChainPtr;
typedef xtl::com_ptr<IDevice>                  DevicePtr;
typedef xtl::com_ptr<ITexture>                 TexturePtr;
typedef xtl::com_ptr<ISamplerState>            SamplerStatePtr;
typedef xtl::com_ptr<IView>                    ViewPtr;
typedef xtl::com_ptr<IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<IBuffer>                  BufferPtr;
typedef xtl::com_ptr<IRasterizerState>         RasterizerStatePtr;
typedef xtl::com_ptr<IProgram>                 ProgramPtr;
typedef xtl::com_ptr<IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<IPredicate>               PredicatePtr;

//тестовое приложение
struct Test
{
  public:
    typedef xtl::function<void (Test&)> CallbackFn;

    syslib::Window  window;
    SwapChainPtr    swap_chain;
    DevicePtr       device;
    CallbackFn      redraw;
    CallbackFn      reload;
    ProgramPtr      shader;
    Scene           scene;
    Camera::Pointer camera;
    float           x_camera_speed;
    float           y_camera_speed;
    float           x_camera_rotation_speed;
    float           y_camera_rotation_speed;
    bool            pressed_keys [syslib::Key_Num];

    Test (const wchar_t* title, const CallbackFn& in_redraw, const CallbackFn& in_reload, const char* adapter_mask="*", const char* init_string="");

  private:
    void OnKeyPressed  (const syslib::WindowEventContext& context);
    void OnKeyReleased (const syslib::WindowEventContext& context);
    void OnResize ();
    void OnRedraw ();
    void OnClose ();
    void OnCameraUpdate ();
};

//обновление матрицы проецирования
void update_proj_matrix (DevicePtr device, const math::mat4f& proj_matrix);
//обновление матрицы вида
void update_view_matrix (DevicePtr device, const math::mat4f& view_matrix);

//чтение ихсодного текста шейдера в строку
stl::string read_shader (const char* file_name);

#endif
