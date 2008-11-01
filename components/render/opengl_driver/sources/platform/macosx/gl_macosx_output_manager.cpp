#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::macosx;

namespace
{

/*
    Основные определения типов
*/

typedef xtl::com_ptr<Output>   OutputPtr;
typedef stl::vector<OutputPtr> OutputArray;

}

/*
    Описание реализации адаптера
*/

struct OutputManager::Impl: public xtl::reference_counter
{
  OutputArray  outputs;  //устройства вывода
  static Impl* instance; //глобальный экземпляр реализации менеджера устройств вывода

  Impl ()
  {
    try
    {
      //построение массива устройств вывода

      CGDisplayCount displays_count;

      check_quartz_error (CGGetActiveDisplayList (0, 0, &displays_count), "::CGGetActiveDisplayList", "Can't get displays count");

      xtl::uninitialized_storage<CGDirectDisplayID> display_list (displays_count);

      check_quartz_error (CGGetActiveDisplayList (displays_count, display_list.data (), &displays_count), "::CGGetActiveDisplayList",
                          "Can't get active display list");

      outputs.reserve (displays_count);

      for (size_t i = 0; i < displays_count; i++)
        outputs.push_back (OutputPtr (new Output (display_list.data ()[i]), false));
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::low_level::opengl::macosx::OutputManager::Impl::Impl");
      throw;
    }

    instance = this;
  }

  ~Impl ()
  {
    instance = 0;
  }
};

OutputManager::Impl* OutputManager::Impl::instance = 0;

/*
    Конструктор / деструктор
*/

OutputManager::OutputManager ()
  : impl (OutputManager::Impl::instance)
{
  if (!impl) impl = new Impl;
  else       addref (impl);
}

OutputManager::~OutputManager ()
{
  release (impl);
}

/*
    Перечисление доступных устройств вывода
*/

size_t OutputManager::GetOutputsCount () const
{
  return impl->outputs.size ();
}

Output* OutputManager::GetOutput (size_t index) const
{
  if (index >= impl->outputs.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::macosx::OutputManager::GetOutput", "index", index, impl->outputs.size ());

  return get_pointer (impl->outputs [index]);
}

/*
   Поиск устройства вывода, перекрытие которого с окном максимально
*/

Output* OutputManager::FindContainingOutput (WindowRef window) const
{
    //получение границ окна

  ::Rect window_rect;

  if (GetWindowBounds (window, kWindowStructureRgn, &window_rect) != noErr)
    return 0;

    //поиск максимального перекрытия

  size_t  max_intersection_size = 0;
  Output* best_output = 0;

  for (OutputArray::iterator iter = impl->outputs.begin (), end = impl->outputs.end (); iter != end; ++iter)
  {
    CGRect output_bounds = CGDisplayBounds ((*iter)->GetDisplayID ());

    short output_left   = (short)output_bounds.origin.x,
    output_top    = (short)output_bounds.origin.y,
    output_right  = (short)(output_bounds.origin.x + output_bounds.size.width),
    output_bottom = (short)(output_bounds.origin.y + output_bounds.size.height);

    ::Rect intersection_rect;

    intersection_rect.left   = stl::max (output_left,   window_rect.left);
    intersection_rect.top    = stl::max (output_top,    window_rect.top);
    intersection_rect.right  = stl::min (output_right,  window_rect.right);
    intersection_rect.bottom = stl::min (output_bottom, window_rect.bottom);

    if (intersection_rect.left >= intersection_rect.right)
      continue;

    if (intersection_rect.top >= intersection_rect.bottom)
      continue;

    size_t intersection_size = (intersection_rect.right - intersection_rect.left) * (intersection_rect.bottom - intersection_rect.top);

    if (intersection_size > max_intersection_size)
    {
      max_intersection_size = intersection_size;
      best_output = iter->get ();
    }
  }

  return best_output;
}
