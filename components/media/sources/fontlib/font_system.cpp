#include "shared.h"

using namespace media;

namespace
{

void ResetFont (FontFace* sound)
{
  FontFace().Swap (*sound);
}

}

FontSystemImpl::FontSystemImpl ()
{
  RegisterLoadFunc ("bfs", &FontFace::DefaultBFSLoader);
}

FontSystemImpl::~FontSystemImpl ()
{
  CloseAllFontFaces ();
}

void FontSystemImpl::RegisterFontFace (FontFace& sound)
{
  open_font_faces.insert (&sound);
}

void FontSystemImpl::UnregisterFontFace (FontFace& sound)
{
  open_font_faces.erase (&sound);
}

void FontSystemImpl::CloseAllFontFaces ()
{
  for_each (open_font_faces.begin (),open_font_faces.end (), ResetFont);
}

bool FontSystemImpl::RegisterLoadFunc (const char* extension, const FontSystem::CodecLoadFunc& load_codec)
{
  return (load_codecs.insert_pair (extension, load_codec)).second;
}

void FontSystemImpl::UnRegisterLoadFunc (const char* extension)
{
  load_codecs.erase (stl::string (extension));
}

void FontSystemImpl::UnRegisterAllFuncs ()
{
  load_codecs.clear ();
}

FontSystem::CodecLoadFunc* FontSystemImpl::GetLoadFunc (const char* extension)
{
  stl::hash_map<stl::string, FontSystem::CodecLoadFunc>::iterator load_func = load_codecs.find (stl::string (extension));

  if (load_func == load_codecs.end ())
    return NULL;
  return &load_func->second;
}

namespace media
{

bool FontSystem::RegisterCodec (const char* extension, const CodecLoadFunc& load_codec)
{
  return FontSystemSingleton::Instance ().RegisterLoadFunc (extension, load_codec);
}

void FontSystem::UnregisterCodec (const char* extension)
{
  FontSystemSingleton::Instance ().UnRegisterLoadFunc (extension);
}

void FontSystem::UnregisterAllCodecs ()
{
  FontSystemSingleton::Instance ().UnRegisterAllFuncs ();
}

}
