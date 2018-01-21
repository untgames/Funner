#include "shared.h"

using namespace media::spine;

namespace
{

//Constants
const char* LOG_NAME = "media.spine.SpineSkeletonData"; //log stream name

//logic copied from spine SkeletonBinary.c (version 3.6)
int spine_read_var_int (unsigned char*& input, int optimizePositive)
{
  unsigned char b = *input++;
  int value = b & 0x7F;

  if (b & 0x80)
  {
    b = *input++;
    value |= (b & 0x7F) << 7;

    if (b & 0x80)
    {
      b = *input++;
      value |= (b & 0x7F) << 14;

      if (b & 0x80)
      {
         b = *input++;
         value |= (b & 0x7F) << 21;

         if (b & 0x80) value |= (*input++ & 0x7F) << 28;
      }
    }
  }

  if (!optimizePositive) value = (((unsigned int)value >> 1) ^ -(value & 1));

  return value;
}


}

/*
   Constructors / destructor / assignment
*/

SkeletonData::SkeletonData ()
  : impl (0)
  {}

SkeletonData::SkeletonData (SkeletonDataImpl* in_impl)
  : impl (in_impl)
  {}

SkeletonData::SkeletonData (const char* skeleton_file_name, const char* atlas_file_name)
  : impl (0)
{
  try
  {
    Load (skeleton_file_name, atlas_file_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::spine::SkeletonData::SkeletonData (const char*, const char*)");
    throw;
  }
}

SkeletonData::SkeletonData (const SkeletonData& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

SkeletonData::~SkeletonData ()
{
  if (impl)
    impl->Release ();
}

SkeletonData& SkeletonData::operator = (const SkeletonData& source)
{
  SkeletonData (source).Swap (*this);

  return *this;
}

/*
   Load from file
*/

void SkeletonData::Load (const char* skeleton_file_name, const char* atlas_file_name)
{
  static const char* METHOD_NAME = "media::spine::SkeletonData::Load";

  if (!skeleton_file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "skeleton_file_name");

  if (!atlas_file_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "atlas_file_name");

  try
  {
    static common::ComponentLoader component_loader ("media.spine.loaders.*");

    common::InputFile skeleton_file (skeleton_file_name);

    char read_buffer [128];

    memset (read_buffer, 0, sizeof (read_buffer));

    //read first part of file to determine required spine loader version
    size_t      read_size = sizeof (read_buffer) - 1;
    stl::string spine_version;

    stl::string file_name_suffix = common::suffix (skeleton_file_name);

    if (read_size == skeleton_file.Read (read_buffer, read_size))
    {
      if (file_name_suffix == ".json")
      {
        //search for a spine version attribute
        const char* SPINE_VERSION_ATTRIBUTE_TOKEN = "\"spine\"";

        const char* spine_version_start = strstr (read_buffer, SPINE_VERSION_ATTRIBUTE_TOKEN);

        if (spine_version_start)
        {
          //go to the end of token
          spine_version_start += xtl::xstrlen (SPINE_VERSION_ATTRIBUTE_TOKEN);

          //find start of token value
          spine_version_start = strchr (spine_version_start, '\"');

          if (spine_version_start)
          {
            spine_version_start++;  //now spine_version_start should point to first digit of spine version

            //find closing symbol of version string
            char* spine_version_end = strchr (spine_version_start, '\"');

            if (spine_version_end)
            {
              *spine_version_end = 0; //terminate string here

              spine_version = spine_version_start;
            }
          }
        }
      }
      else if (file_name_suffix == ".skel")
      {
        unsigned char* input = (unsigned char*)read_buffer;

        int hash_length = spine_read_var_int (input, 1);

        //check hash length was read correctly and we have enough data in read buffer
        if (hash_length > 0 && (hash_length + 5) < sizeof (read_buffer))
        {
          input += hash_length - 1;

          int version_length = spine_read_var_int (input, 1);

          if (version_length > 0 && (input - (unsigned char*)read_buffer + version_length) < sizeof (read_buffer))
          {
            input [version_length - 1] = 0;  //terminate string after version

            spine_version = (char*)input;
          }
        }
      }
    }

    skeleton_file.Close ();

    if (!spine_version.empty ())
    {
      //check if version has more than one dot symbols
      stl::string::size_type dot_symbol = spine_version.find ('.');

      if (dot_symbol != stl::string::npos)
      {
        dot_symbol = spine_version.find ('.', dot_symbol + 1);

        if (dot_symbol != stl::string::npos)
        {
          spine_version.resize (dot_symbol);
        }
      }
    }

    const SerializerManager::SkeletonDataLoader* loader = 0;

    if (!spine_version.empty ())
      loader = SerializerManagerSingleton::Instance ()->FindLoader (skeleton_file_name, spine_version.c_str ());

    if (!loader)
    {
      if (file_name_suffix == ".skel")
        throw xtl::format_operation_exception ("", "Couldn't find spine loader for binary format version '%s'", spine_version.c_str ());

      common::Log (LOG_NAME).Printf ("Couldn't find spine loader for version '%s', try to load with latest available", spine_version.c_str ());

      loader = SerializerManagerSingleton::Instance ()->FindLoaderLatestVersion (skeleton_file_name);
    }

    if (!loader)
      throw xtl::format_operation_exception ("", "Can't find loader for file '%s'", skeleton_file_name);

    SkeletonData ((*loader) (skeleton_file_name, atlas_file_name)).Swap (*this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }
}

/*
   Create object instances
*/

Skeleton SkeletonData::CreateSkeleton () const
{
  if (!impl)
    throw xtl::format_operation_exception ("media::spine::SkeletonData::CreateSkeleton", "Skeleton data not loaded.");

  return Wrappers::Wrap<Skeleton, SkeletonImpl> (impl->CreateSkeleton ());
}

AnimationStateData SkeletonData::CreateAnimationStateData () const
{
  if (!impl)
    throw xtl::format_operation_exception ("media::spine::SkeletonData::CreateSkeleton", "Skeleton data not loaded.");

  return Wrappers::Wrap<AnimationStateData, AnimationStateDataImpl> (impl->CreateAnimationStateData ());
}

/*
   Params
*/

float SkeletonData::Width () const
{
  if (!impl)
    return 0.f;

  return impl->Width ();
}

float SkeletonData::Height () const
{
  if (!impl)
    return 0.f;

  return impl->Height ();
}

/*
   Animations
*/

unsigned int SkeletonData::AnimationsCount () const
{
  if (!impl)
    return 0;

  return impl->AnimationsCount ();
}

const char* SkeletonData::AnimationName (unsigned int index) const
{
  if (index >= AnimationsCount ())
    throw xtl::make_range_exception ("media::spine::SkeletonData::AnimationName", "index", index, 0, AnimationsCount ());

  return impl->AnimationName (index);
}

float SkeletonData::AnimationDuration (unsigned int index) const
{
  if (index >= AnimationsCount ())
    throw xtl::make_range_exception ("media::spine::SkeletonData::AnimationDuration", "index", index, 0, AnimationsCount ());

  return impl->AnimationDuration (index);
}

/*
   Skins
*/

unsigned int SkeletonData::SkinsCount () const
{
  if (!impl)
    return 0;

  return impl->SkinsCount ();
}

const char* SkeletonData::SkinName (unsigned int index) const
{
  if (index >= SkinsCount ())
    throw xtl::make_range_exception ("media::spine::SkeletonData::SkinName", "index", index, 0, SkinsCount ());

  return impl->SkinName (index);
}

const char* SkeletonData::DefaultSkinName () const
{
  if (!impl)
    return 0;

  return impl->DefaultSkinName ();
}

/*
   Swap
*/

void SkeletonData::Swap (SkeletonData& data)
{
  stl::swap (impl, data.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (SkeletonData& data1, SkeletonData& data2)
{
  data1.Swap (data2);
}

}

}
