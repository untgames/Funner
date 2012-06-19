#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
    Описание реализации менеджера ввода сцены
*/

typedef stl::vector<InputPortPtr> InputPortList;
typedef stl::vector<const char*>  TokenList;

struct SceneInputManager::Impl: public xtl::reference_counter, public IScreenListener
{
  scene_graph::Screen* screen;              //присоединенный экран
  InputPortList        input_ports;         //области ввода
  bool                 need_reorder;        //требуется сортировка областей ввода
  stl::string          event_tokens_buffer; //буфер токенов
  TokenList            event_tokens;        //токены
  math::vec2f          mouse_position;      //положение курсора мыши
  math::vec2f          screen_size;         //размеры экрана
  math::vec2f          screen_offset;       //смещение от начала экрана
  float                touch_size;          //размер тача
  InputTransformSpace  touch_size_space;    //система координат размеров тача
  
///Конструктор
  Impl ()
    : screen ()
    , need_reorder (false)
    , touch_size (1.0f)
    , touch_size_space (InputTransformSpace_Default)
  {
    static const size_t RESERVE_TOKENS_COUNT  = 16;
    static const size_t RESERVE_TOKENS_BUFFER = 256;
    
    event_tokens.reserve (RESERVE_TOKENS_COUNT);
    event_tokens_buffer.reserve (RESERVE_TOKENS_BUFFER);
  }
  
///Деструктор
  ~Impl ()
  {
    SetScreen (0);
  }
  
///Изменение экрана
  void SetScreen (scene_graph::Screen* in_screen)
  {
    if (in_screen == screen)
      return;
    
    if (screen)
    {
        //отсоединение текущего экрана
       
      input_ports.clear ();
      
      need_reorder = false;
      
      screen->DetachListener (this);
      
      screen = 0;
    }
    
      //присоединение нового экрана
      
    if (in_screen)
    {
      try
      {
        screen = in_screen;
        
        OnScreenChangeArea (screen->Area ());
        
        screen->AttachListener (this);        
        
        for (size_t i=0, count=screen->ViewportsCount (); i<count; i++)
          OnScreenAttachViewport (screen->Viewport (i));
      }
      catch (...)
      {
        screen->DetachListener (this);
        
        input_ports.clear ();
        
        need_reorder = false;
        
        screen = 0;
        
        throw;
      }
    }
  }
  
///Изменение размеров экрана  
  void OnScreenChangeArea (const Rect& rect)
  {
    screen_size   = math::vec2f (float (rect.right () - rect.left ()), float (rect.bottom () - rect.top ()));
    screen_offset = math::vec2f (float (rect.left ()), float (rect.top ()));
  }  
  
///Присоединена область вывода
  void OnScreenAttachViewport (Viewport& viewport)
  {
    try
    {
      InputPortPtr input_port (new InputPort (viewport, need_reorder), false);
      
      input_port->SetTouchSize (touch_size, touch_size_space);

      input_ports.push_back (input_port);
      
      need_reorder = true;
    }
    catch (xtl::exception& e)
    {
      e.touch ("input::SceneInputManager::Impl::OnScreenAttachViewport");
      throw;
    }
  }

///Отсоединена область вывода
  void OnScreenDetachViewport (Viewport& viewport)
  {
    for (InputPortList::iterator iter=input_ports.begin (), end=input_ports.end (); iter!=end; ++iter)
      if (&(*iter)->AttachedViewport () == &viewport)
      {
        input_ports.erase (iter);
        return;
      }
  }

///Экран удален
  void OnScreenDestroy ()
  {
    SetScreen (0);
  }

///Сортировка областей ввода
  struct InputPortComparator
  {
    bool operator () (const InputPortPtr& port1, const InputPortPtr& port2) const
    {
      return port1->AttachedViewport ().ZOrder () < port2->AttachedViewport ().ZOrder ();
    }
  };    

  void Sort ()
  {
    if (!need_reorder)
      return;
      
    stl::sort (input_ports.begin (), input_ports.end (), InputPortComparator ());      
      
    need_reorder = false;
  }
  
  void TokenizeInputEvent (const char* event)
  {
    event_tokens_buffer = event;
    
    event_tokens.clear ();
    
    char* s = &event_tokens_buffer [0];
    
    for (;;)
    {    
      bool loop = true;
      
      for (;loop;)
      {
        switch (*s)
        {
          case ' ':
          case '\t':
            s++;
            break;
          default:
            loop = false;
            break;
        }
      }
      
      const char *first = s, *last = s;

      if (*s)
      {
        bool loop = true;
        
        for (++s;loop;)
        {
          switch (*s)
          {
            case ' ':
            case '\t':
            case '\0':
              loop = false;
              break;
            default:
              s++;
              break;
          }
        }
        
        last = s;
      }
      
      if (first != last)      
        event_tokens.push_back (first);

      if (!*s)
        break;
        
      *s++ = '\0';
    }    
  }
  
  bool ParseInputEvent (const char* event_string, TouchEvent& event)
  {
    TokenizeInputEvent (event_string);

    if (event_tokens.size () >= 6 && !strcmp (event_tokens [0], "Touch"))
    {
      event.touch    = xtl::io::get<size_t> (event_tokens [3]);
      event.position = math::vec2f (xtl::io::get<float> (event_tokens [4]), xtl::io::get<float> (event_tokens [5])) * screen_size + screen_offset;
      event.button   = 0;

      if      (!strcmp (event_tokens [1], "began")) event.state = TouchState_Pressed;
      else if (!strcmp (event_tokens [1], "moved")) event.state = TouchState_Moving;
      else if (!strcmp (event_tokens [1], "ended")) event.state = TouchState_Released;
      else                                          return false;

      return true;
    }
    else if (event_tokens.size () >= 3 && !strcmp (event_tokens [1], "axis"))
    {
      if      (!strcmp (event_tokens [0], "CursorX")) mouse_position [0] = (xtl::io::get<float> (event_tokens [2]) + 1.0f) * 0.5f;
      else if (!strcmp (event_tokens [0], "CursorY")) mouse_position [1] = (xtl::io::get<float> (event_tokens [2]) + 1.0f) * 0.5f;
      else                                            return false;      

      event.touch    = MOUSE_TOUCH_ID;
      event.state    = TouchState_Moving;
      event.position = mouse_position * screen_size + screen_offset;
      event.button   = 0;
      
      return true;
    }
    else if (event_tokens.size () >= 2 && !strncmp (event_tokens [0], "Mouse", 5))
    {
      event.touch    = MOUSE_TOUCH_ID;      
      event.button   = xtl::io::get<unsigned int> (event_tokens [0] + 5);
      event.position = mouse_position * screen_size + screen_offset;

      if      (!strcmp (event_tokens [1], "down")) event.state = TouchState_Pressed;
      else if (!strcmp (event_tokens [1], "up"))   event.state = TouchState_Released;
      else                                         return false;
      
      return true;
    }    

    return false;    
  }
  
///Обработка события ввода
  void ProcessInputEvent (const char* event_string)
  {
    if (!screen)
      return;      

    TouchEvent event;
    
    if (!ParseInputEvent (event_string, event))
      return;

    if (need_reorder)
      Sort ();      

    bool touch_catched = false;
    
    math::vec2f position = event.position;

    for (InputPortList::iterator iter=input_ports.begin (), end=input_ports.end (); iter!=end; ++iter)
      (*iter)->OnTouch (event, touch_catched);
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

SceneInputManager::SceneInputManager ()
{
  try
  {
    impl = new Impl;
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::SceneInputManager::SceneInputManager");
    throw;
  }  
}

SceneInputManager::SceneInputManager (const SceneInputManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

SceneInputManager::~SceneInputManager ()
{
  release (impl);
}

SceneInputManager& SceneInputManager::operator = (const SceneInputManager& manager)
{
  SceneInputManager (manager).Swap (*this);

  return *this;
}

/*
    Присоединенный экран
*/

Screen* SceneInputManager::Screen ()
{
  return impl->screen;
}

const Screen* SceneInputManager::Screen () const
{
  return impl->screen;
}

void SceneInputManager::SetScreen (scene_graph::Screen* screen)
{
  impl->SetScreen (screen);
}

/*
    Обработка события ввода
*/

void SceneInputManager::ProcessEvent (const char* event)
{
  try
  {
    if (!event)
      throw xtl::make_null_argument_exception ("", "event");
      
    impl->ProcessInputEvent (event);
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::SceneInputManager::ProcessEvent");
    throw;
  }
}

/*
    Сброс состояния обработки ввода
*/

void SceneInputManager::Reset ()
{
  try
  {
    for (InputPortList::iterator iter=impl->input_ports.begin (), end=impl->input_ports.end (); iter!=end; ++iter)
      (*iter)->ResetTouchState ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("input::SceneInputManager::Reset");
    throw;
  }
}

/*
    Размер тача
*/

void SceneInputManager::SetTouchSize (float size, InputTransformSpace space)
{
  static const char* METHOD_NAME = "input::SceneInputManager::SetTouchSize";

  if (size <= 0.0f)
    throw xtl::make_argument_exception (METHOD_NAME, "size", size, "Size must be greater than zero");

  switch (space)
  {
    case InputTransformSpace_Screen:
    case InputTransformSpace_Camera:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "space", space);
  }

  impl->touch_size = size;
  
  for (InputPortList::iterator iter=impl->input_ports.begin (), end=impl->input_ports.end (); iter!=end; ++iter)
    (*iter)->SetTouchSize (size, space);
}

float SceneInputManager::TouchSize () const
{
  return impl->touch_size;
}

InputTransformSpace SceneInputManager::TouchSizeSpace () const
{
  return impl->touch_size_space;
}

/*
    Обмен
*/

void SceneInputManager::Swap (SceneInputManager& manager)
{
  stl::swap (impl, manager.impl);
}

namespace input
{

void swap (SceneInputManager& manager1, SceneInputManager& manager2)
{
  manager1.Swap (manager2);
}

}
