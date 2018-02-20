#include "shared.h"

using namespace common;
using namespace media::spine;
using namespace media::SPINE_NAMESPACE_NAME;
using namespace SPINE_NAMESPACE_NAME;

#define COMPONENT_NAME_HELPER(...) SpineLoader_ ## __VA_ARGS__
#define COMPONENT_NAME(...) COMPONENT_NAME_HELPER(__VA_ARGS__)
#define SPINE_LOADER_COMPONENT_NAME COMPONENT_NAME(SPINE_VERSION)
#define MACRO_TO_STRING_HELPER(name) #name
#define MACRO_TO_STRING(macro) MACRO_TO_STRING_HELPER(macro)
#define SPINE_VERSION_STRING MACRO_TO_STRING(SPINE_VERSION)

namespace
{

/*
   Constants
*/

const char* COMPONENT_NAME_FORMAT = "media.spine.loaders.SpineLoader_%s"; //component name

}

namespace SPINE_NAMESPACE_NAME
{

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path)
{
  //According to documentation this function should load a texture and store it and its size in the void* rendererObject, width and height fields of an spAtlasPage struct.
  //But texture size is loaded from atlas json by spline, so no need to load it here

  //store texture path in rendering object
  self->rendererObject = new stl::string (path);
}

void _spAtlasPage_disposeTexture (spAtlasPage* self)
{
  //delete texture path stored in rendererObject field
  delete (stl::string*)self->rendererObject;
}

}

namespace components
{

namespace SPINE_NAMESPACE_NAME
{

/*
   Component
*/

class Component
{
  public:
    Component ()
    {
      stl::string spine_float_version_string = SPINE_VERSION_STRING;

      //replace underscore with dot to make valid float string
      for (char* ch = spine_float_version_string.begin (); *ch; ch++)
      {
        if (*ch == '_')
          *ch = '.';
      }

      SerializerManager::RegisterLoader ("json", spine_float_version_string.c_str (), &LoadJsonSkeletonData);
      SerializerManager::RegisterLoader ("skel", spine_float_version_string.c_str (), &LoadBinarySkeletonData);
    }

  private:
    static void ReadFileData (const char* file_name, xtl::uninitialized_storage<char>& read_to, bool add_null_char = false)
    {
      common::InputFile file (file_name);

      filesize_t file_size = file.Size ();

      //resize read buffer to file size
      read_to.resize (file_size + (add_null_char ? 1 : 0));

      char* read_ptr = read_to.data ();

      for (;file_size;)
      {
        size_t readed_bytes = file.Read (read_ptr, file_size);

        if (!readed_bytes)
          throw xtl::format_operation_exception ("media::spine::Component::ReadFileData", "Can't read data from file '%s'", file_name);

        file_size -= readed_bytes;
        read_ptr  += readed_bytes;
      }

      if (add_null_char)
        read_to.data () [read_to.size () - 1] = 0;
    }

    static SpineAtlasPtr LoadAtlas (const char* atlas_file_name)
    {
      static const char* METHOD_NAME = "media::spine::Component::LoadAtlas";

      if (!atlas_file_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "atlas_file_name");

      //read files data to memory
      xtl::uninitialized_storage<char> atlas_file_data;

      ReadFileData (atlas_file_name, atlas_file_data);

      //load atlas
      SpineAtlasPtr atlas (new SpineHandleHolder<spAtlas> (spAtlas_create (atlas_file_data.data (), atlas_file_data.size (), common::dir (atlas_file_name).c_str (), 0), spAtlas_dispose), false);

      if (!atlas->NativeHandle ())
        throw xtl::format_operation_exception (METHOD_NAME, "Can't load atlas from file '%s'", atlas_file_name);

      return atlas;
    }

    static SkeletonDataImpl* LoadJsonSkeletonData (const char* skeleton_file_name, const char* atlas_file_name)
    {
      try
      {
        if (!skeleton_file_name)
          throw xtl::make_null_argument_exception ("", "skeleton_file_name");

        //load atlas
        SpineAtlasPtr atlas = LoadAtlas (atlas_file_name);

        //read files data to memory
        xtl::uninitialized_storage<char> skeleton_file_data;

        ReadFileData (skeleton_file_name, skeleton_file_data, true);

        //load skeleton
        spSkeletonJson*      skeleton_json = spSkeletonJson_create (atlas->NativeHandle ());
        SpineSkeletonDataPtr skeleton_data (new SpineHandleHolder<spSkeletonData> (spSkeletonJson_readSkeletonData (skeleton_json, skeleton_file_data.data ()), spSkeletonData_dispose), false);

        if (!skeleton_data->NativeHandle ())
        {
          stl::string error = common::format ("Can't load skeleton data from json file '%s', error '%s'", skeleton_file_name, skeleton_json->error ? skeleton_json->error : "unknown");

          spSkeletonJson_dispose (skeleton_json);

          throw xtl::format_operation_exception ("", error.c_str ());
        }

        spSkeletonJson_dispose (skeleton_json);

        return new SkeletonDataSpineImpl (atlas, Clipper (), skeleton_data);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::spine::Component::LoadJsonSkeletonData");
        throw;
      }
    }

    static SkeletonDataImpl* LoadBinarySkeletonData (const char* skeleton_file_name, const char* atlas_file_name)
    {
      try
      {
        if (!skeleton_file_name)
          throw xtl::make_null_argument_exception ("", "skeleton_file_name");

        //load atlas
        SpineAtlasPtr atlas = LoadAtlas (atlas_file_name);

        //read files data to memory
        xtl::uninitialized_storage<char> skeleton_file_data;

        ReadFileData (skeleton_file_name, skeleton_file_data);

        //load skeleton
        spSkeletonBinary*    skeleton_binary = spSkeletonBinary_create (atlas->NativeHandle ());
        SpineSkeletonDataPtr skeleton_data (new SpineHandleHolder<spSkeletonData> (spSkeletonBinary_readSkeletonData (skeleton_binary, (const unsigned char*)skeleton_file_data.data (), skeleton_file_data.size ()), spSkeletonData_dispose), false);

        if (!skeleton_data->NativeHandle ())
        {
          stl::string error = common::format ("Can't load skeleton data from binary file '%s', error '%s'", skeleton_file_name, skeleton_binary->error ? skeleton_binary->error : "unknown");

          spSkeletonBinary_dispose (skeleton_binary);

          throw xtl::format_operation_exception ("", error.c_str ());
        }

        spSkeletonBinary_dispose (skeleton_binary);

        return new SkeletonDataSpineImpl (atlas, Clipper (), skeleton_data);
      }
      catch (xtl::exception& e)
      {
        e.touch ("media::spine::Component::LoadBinarySkeletonData");
        throw;
      }
    }

    static SkeletonClippingSpineImplPtr Clipper ()
    {
      if (!clipper)
      {
        SpineSkeletonClippingPtr spine_clipper (new SpineHandleHolder<spSkeletonClipping> (spSkeletonClipping_create (), spSkeletonClipping_dispose), false);

        clipper = SkeletonClippingSpineImplPtr (new SkeletonClippingSpineImpl (spine_clipper), false);
      }

      return clipper;
    }

  private:
    static SkeletonClippingSpineImplPtr clipper;
};

SkeletonClippingSpineImplPtr Component::clipper;

extern "C"
{
  ComponentRegistrator<Component> SPINE_LOADER_COMPONENT_NAME (common::format (COMPONENT_NAME_FORMAT, SPINE_VERSION_STRING).c_str ());
}

}

}
