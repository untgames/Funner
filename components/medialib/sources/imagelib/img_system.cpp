#include "shared.h"

using namespace medialib;

/*
    ImageSystemImpl
*/

ImageSystemImpl::ImageSystemImpl ()
{
  RegisterLoader ("jpg",     &Image::DefaultLoader);
  RegisterLoader ("bmp",     &Image::DefaultLoader);
  RegisterLoader ("tga",     &Image::DefaultLoader);
  RegisterLoader ("tif",     &Image::DefaultLoader);
  RegisterLoader ("dds",     &Image::DefaultSaver);    
//  RegisterLoader ("cubemap", &Image::DefaultSaver);
//  RegisterLoader ("skybox", &Image::DefaultSaver);
  RegisterSaver  ("jpg",     &Image::DefaultSaver);
  RegisterSaver  ("bmp",     &Image::DefaultSaver);
  RegisterSaver  ("tga",     &Image::DefaultSaver);
  RegisterSaver  ("tif",     &Image::DefaultSaver);
  RegisterSaver  ("cubemap", &Image::DefaultSaver);
  RegisterSaver  ("skybox",  &Image::DefaultSaver);
  RegisterSaver  ("dds",     &Image::DefaultSaver);
}

/*
    ImageSystem
*/

void ImageSystem::SetDebugLog (const LogHandler& handler)
{
  ImageSystemSingleton::Instance ().SetDebugLog (handler);
}

const ImageSystem::LogHandler& ImageSystem::GetDebugLog ()
{
  return ImageSystemSingleton::Instance ().GetDebugLog ();
}

void ImageSystem::RegisterLoader (const char* extension, const LoadHandler& handler)
{
  ImageSystemSingleton::Instance ().RegisterLoader (extension, handler);
}

void ImageSystem::RegisterSaver (const char* extension, const SaveHandler& handler)
{
  ImageSystemSingleton::Instance ().RegisterSaver (extension, handler);
}

void ImageSystem::UnregisterLoader (const char* extension)
{
  ImageSystemSingleton::Instance ().UnregisterLoader (extension);
}

void ImageSystem::UnregisterSaver (const char* extension)
{
  ImageSystemSingleton::Instance ().UnregisterSaver (extension);
}

void ImageSystem::UnregisterAll ()
{
  ImageSystemSingleton::Instance ().UnregisterAll ();
}
