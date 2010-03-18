#include "shared.h"

using namespace media::rms;

/*
    Описание реализации связывания
*/

typedef stl::auto_ptr<ICustomBinding> BindingPtr;

struct Binding::Impl: public xtl::reference_counter
{
  BindingPtr           binding;           //объект связывания
  xtl::auto_connection on_delete_binding; //соединение с сигналом, оповещающим о досрочном удалении объекта

  Impl () {}
  
  Impl (BindingPtr& in_binding) : binding (in_binding)
  {
    xtl::trackable* trackable = binding->GetTrackable ();
    
    if (trackable)
      on_delete_binding = trackable->connect_tracker (xtl::bind (&Impl::ResetBinding, this));
  }

  void ResetBinding ()
  {
    binding.release ();
    on_delete_binding.disconnect ();
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

Binding::Binding ()
  : impl (new Impl)
{
}

Binding::Binding (ICustomBinding* in_binding)
{
  BindingPtr binding (in_binding);

  if (!binding)
    throw xtl::make_null_argument_exception ("media::rms::Binding::Binding", "binding");

  impl = new Impl (binding);
}

Binding::Binding (const Binding& binding)
  : impl (binding.impl)
{
  addref (impl);
}

Binding::~Binding ()
{
  release (impl);
}

Binding& Binding::operator = (const Binding& binding)
{
  Binding (binding).Swap (*this);
  
  return *this;
}

/*
    Синхронная загрузка и выгрузка ресурсов
*/

namespace
{

struct AsyncWaiter
{
  bool& done;
  
  AsyncWaiter (bool& in_done) : done (in_done) {}
  
  bool operator () (AsyncOperation&)
  {
    done = true;
    return true;
  }
};

void default_load_progress_handler (Progress& progress)
{
}

void default_unload_progress_handler (Progress&)
{
}

}

void Binding::Load ()
{
  if (!impl->binding)
    return;

  try
  {
    bool done = false, prepared = false;
    
    AsyncOperation::Pointer async_operation = AsyncOperation::Create (&default_load_progress_handler);    
    
    async_operation->AddTask (AsyncWaiter (prepared));
    
    impl->binding->AsyncLoad (*async_operation);
    
    async_operation->AddTask (AsyncWaiter (done));
    
    while (!prepared)    
    {
      try
      {
        AsyncOperation::DoStep ();
      }
      catch (...)
      {
      }
    }
    
    while (!done)
      AsyncOperation::DoStep ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Binding::Load");
    throw;
  }
}

void Binding::Unload ()
{
  if (!impl->binding)
    return;

  try
  {
    bool done = false;

    AsyncOperation::Pointer async_operation = AsyncOperation::Create (&default_unload_progress_handler);

    impl->binding->AsyncUnload (*async_operation);

    async_operation->AddTask (AsyncWaiter (done));

    while (!done) 
    {
      try
      {
        AsyncOperation::DoStep ();
      }
      catch (...)
      {
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Binding::Unload");
    throw;
  }
}

/*
    Асинхронная загрузка и выгрузка ресурсов
*/

void Binding::AsyncLoad (const ProgressHandler& progress_handler)
{
  if (!impl->binding)
    return;
  
  try
  {
    impl->binding->AsyncLoad (*AsyncOperation::Create (progress_handler));
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Binding::AsyncLoad");
    throw;
  }
}

void Binding::AsyncUnload (const ProgressHandler& progress_handler)
{
  if (!impl->binding)
    return;
    
  try
  {
    impl->binding->AsyncUnload (*AsyncOperation::Create (progress_handler));
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Binding::AsyncUnload(const ProgressHandler&)");
    throw;
  }
}

void Binding::AsyncLoad (AsyncOperation& async_operation)
{
  if (!impl->binding)
    return;

  try
  {
    impl->binding->AsyncLoad (async_operation);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Binding::AsyncLoad(AsyncOperation&)");
    throw;
  }
}

void Binding::AsyncUnload (AsyncOperation& async_operation)
{
  if (!impl->binding)
    return;

  try
  {
    impl->binding->AsyncUnload (async_operation);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Binding::AsyncUnload(AsyncOperation&)");
    throw;
  }
}

void Binding::AsyncLoad ()
{
  try
  {
    AsyncLoad (&default_load_progress_handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Binding::AsyncLoad()");
    throw;
  }
}

void Binding::AsyncUnload ()
{
  try
  {
    AsyncUnload (&default_unload_progress_handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::rms::Binding::AsyncUnload()");
    throw;
  }  
}

/*
    Обмен
*/

void Binding::Swap (Binding& binding)
{
  stl::swap (impl, binding.impl);
}

namespace media
{

namespace rms
{

void swap (Binding& binding1, Binding& binding2)
{
  binding1.Swap (binding2);
}

}

}
