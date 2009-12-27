#ifndef DEMO_WATER_SLIDES_SHARED_HEADER
#define DEMO_WATER_SLIDES_SHARED_HEADER

#include <time.h>
#include <stdlib.h>
#include <float.h>

#include <stl/auto_ptr.h>
//#include <stl/algorithm>
#include <stl/vector>

#include <xtl/connection.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/function.h>
#include <xtl/bind.h>

#include <common/file.h>
#include <common/streams.h>
#include <common/strlib.h>
#include <common/log.h>
#include <common/time.h>
#include <common/parser.h>

#include <math/matrix.h>
#include <math/utility.h>

#include <syslib/window.h>
#include <syslib/application.h>
#include <syslib/timer.h>

#include <media/image.h>

#include <render/low_level/driver.h>
#include <render/low_level/device.h>
#include <render/low_level/utils.h>

#include <sg/scene.h>
#include <sg/listener.h>
#include <sg/sound_emitter.h>

#include <sound/scene_player.h>

typedef xtl::com_ptr<render::low_level::IDriver>                  DriverPtr;
typedef xtl::com_ptr<render::low_level::ISwapChain>               SwapChainPtr;
typedef xtl::com_ptr<render::low_level::IDevice>                  DevicePtr;
typedef xtl::com_ptr<render::low_level::ITexture>                 TexturePtr;
typedef xtl::com_ptr<render::low_level::ISamplerState>            SamplerStatePtr;
typedef xtl::com_ptr<render::low_level::IView>                    ViewPtr;
typedef xtl::com_ptr<render::low_level::IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<render::low_level::IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<render::low_level::IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<render::low_level::IBuffer>                  BufferPtr;
typedef xtl::com_ptr<render::low_level::IRasterizerState>         RasterizerStatePtr;
typedef xtl::com_ptr<render::low_level::IProgram>                 ProgramPtr;
typedef xtl::com_ptr<render::low_level::IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<render::low_level::IPredicate>               PredicatePtr;
typedef scene_graph::Listener::Pointer                            ListenerPtr;
typedef scene_graph::SoundEmitter::Pointer                        SoundEmitterPtr;

//интерфейс игрового отображения
class IGameView: public xtl::reference_counter
{
  public:
    virtual ~IGameView () {}
    
      //виртуальные размеры отображения
    virtual size_t Width  () { return 100; }
    virtual size_t Height () { return 100; }    
    
      //создание ресурсов
    virtual void LoadResources (sound::ScenePlayer*, render::low_level::IDevice&) {}
    virtual void FlushResources () {}
    
      //обработчики событий
    virtual void OnDraw  () {}
    virtual void OnIdle  () {}
    virtual void OnMouse (syslib::WindowEvent event, int x, int y) {}
};

typedef xtl::intrusive_ptr<IGameView> GameView;

//класс конфигурационного файла
class Configuration
{
  public:
    Configuration (const char* file_name);
    ~Configuration ();

    int         GetInteger (const char* property_name, int default_value = 0) const;
    const char* GetString  (const char* property_name, const char* default_value = "") const;

    size_t      LogMessagesCount () const;
    const char* LogMessage       (size_t index) const;

  private:
    Configuration ();
    Configuration (const Configuration&); //no impl
    Configuration& operator = (const Configuration&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

//класс приложения
class MyApplication
{
  public:
      //получение количества милисекунд, прошедших от момента запуска приложения
    static size_t Milliseconds ();  

      //протоколирование
    void LogMessage        (const char* message);
    void LogFormatMessage  (const char* format, ...);
    void VLogFormatMessage (const char* format, va_list list);    
    
      //установка текущего отображения
    void            SetView (const GameView&);
    const GameView& View    () const;    

      //конструкторы / деструктор / присваивание
    MyApplication  ();
    ~MyApplication ();

  private:    
    MyApplication  (const MyApplication&); // no impl
    MyApplication& operator = (const MyApplication&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

//получение ортографической матрицы проекции
math::mat4f get_ortho_proj (float left, float right, float bottom, float top, float znear, float zfar);

//получение перспективной матрицы проекции
math::mat4f get_perspective_proj (const math::anglef& fov_x, const math::anglef& fov_y, float znear, float zfar);

//создание игровых отображений
GameView create_test_game_view ();

#endif
