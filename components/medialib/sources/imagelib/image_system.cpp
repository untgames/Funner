#include "shared.h"
#include <stl/functional>

using namespace medialib;

namespace
{

void ResetImage (Image* img)
{
  Image().Swap (*img);
}

void DefaultLogFunction (const char* log_message)
{
  printf ("%s\n", log_message);
}

}

ImageSystemImpl::ImageSystemImpl ()
{
  log_function = DefaultLogFunction;
  RegisterLoadFunc ("jpg", &Image::DefaultLoader);
  RegisterLoadFunc ("bmp", &Image::DefaultLoader);
  RegisterLoadFunc ("tga", &Image::DefaultLoader);
  RegisterLoadFunc ("tif", &Image::DefaultLoader);
  RegisterSaveFunc ("jpg", &Image::DefaultSaver);
  RegisterSaveFunc ("bmp", &Image::DefaultSaver);
  RegisterSaveFunc ("tga", &Image::DefaultSaver);
  RegisterSaveFunc ("tif", &Image::DefaultSaver);
}

ImageSystemImpl::~ImageSystemImpl ()
{
  CloseAllImages ();
}

void ImageSystemImpl::RegisterImage (Image& image)
{
  open_images.insert (&image);
}

void ImageSystemImpl::UnregisterImage (Image& image)
{
  open_images.erase (&image);
}

void ImageSystemImpl::CloseAllImages ()
{
  for_each (open_images.begin (),open_images.end (), ResetImage);
}

void ImageSystemImpl::SetDebugLog (const design::function<void (const char*)>& new_log_function)
{
  log_function = new_log_function;
}

void ImageSystemImpl::DebugLog (const char* debug_message)
{
  log_function (debug_message);
}

bool ImageSystemImpl::RegisterLoadFunc (const char* extension, const ImageSystem::CodecLoadFunc& codec)
{
  CodecLoadFunction codec_function = {codec, false};
  return (load_codecs.insert_pair (extension, codec_function)).second;
}

bool ImageSystemImpl::RegisterSaveFunc (const char* extension, const ImageSystem::CodecSaveFunc& codec)
{
  CodecSaveFunction codec_function = {codec, false};
  return (save_codecs.insert_pair (extension, codec_function)).second;
}

void ImageSystemImpl::UnRegisterLoadFunc (const char* extension)
{
  load_codecs.erase (stl::string (extension));
}

void ImageSystemImpl::UnRegisterSaveFunc (const char* extension)
{
  save_codecs.erase (stl::string (extension));
}

void ImageSystemImpl::UnRegisterAllFuncs ()
{
  load_codecs.clear ();
  save_codecs.clear ();
}

CodecLoadFunction* ImageSystemImpl::GetLoadFunc (const char* extension)
{
  stl::hash_map<stl::string, CodecLoadFunction>::iterator load_func = load_codecs.find (stl::string (extension));

  if (load_func == load_codecs.end ())
    return NULL;
  return &load_func->second;
}

CodecSaveFunction* ImageSystemImpl::GetSaveFunc (const char* extension)
{
  stl::hash_map<stl::string, CodecSaveFunction>::iterator save_func = save_codecs.find (stl::string (extension));

  if (save_func == save_codecs.end ())
    return NULL;
  return &save_func->second;
}

namespace medialib
{

void ImageSystem::SetDebugLog (const design::function<void (const char*)>& log_function)
{
  ImageSystemSingleton::Instance ().SetDebugLog (log_function);
}

bool ImageSystem::RegisterLoadCodec (const char* extension, const CodecLoadFunc& codec)
{
  return ImageSystemSingleton::Instance ().RegisterLoadFunc (extension, codec);
}

bool ImageSystem::RegisterSaveCodec (const char* extension, const CodecSaveFunc& codec)
{
  return ImageSystemSingleton::Instance ().RegisterSaveFunc (extension, codec);
}

void ImageSystem::UnRegisterLoadCodec (const char* extension)
{
  ImageSystemSingleton::Instance ().UnRegisterLoadFunc (extension);
}

void ImageSystem::UnRegisterSaveCodec (const char* extension)
{
  ImageSystemSingleton::Instance ().UnRegisterSaveFunc (extension);
}

void ImageSystem::UnRegisterAllCodecs ()
{
  ImageSystemSingleton::Instance ().UnRegisterAllFuncs ();
}

}
