#include "shared.h"

using namespace media::freetype;

//Конструктор/деструктор
FreetypeStroker::FreetypeStroker (const FreetypeLibrary& in_library, unsigned int stroke_size)
  : library (in_library)
  , stroker (0)
{
  if (!stroke_size)
    return;

  library.FT_Stroker_New (&stroker);

  library.FT_Stroker_Set (stroker, (FT_Fixed)(stroke_size / 1000.f * 64.f), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
}

FreetypeStroker::~FreetypeStroker ()
{
  if (stroker)
    library.FT_Stroker_Done (stroker);
}
