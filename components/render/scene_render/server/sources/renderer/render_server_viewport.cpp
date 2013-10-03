#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    ќписание реализации области вывода
*/

namespace
{

struct RenderTargetEntry
{
  RenderTargetDesc desc;
  bool             need_update;

  RenderTargetEntry (const RenderTargetDesc& in_desc) : desc (in_desc), need_update (true) {}
};

typedef stl::hash_map<const RenderTargetDesc*, RenderTargetEntry> RenderTargetEntryMap;
typedef stl::vector<IViewportListener*>                           ListenerArray;

}

struct Viewport::Impl: public xtl::reference_counter, public IRenderTargetMapListener
{
  RenderTargetMap      render_targets;             //цели рендеринга
  RenderTargetEntryMap render_target_entries;      //дескрипторы целей рендеринга
  stl::string          name;                       //им€ области вывода
  Rect                 area;                       //область вывода
  float                min_depth;                  //минимальна€ глубина
  float                max_depth;                  //максимальна€ глубина
  int                  zorder;                     //пор€док вывода
  bool                 is_active;                  //активна ли область отрисовки
  manager::Frame       frame;                      //присоединенный кадр
  math::vec4f          background_color;           //цвет фона
  bool                 background_state;           //состо€ние фона
  ListenerArray        listeners;                  //слушатели событий области вывода
  bool                 need_reconfiguration;       //конфигураци€ изменена
  bool                 need_update_area;           //обновилась область
  bool                 need_update_render_targets; //требуетс€ обновить буферы отрисовки
  bool                 need_update_background;     //требуетс€ обновить параметры очистки
  bool                 need_update_camera;         //требуетс€ обновить камеру
  bool                 need_update_properties;     //требуетс€ обновление свойств

///  онструктор
  Impl (RenderManager& render_manager, const RenderTargetMap& in_render_targets)
    : render_targets (in_render_targets)
    , area (0, 0, 100, 100)
    , min_depth (0.0f)
    , max_depth (1.0f)
    , zorder (0)
    , is_active (true)
    , frame (render_manager.Manager ().CreateFrame ())
    , background_state (false)
    , need_reconfiguration (true)
    , need_update_area (true)
    , need_update_render_targets (true)
    , need_update_background (true)
    , need_update_camera (true)
    , need_update_properties (true)
  {
    render_targets.AttachListener (this);
  }

/// ƒеструктор
  ~Impl ()
  {
    render_targets.DetachListener (this);
  }

/// ќповещение о добавлении новой области вывода
  void OnRenderTargetAdded (const RenderTargetDesc& desc)
  {
    RenderTargetEntryMap::iterator iter = render_target_entries.find (&desc);

    if (iter != render_target_entries.end ())
      return;

    render_target_entries.insert_pair (&desc, RenderTargetEntry (desc));

    need_update_render_targets = true;
    need_reconfiguration       = true;
  }

/// ќповещение об изменении области вывода
  void OnRenderTargetUpdated (const RenderTargetDesc& desc)
  {
    RenderTargetEntryMap::iterator iter = render_target_entries.find (&desc);

    if (iter == render_target_entries.end ())
      return;

    iter->second.need_update   = true;    
    need_update_render_targets = true;
    need_reconfiguration       = true;
  }

/// ќповещение об удалении области вывода
  void OnRenderTargetRemoved (const RenderTargetDesc& desc)
  {
    render_target_entries.erase (&desc);

    need_update_render_targets = true;
    need_reconfiguration       = true;    
  }
};

/*
     онструкторы / деструктор / присваивание
*/

Viewport::Viewport (RenderManager& render_manager, const RenderTargetMap& render_target_map)
{
  try
  {
    impl = new Impl (render_manager, render_target_map);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Viewport::Viewport");
    throw;
  }
}

Viewport::Viewport (const Viewport& vp)
  : impl (vp.impl)
{
  addref (impl);
}

Viewport::~Viewport ()
{
  release (impl);
}

Viewport& Viewport::operator = (const Viewport& vp)
{
  Viewport tmp (vp);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    »м€ области вывода
*/

void Viewport::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Viewport::SetName", "name");

  impl->name = name;
}

const char* Viewport::Name () const
{
  return impl->name.c_str ();
}

/*
    ќбласть
*/

void Viewport::SetArea (const Rect& rect)
{
  impl->area                 = rect;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

const Rect& Viewport::Area () const
{
  return impl->area;
}

/*
    ƒиапазон глубины дл€ области вывода
*/

void Viewport::SetMinDepth (float value)
{
  impl->min_depth            = value;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

void Viewport::SetMaxDepth (float value)
{
  impl->max_depth            = value;
  impl->need_update_area     = true;
  impl->need_reconfiguration = true;
}

float Viewport::MinDepth () const
{
  return impl->min_depth;
}

float Viewport::MaxDepth () const
{
  return impl->max_depth;
}

/*
    јктивность области вывода
*/

void Viewport::SetActive (bool active)
{
  impl->is_active = active;
}

bool Viewport::IsActive () const
{
  return impl->is_active;
}

/*
    ѕор€док вывода
*/

void Viewport::SetZOrder (int order)
{
  impl->zorder = true;

  for (ListenerArray::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnViewportZOrderUpdated (order);
    }
    catch (...)
    {
    }
  }
}

int Viewport::ZOrder () const
{
  return impl->zorder;
}

/*
    «адний фон
*/

void Viewport::SetBackground (bool state, const math::vec4f& color)
{
  impl->background_state       = state;
  impl->background_color       = color;
  impl->need_update_background = true;
  impl->need_reconfiguration   = true;
}

bool Viewport::BackgroundState () const
{
  return impl->background_state;
}

const math::vec4f& Viewport::BackgroundColor () const
{
  return impl->background_color;
}

/*
    “ехника отрисовки
*/

void Viewport::SetTechnique (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

const char* Viewport::Technique () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    —войства отрисовки
*/

void Viewport::SetProperties (const common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

const common::PropertyMap& Viewport::Properties () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ѕрисоединенный кадр
*/

manager::Frame& Viewport::Frame () const
{
  return impl->frame;
}

/*
    ќбновление
*/

void Viewport::Update (manager::Frame* parent_frame)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    ƒобавление слушателей
*/

void Viewport::AttachListener (IViewportListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::scene::server::Viewport::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Viewport::DetachListener (IViewportListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Viewport::DetachAllListeners ()
{
  impl->listeners.clear ();
}
