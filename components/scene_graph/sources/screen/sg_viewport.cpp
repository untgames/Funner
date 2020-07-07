#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации Viewport
*/

typedef stl::list<IViewportListener*> ListenerList;

struct Viewport::Impl: public xtl::reference_counter, public xtl::instance_counter<Viewport>
{
  stl::string          name;              //имя области вывода
  scene_graph::Camera* camera;            //камера
  stl::string          technique;         //имя техники рендеринга
  Rect                 rect;              //границы области вывода
  float                min_depth;         //минимальное значение глубины
  float                max_depth;         //максимальное значение глубины
  bool                 is_active;         //флаг активности области вывода  
  bool                 input_state;       //участвует ли область вывода в обработке ввода
  int                  z_order;           //порядок отрисовки области вывода
  math::vec4f          background_color;  //цвет фона
  bool                 has_background;    //наличие фона
  common::PropertyMap  properties;        //переменные рендеринга
  ListenerList         listeners;         //слушатели событий области вывода
  xtl::auto_connection on_destroy_camera; //слот соединения с сигналом оповещения об удалении камеры

  Impl () : camera (0), rect (0, 0, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT), min_depth (0.0f), max_depth (1.0f), is_active (true), input_state (true), z_order (INT_MAX), has_background (false)
  {
  }

  ~Impl ()
  {
    DestroyNotify ();
  }

  void SetCamera (scene_graph::Camera* in_camera)
  {
    if (camera == in_camera)
      return;
    
    on_destroy_camera.disconnect ();

    camera = in_camera;

    if (camera)
    {
      on_destroy_camera = camera->RegisterEventHandler (scene_graph::NodeEvent_AfterDestroy,
        xtl::bind (&Impl::SetCamera, this, (scene_graph::Camera*)0));
    }

    ChangeCameraNotify ();
  }

  template <class Fn>
  void Notify (Fn fn)
  {
    for (ListenerList::iterator iter=listeners.begin (); iter!=listeners.end ();)
    {
      ListenerList::iterator next = iter;
      
      ++next;
      
      try
      {
        fn (*iter);
      }
      catch (...)
      {
        //подавление всех исключений
      }
      
      iter = next;
    }    
  }
    
  void ChangeNameNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeName, _1, name.c_str ()));
  }
  
  void ChangeAreaNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeArea, _1, xtl::cref (rect), min_depth, max_depth));
  }
  
  void ChangeCameraNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeCamera, _1, camera));
  }
  
  void ChangeZOrderNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeZOrder, _1, z_order));
  }
  
  void ChangeActiveNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeActive, _1, is_active));
  }
  
  void ChangeInputStateNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeInputState, _1, input_state));
  }  
  
  void ChangeBackgroundNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeBackground, _1, has_background, xtl::cref (background_color)));
  }  
  
  void ChangeTechniqueNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeTechnique, _1, technique.c_str ()));
  }
  
  void ChangePropertiesNotify (const common::PropertyMap& properties)
  {
    Notify (xtl::bind (&IViewportListener::OnViewportChangeProperties, _1, properties));
  }  
  
  void DestroyNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnViewportDestroy, _1));    
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Viewport::Viewport ()
  : impl (new Impl)
{
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
  Viewport (vp).Swap (*this);
  
  return *this;
}

/*
    Имя области вывода
*/

void Viewport::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Viewport::SetName", "name");
    
  if (name == impl->name)
    return;
    
  impl->name = name;
  
  impl->ChangeNameNotify ();
}

const char* Viewport::Name () const
{
  return impl->name.c_str ();
}

/*
    Идентификатор области вывода
*/

size_t Viewport::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Техника рендеринга
*/

void Viewport::SetTechnique (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Viewport::SetTechnique", "name");
    
  if (impl->technique == name)
    return;
    
  impl->technique = name;
  
  impl->ChangeTechniqueNotify ();
}

const char* Viewport::Technique () const
{
  return impl->technique.c_str ();
}

/*
    Границы области вывода
*/

void Viewport::SetArea (const Rect& rect)
{
  if (impl->rect == rect)
    return;

  impl->rect = rect;

  impl->ChangeAreaNotify ();
}

void Viewport::SetArea (int left, int top, int width, int height)
{
  SetArea (Rect (left, top, width, height));
}

void Viewport::SetOrigin (int left, int top)
{
  Rect new_rect = impl->rect;
  
  new_rect.x = left;
  new_rect.y  = top;
  
  SetArea (new_rect);
}

void Viewport::SetSize (int width, int height)
{
  Rect new_rect = impl->rect;
  
  new_rect.width  = width;
  new_rect.height = height;

  SetArea (new_rect);
}

const Rect& Viewport::Area () const
{
  return impl->rect;
}

/*
    Область depth
*/

void Viewport::SetDepthRange (float min_depth, float max_depth)
{
  if (impl->min_depth == min_depth && impl->max_depth == max_depth)
    return;

  impl->min_depth = min_depth;
  impl->max_depth = max_depth;

  impl->ChangeAreaNotify ();  
}

void Viewport::SetMinDepth (float value)
{
  SetDepthRange (value, impl->max_depth);
}

void Viewport::SetMaxDepth (float value)
{
  SetDepthRange (impl->min_depth, value);
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
    Управление порядком отрисовки областей вывода (отрисовка от наибольшего номера по уменьшению номера)
*/

void Viewport::SetZOrder (int z_order)
{
  if (z_order == impl->z_order)
    return;

  impl->z_order = z_order;
  
  impl->ChangeZOrderNotify ();
}

int Viewport::ZOrder () const
{
  return impl->z_order;
}

/*
    Камера, связанная с областью вывода (политика владения - weak-reference)
*/

void Viewport::SetCamera (scene_graph::Camera* camera)
{
  impl->SetCamera (camera);
}

const scene_graph::Camera* Viewport::Camera () const
{
  return impl->camera;
}

scene_graph::Camera* Viewport::Camera ()
{
  return impl->camera;
}

/*
    Управление активностью области вывода (неакивные области не участвуют в отрисовке)
*/

void Viewport::SetActive (bool state)
{
  if (state == impl->is_active)
    return;

  impl->is_active = state;
  
  impl->ChangeActiveNotify ();
}

bool Viewport::IsActive () const
{
  return impl->is_active;
}

/*
    Участвует ли область вывода в обработке ввода
*/

void Viewport::SetInputState (bool state)
{
  if (state == impl->input_state)
    return;
    
  impl->input_state = state;
  
  impl->ChangeInputStateNotify ();
}

bool Viewport::InputState () const
{
  return impl->input_state;
}

/*
    Настройка фона
*/

void Viewport::SetBackgroundColor (const math::vec4f& color)
{
  if (impl->background_color == color)
    return;

  impl->background_color = color;
  
  impl->ChangeBackgroundNotify ();
}

void Viewport::SetBackgroundColor (float red, float green, float blue, float alpha)
{
  SetBackgroundColor (math::vec4f (red, green, blue, alpha));
}

const math::vec4f& Viewport::BackgroundColor () const
{
  return impl->background_color;
}

void Viewport::SetBackgroundState (bool state)
{
  if (state == impl->has_background)
    return;

  impl->has_background = state;

  impl->ChangeBackgroundNotify ();
}

bool Viewport::BackgroundState () const
{
  return impl->has_background;
}

/*
    Установка переменных рендеринга
*/

void Viewport::SetProperties (const common::PropertyMap& properties)
{
  if (impl->properties.Id () == properties.Id ())
    return;
    
  impl->properties = properties;
  
  impl->ChangePropertiesNotify (properties);
}

const common::PropertyMap& Viewport::Properties () const
{
  return impl->properties;
}

/*
    Работа со слушателями
*/

void Viewport::AttachListener (IViewportListener* listener) const
{
  if (!listener)
    throw xtl::make_null_argument_exception ("scene_graph::Viewport::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Viewport::DetachListener (IViewportListener* listener) const
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Viewport::DetachAllListeners () const
{
  impl->listeners.clear ();
}

/*
    Обмен
*/

void Viewport::Swap (Viewport& vp)
{
  stl::swap (impl, vp.impl);
}

namespace render
{
    
void swap (Viewport& vp1, Viewport& vp2)
{
  vp1.Swap (vp2);
}

}
