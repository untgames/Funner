#include "shared.h"

using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

template <class Fn>
inline void init_extension_entry (ILibrary& library, const char* name, Fn& fn)
{
  fn = reinterpret_cast<Fn> (library.GetProcAddress (name, EntrySearch_Context | EntrySearch_NoThrow));
}

}

/*
    Инициализация точек входа GLX
*/

void GlxExtensionsEntries::Init (ILibrary& library)
{
    //обнуление адресов точек входа

  memset (this, 0, sizeof GlxExtensionsEntries);

    //получение адресов точек входа расширений OpenGL
<<<GLXINIT>>>
}
