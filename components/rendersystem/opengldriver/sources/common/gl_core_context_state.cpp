#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Исключение: внутренняя ошибка OpenGL
*/

struct OpenGLExceptionTag;

typedef DerivedException<Exception, OpenGLExceptionTag> OpenGLException;

/*
    Описание реализации состояния контекста
*/

struct ContextState::Impl: public xtl::reference_counter
{
  Context        context;                            //контекст OpenGL
  ContextObject* selected_objects [ContextSlot_Num]; //выбранные объекты

  Impl (ISwapChain* swap_chain) : context (swap_chain)
  {
    memset (selected_objects, 0, sizeof (selected_objects));
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

ContextState::ContextState (ISwapChain* swap_chain)
  : impl (new Impl (swap_chain))
{
}

ContextState::ContextState (const ContextState& state)
  : impl (state.impl)
{
  addref (impl);
}

ContextState::~ContextState ()
{
  release (impl);
}

ContextState& ContextState::operator = (const ContextState& state)
{
  ContextState (state).Swap (*this);  

  return *this;
}

/*
    Выбор объекта в контекст
*/

void ContextState::SetObject (ContextSlot slot, ContextObject* object)
{
  if (slot < 0 || slot >= ContextSlot_Num)
    RaiseInvalidArgument ("render::low_level::opengl::ContextState::SetObject", "slot", slot);
    
  ContextObject*& current_object = impl->selected_objects [slot];
    
  if (current_object == object)
    return;

  ContextLock lock (*this);

  if (current_object)
    current_object->Unbind (*this);

  current_object = 0;

  if (object)
    object->Bind (*this);

  current_object = object;
}

void ContextState::SetObject (ContextSlot slot, IObject* object)
{
  SetObject (slot, cast_object<ContextObject*> (object));
}

ContextObject* ContextState::GetObject (ContextSlot slot) const
{
  if (slot < 0 || slot >= ContextSlot_Num)
    RaiseInvalidArgument ("render::low_level::opengl::ContextState::GetObject", "slot", slot);

  return impl->selected_objects [slot];
}

/*
    Контекст
*/

Context& ContextState::GetContext ()
{
  return impl->context;
}

const Context& ContextState::GetContext () const
{
  return impl->context;
}

/*
    Проверка состояния OpenGL
*/

void ContextState::Check (const char* source)
{
  if (!source)
    return;
    
  ContextLock lock (*this);
  
  GLenum error = glGetError ();
  
  switch (error)
  {
    case GL_NO_ERROR:
      break;
    case GL_INVALID_ENUM:
      Raise<OpenGLException> (source, "OpenGL error: invalid enum");
      break;
    case GL_INVALID_VALUE:
      Raise<OpenGLException> (source, "OpenGL error: invalid value");
      break;
    case GL_INVALID_OPERATION:
      Raise<OpenGLException> (source, "OpenGL error: invalid operation");
      break;
    case GL_STACK_OVERFLOW:
      Raise<OpenGLException> (source, "OpenGL error: stack overflow");
      break;
    case GL_STACK_UNDERFLOW:
      Raise<OpenGLException> (source, "OpenGL error: stack underflow");
      break;
    case GL_OUT_OF_MEMORY:
      Raise<OpenGLException> (source, "OpenGL error: out of memory");
      break;
    default:
      Raise<OpenGLException> (source, "OpenGL error: code=%d", error);
      break;
  }
}

/*
    Обмен
*/

void ContextState::Swap (ContextState& state)
{
  stl::swap (impl, state.impl);
}

namespace render
{

namespace low_level
{

namespace opengl
{

void swap (ContextState& state1, ContextState& state2)
{
  state1.Swap (state2);
}

}

}

}
