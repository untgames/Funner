#include "shared.h"

using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

template <class Fn>
inline void init_extension_entry (ILibrary& library, const char* name, Fn& fn)
{
  fn = (Fn)library.GetProcAddress (name, EntrySearch_Context | EntrySearch_NoThrow);
}

}

/*
    Инициализация точек входа WGL
*/

void GlxExtensionEntries::Init (ILibrary& library)
{
    //обнуление адресов точек входа

  memset (this, 0, sizeof (GlxExtensionEntries));

    //получение адресов точек входа расширений OpenGL

  init_extension_entry (library, "glXSwapIntervalSGI", SwapIntervalSGI);
}
