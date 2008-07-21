#include "shared.h"

using namespace render;

/*
     онстанты
*/

namespace
{

const size_t LISTENER_ARRAY_RESERVE_SIZE = 16; //резервируемый размер массива слушателей

/*
    ѕеременна€ ренедринга
*/

struct RenderProperty: public IViewportProperty, public xtl::reference_counter
{
  stl::string name, value;

  const char* Name  () const { return name.c_str (); }
  const char* Value () const { return value.c_str (); }

  RenderProperty (const char* in_name, const char* in_value) : name (in_name), value (in_value) {}
};

}

/*
    ќписание реализации Viewport
*/

typedef xtl::com_ptr<IViewportListener>                        ListenerPtr;
typedef stl::vector<ListenerPtr>                               ListenerArray;
typedef xtl::intrusive_ptr<RenderProperty>                     PropertyPtr;
typedef stl::hash_map<stl::hash_key<const char*>, PropertyPtr> PropertyMap;

struct Viewport::Impl: public xtl::reference_counter
{
  stl::string          name;              //им€ области вывода
  scene_graph::Camera* camera;            //камера
  stl::string          path_name;         //им€ пути рендеринга
  Rect                 rect;              //границы области вывода
  bool                 is_active;         //флаг активности области вывода
  int                  z_order;           //пор€док отрисовки области вывода
  PropertyMap          properties;        //переменные рендеринга
  ListenerArray        listeners;         //слушатели событий области вывода
  xtl::auto_connection on_destroy_camera; //слот соединени€ с сигналом оповещени€ об удалении камеры

  Impl () : camera (0), is_active (true), z_order (INT_MAX)
  {
    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
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

    ChangeCameraNotify (camera);
  }

  template <class Fn>
  void Notify (Fn fn)
  {
    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
    {
      try
      {
        fn (*iter);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }    
  }
    
  void ChangeNameNotify (const char* new_name)
  {
    Notify (xtl::bind (&IViewportListener::OnChangeName, _1, new_name));
  }
  
  void ChangeAreaNotify (const Rect& new_area)
  {
    Notify (xtl::bind (&IViewportListener::OnChangeArea, _1, xtl::cref (new_area)));
  }
  
  void ChangeCameraNotify (scene_graph::Camera* new_camera)
  {
    Notify (xtl::bind (&IViewportListener::OnChangeCamera, _1, new_camera));
  }
  
  void ChangeZOrderNotify (int new_z_order)
  {
    Notify (xtl::bind (&IViewportListener::OnChangeZOrder, _1, new_z_order));
  }
  
  void ChangeActiveNotify (bool new_state)
  {
    Notify (xtl::bind (&IViewportListener::OnChangeActive, _1, new_state));
  }
  
  void ChangeRenderPathNotify (const char* new_path_name)
  {
    Notify (xtl::bind (&IViewportListener::OnChangeRenderPath, _1, new_path_name));
  }
  
  void ChangePropertyNotify (const char* name, const char* new_value)
  {
    Notify (xtl::bind (&IViewportListener::OnChangeProperty, _1, name, new_value));
  }  
  
  void DestroyNotify ()
  {
    Notify (xtl::bind (&IViewportListener::OnDestroy, _1));    
  }
};

/*
     онструкторы / деструктор / присваивание
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
    »м€ области вывода
*/

void Viewport::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::Viewport::SetName", "name");
    
  if (name == impl->name)
    return;
    
  impl->name = name;
  
  impl->ChangeNameNotify (impl->name.c_str ());
}

const char* Viewport::Name () const
{
  return impl->name.c_str ();
}

/*
    »дентификатор области вывода
*/

size_t Viewport::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    ”становка пути рендеринга содержимого области вывода
*/

void Viewport::SetRenderPath (const char* path_name)
{
  if (!path_name)
    throw xtl::make_null_argument_exception ("render::Viewport::SetRenderPath", "path_name");
    
  if (impl->path_name == path_name)
    return;
    
  impl->path_name = path_name;
  
  impl->ChangeRenderPathNotify (impl->path_name.c_str ());
}

const char* Viewport::RenderPath () const
{
  return impl->path_name.c_str ();
}

/*
    √раницы области вывода
*/

void Viewport::SetArea (const Rect& rect)
{
  if (impl->rect.left == rect.left && impl->rect.top == rect.top && impl->rect.width == rect.width && impl->rect.height == rect.height)
    return;

  impl->rect = rect;

  impl->ChangeAreaNotify (impl->rect);
}

void Viewport::SetArea (int left, int top, size_t width, size_t height)
{
  SetArea (Rect (left, top, width, height));
}

void Viewport::SetOrigin (int left, int top)
{
  Rect new_rect = impl->rect;
  
  new_rect.left = left;
  new_rect.top  = top;
  
  SetArea (new_rect);
}

void Viewport::SetSize (size_t width, size_t height)
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
    ”правление пор€дком отрисовки областей вывода (отрисовка от наименьших номеров по увеличению номера)
*/

void Viewport::SetZOrder (int z_order)
{
  if (z_order == impl->z_order)
    return;

  impl->z_order = z_order;
  
  impl->ChangeZOrderNotify (impl->z_order);
}

int Viewport::ZOrder () const
{
  return impl->z_order;
}

/*
     амера, св€занна€ с областью вывода (политика владени€ - weak-reference)
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
    ”правление активностью области вывода (неакивные области не участвуют в отрисовке)
*/

void Viewport::SetActive (bool state)
{
  if (state == impl->is_active)
    return;

  impl->is_active = state;
  
  impl->ChangeActiveNotify (impl->is_active);
}

bool Viewport::IsActive () const
{
  return impl->is_active;
}

/*
    ”становка переменных рендеринга
*/

void Viewport::SetProperty (const char* name, const char* value)
{
  static const char* METHOD_NAME = "render::Viewport::SetProperty";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  if (!value)
    throw xtl::make_null_argument_exception (METHOD_NAME, "value");
    
  PropertyMap::iterator iter = impl->properties.find (name);
  
  if (iter != impl->properties.end ())
  {
    if (iter->second->value == value)
      return;

    iter->second->value = value;
  }
  else
  {
    impl->properties.insert_pair (name, PropertyPtr (new RenderProperty (name, value), false));
  }
  
  impl->ChangePropertyNotify (name, value);
}

const char* Viewport::GetProperty (const char* name) const
{
  if (!name)
    return "";

  PropertyMap::iterator iter = impl->properties.find (name);

  if (iter == impl->properties.end ())
    return "";

  return iter->second->value.c_str ();
}

bool Viewport::HasProperty (const char* name) const
{
  if (!name)
    return false;

  PropertyMap::iterator iter = impl->properties.find (name);

  return iter != impl->properties.end ();
}

void Viewport::RemoveProperty (const char* name)
{
  if (!name)
    return;
    
  impl->properties.erase (name);
}

void Viewport::RemoveAllProperties ()
{  
  impl->properties.clear ();
}

/*
    ѕеречисление переменных рендеринга
*/

Viewport::PropertyIterator Viewport::CreatePropertyIterator () const
{
  struct PropertySelector
  {
    const IViewportProperty& operator () (const PropertyMap::value_type& value) const { return *value.second; }
  };

  return PropertyIterator (impl->properties.begin (), impl->properties.begin (), impl->properties.end (), PropertySelector ());
}

/*
    –абота со слушател€ми
*/

void Viewport::AttachListener (IViewportListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::Viewport::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Viewport::DetachListener (IViewportListener* listener)
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Viewport::DetachAllListeners ()
{
  impl->listeners.clear ();
}

/*
    ќбмен
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
