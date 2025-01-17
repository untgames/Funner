#include <cstdio>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/exception.h>
#include <xtl/function.h>
#include <xtl/shared_ptr.h>

#include <common/log.h>
#include <common/strlib.h>

#include <media/geometry/mesh.h>

#include <media/spine/animation_state.h>
#include <media/spine/animation_state_data.h>
#include <media/spine/attachment.h>
#include <media/spine/bone.h>
#include <media/spine/material.h>
#include <media/spine/skeleton.h>
#include <media/spine/skeleton_data.h>
#include <media/spine/slot.h>

using namespace media::spine;

struct TestDesc
{
  const char* spine_file_name;
  const char* atlas_file_name;
  bool        mix_blend;
};

TestDesc TEST_DESCS [] = { { "data/3.8/vine/export/vine-pro.skel", "data/3.8/vine/export/vine.atlas", false },
                           { "data/3.8/vine/export/vine-pro.json", "data/3.8/vine/export/vine.atlas", false },
                           { "data/3.8/alien/export/alien-ess.skel", "data/3.8/alien/export/alien.atlas", false },
                           { "data/3.8/alien/export/alien-ess.json", "data/3.8/alien/export/alien.atlas", false },
                           { "data/3.8/alien/export/alien-pro.skel", "data/3.8/alien/export/alien.atlas", false },
                           { "data/3.8/alien/export/alien-pro.json", "data/3.8/alien/export/alien.atlas", false },
                           { "data/3.8/coin/export/coin-pro.skel", "data/3.8/coin/export/coin.atlas", false },
                           { "data/3.8/coin/export/coin-pro.json", "data/3.8/coin/export/coin.atlas", false },
                           { "data/3.8/dragon/export/dragon-ess.skel", "data/3.8/dragon/export/dragon.atlas", false },
                           { "data/3.8/dragon/export/dragon-ess.json", "data/3.8/dragon/export/dragon.atlas", false },
                           { "data/3.8/goblins/export/goblins-ess.skel", "data/3.8/goblins/export/goblins.atlas", false },
                           { "data/3.8/goblins/export/goblins-ess.json", "data/3.8/goblins/export/goblins.atlas", false },
                           { "data/3.8/tank/export/tank-pro.skel", "data/3.8/tank/export/tank.atlas", false },
                           { "data/3.8/tank/export/tank-pro.skel", "data/3.8/tank/export/tank.atlas", true },
                           { "data/3.8/tank/export/tank-pro.json", "data/3.8/tank/export/tank.atlas", false } };

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

void user_event_handler (const char* source, const TrackEntry& track, const char* event_name, int int_value, float float_value, const char* string_value)
{
  printf ("User event from source '%s', event '%s', int value %d, float value %.2f, string value '%s'\n", source, event_name, int_value, float_value, string_value);
}

void animation_event_handler (const char* source, const TrackEntry& track, AnimationEvent event)
{
  printf ("Animation event from source '%s', event '%d'\n", source, event);
}

void print (const SkeletonData& skeleton_data)
{
  printf ("Skeleton Data:\n");

  printf ("  Size %.2f x %.2f\n", skeleton_data.Width (), skeleton_data.Height ());

  printf ("  Animations (count %d):\n", skeleton_data.AnimationsCount ());

  for (int i = 0, count = skeleton_data.AnimationsCount (); i < count; i++)
  {
    printf ("    animation '%s' duration %.2f\n", skeleton_data.AnimationName (i), skeleton_data.AnimationDuration (i));
  }

  if (skeleton_data.DefaultSkinName ())
    printf ("  Default skin '%s'\n", skeleton_data.DefaultSkinName ());
  else
    printf ("  Default skin is null\n");

  printf ("  Skins (count %d):\n", skeleton_data.SkinsCount ());

  for (int i = 0, count = skeleton_data.SkinsCount (); i < count; i++)
    printf ("    '%s'\n", skeleton_data.SkinName (i));
}

void print_offset (int offset)
{
  for (int i = 0; i < offset; i++)
    printf ("  ");
}

void print (const Bone& bone, int offset)
{
  print_offset (offset);
  printf ("bone '%s':\n", bone.Name ());

  print_offset (offset);
  printf ("  has parent %c\n", bone.Parent () ? 'y' : 'n');

  print_offset (offset);
  printf ("  transform: rotation %.2f, scale %.2f x %.2f, shear %.2f x %.2f, translation %.2f x %.2f\n", bone.Rotation (), bone.Scale ().x, bone.Scale ().y, bone.Shear ().x, bone.Shear ().y, bone.Position ().x, bone.Position ().y);

  print_offset (offset);
  printf ("  world transform: rotation %.2f x %.2f, scale %.2f x %.2f, translation %.2f x %.2f\n", bone.WorldRotation ().x, bone.WorldRotation ().y, bone.WorldScale ().x, bone.WorldScale ().y, bone.WorldPosition ().x, bone.WorldPosition ().y);

  if (bone.ChildrenCount ())
  {
    print_offset (offset);
    printf ("  children bones (count %d):\n", bone.ChildrenCount ());

    for (unsigned int i = 0, count = bone.ChildrenCount (); i < count; i++)
      print (bone.Child (i), offset + 2);
  }
}

void print (const Slot& slot)
{
  printf ("    slot '%s':\n", slot.Name ());

  printf ("      color %.2f %.2f %.2f %.2f\n", slot.Color ().x, slot.Color ().y, slot.Color ().z, slot.Color ().w);

  if (slot.Bone ())
    printf ("      bone name '%s'\n", slot.Bone ()->Name ());
  else
    printf ("      bone is null\n");

  if (slot.Attachment ())
  {
    const Attachment* attachment = slot.Attachment ();

    printf ("      attachment type %d\n", attachment->Type ());

    switch (attachment->Type ())
    {
      case AttachmentType_Point:
      {
        const PointAttachmentData* point_attachment = attachment->AttachmentData<PointAttachmentData> ();

        printf ("      point attachment data:\n");
        printf ("        position %.2f %.2f\n", point_attachment->Position ().x, point_attachment->Position ().y);
        printf ("        rotation %.2f\n", point_attachment->Rotation ());
        printf ("        color %.2f %.2f %.2f %.2f\n", point_attachment->Color ().x, point_attachment->Color ().y, point_attachment->Color ().z, point_attachment->Color ().w);

        break;
      }
      default:
        printf ("      no attachment specific data\n");
        break;
    }
  }
  else
  {
    printf ("      attachment is null\n");
  }
}

void print (const Skeleton& skeleton)
{
  printf ("Skeleton:\n");

  printf ("  slots (count %u):\n", skeleton.SlotsCount ());

  for (unsigned int i = 0, count = skeleton.SlotsCount (); i < count; i++)
    print (skeleton.Slot (i));

  printf ("  bones:");
  print (skeleton.RootBone (), 2);

  printf ("  meshes (count %u):\n", skeleton.MeshesCount ());

  for (unsigned int i = 0, count = skeleton.MeshesCount (); i < count; i++)
  {
    media::geometry::Mesh               mesh         = skeleton.Mesh (i);
    const media::geometry::MaterialMap& material_map = mesh.MaterialMap ();

    printf ("    mesh %u:\n", i);
    printf ("      primitives (count %u):\n", mesh.PrimitivesCount ());

    for (unsigned int j = 0, primitives_count = mesh.PrimitivesCount (); j < primitives_count; j++)
    {
      printf ("        primitive %u:\n", j);

      const media::geometry::Primitive& primitive     = mesh.Primitive (j);
      const char*                       material_name = material_map.MaterialName (primitive.material_id);

      printf ("          type '%s'\n", get_type_name (primitive.type));
      printf ("          first %u\n", primitive.first);
      printf ("          count %u\n", primitive.count);
      printf ("          base vertex %u\n", primitive.base_vertex);
      printf ("          material '%s':\n", material_name);

      const Material& material = skeleton.Material (material_name);

      printf ("            blend mode %d\n", material.BlendMode ());
      printf ("            texcoord wrap u %d\n", material.TexcoordWrapU ());
      printf ("            texcoord wrap v %d\n", material.TexcoordWrapV ());
      printf ("            texture path '%s'\n", material.TexturePath ());
    }

    const media::geometry::IndexBuffer& index_buffer = mesh.IndexBuffer ();

    if (index_buffer.DataType () != media::geometry::IndexType_UInt16)
    {
      printf ("Can't print index buffer, unsupported data type %d\n", index_buffer.DataType ());
      continue;
    }

    printf ("      index buffer (count %u):", index_buffer.Size ());

    const uint16_t* indices = index_buffer.Data<uint16_t> ();

    for (unsigned int k = 0, indices_count = index_buffer.Size (); k < indices_count; k++)
      printf (" %d", (int)indices [k]);

    printf ("\n");

    const media::geometry::VertexBuffer& vertex_buffer = mesh.VertexBuffer (0);

    printf ("      vertex buffer (streams count %u):\n", vertex_buffer.StreamsCount ());

    for (unsigned int j = 0, streams_count = vertex_buffer.StreamsCount (); j < streams_count; j++)
    {
      const media::geometry::VertexStream& vertex_stream = vertex_buffer.Stream (j);

      printf ("        vertex stream %u - vertex size %u, vertices count %u:", j, vertex_stream.VertexSize (), vertex_stream.Size ());

      unsigned int vertex_elements = vertex_stream.VertexSize () / sizeof (float);
      float*       elements        = (float*)vertex_stream.Data ();

      for (unsigned int k = 0, elements_count = vertex_stream.Size () * vertex_elements; k < elements_count; k++)
      {
        printf (" %.2f", elements [k]);
      }

      printf ("\n");
    }
  }

  printf ("  skin '%s'\n", skeleton.Skin () ? skeleton.Skin () : "null");
}

void print (const TrackEntry& track_entry)
{
  printf ("  is disposed %c\n", track_entry.IsDisposed () ? 'y' : 'n');

  if (track_entry.IsDisposed ())
    return;

  printf ("  animation      '%s'\n",  track_entry.Animation ());
  printf ("  duration        %.2f\n", track_entry.Duration ());
  printf ("  alpha           %.2f\n", track_entry.Alpha ());
  printf ("  animation end   %.2f\n", track_entry.AnimationEnd ());
  printf ("  animation last  %.2f\n", track_entry.AnimationLast ());
  printf ("  animation start %.2f\n", track_entry.AnimationStart ());
  printf ("  delay           %.2f\n", track_entry.Delay ());
  printf ("  is looping      %c\n",   track_entry.IsLooping () ? 'y' : 'n');
  printf ("  time scale      %.2f\n", track_entry.TimeScale ());
  printf ("  track time      %.2f\n", track_entry.TrackTime ());
  printf ("  mix blend       %d\n",   track_entry.MixBlend ());
  printf ("  hold previous   %c\n",   track_entry.HoldPrevious () ? 'y' : 'n');
  printf ("  mix duration    %.2f\n", track_entry.MixDuration ());
  printf ("  mix time        %.2f\n", track_entry.MixTime ());
  printf ("  has mixing from %c\n",   track_entry.MixingFrom () ? 'y' : 'n');
  printf ("  has next        %c\n",   track_entry.Next () ? 'y' : 'n');
}

int main ()
{
  printf ("Results of spine_2 test:\n");

  common::LogFilter log_filter ("*", &print_log);

  for (size_t i = 0, count = sizeof (TEST_DESCS) / sizeof (*TEST_DESCS); i < count; i++)
  {
    const TestDesc& test_desc = TEST_DESCS[i];

    printf ("Testing spine file '%s':\n", test_desc.spine_file_name);

    try
    {
      SkeletonData skeleton_data (test_desc.spine_file_name, test_desc.atlas_file_name);

      print (skeleton_data);

      Skeleton skeleton = skeleton_data.CreateSkeleton ();

      if (skeleton_data.SkinsCount () > 1)
      {
        skeleton.SetSkin (skeleton_data.SkinName (1));
      }

      skeleton.UpdateWorldTransform ();

      print (skeleton);

      AnimationStateData animation_state_data = skeleton_data.CreateAnimationStateData ();

      printf ("Animation state data default mix time: %f\n", animation_state_data.DefaultMix ());

      AnimationState animation_state = animation_state_data.CreateAnimationState ();

      animation_state.RegisterEventHandler (xtl::bind (user_event_handler, "animation_state", _1, _2, _3, _4, _5));

      for (int i = 0; i < AnimationEvent_Num; i++)
        animation_state.RegisterEventHandler ((AnimationEvent)i, xtl::bind (animation_event_handler, "animation_state", _1, _2));

      TrackEntry track_entry           = animation_state.SetAnimation (0, skeleton_data.AnimationName (0), false);
      TrackEntry empty_animation_entry = animation_state.EnqueueEmptyAnimation (0, 0.1f, 0);

      track_entry.RegisterEventHandler (xtl::bind (user_event_handler, "track_entry", _1, _2, _3, _4, _5));
      empty_animation_entry.RegisterEventHandler (xtl::bind (user_event_handler, "empty_animation_entry", _1, _2, _3, _4, _5));

      for (int i = 0; i < AnimationEvent_Num; i++)
      {
        track_entry.RegisterEventHandler ((AnimationEvent)i, xtl::bind (animation_event_handler, "track_entry", _1, _2));
        empty_animation_entry.RegisterEventHandler ((AnimationEvent)i, xtl::bind (animation_event_handler, "empty_animation_entry", _1, _2));
      }

      xtl::shared_ptr<TrackEntry> second_track_entry;

      if (test_desc.mix_blend)
      {
         second_track_entry.reset (new TrackEntry (animation_state.SetAnimation (1, skeleton_data.AnimationName (1), true)));

         second_track_entry->SetMixBlend (MixBlend_Add);
         second_track_entry->SetHoldPrevious (true);
      }

      printf ("Track entries before update:\n");
      print (track_entry);
      print (empty_animation_entry);

      if (second_track_entry)
        print (*second_track_entry);

      animation_state.Update (0.5f);
      skeleton.Apply (animation_state);

      printf ("Track entries after update:\n");
      print (track_entry);
      print (empty_animation_entry);

      if (second_track_entry)
        print (*second_track_entry);

      printf ("After animation:\n");

      skeleton.UpdateWorldTransform ();

      print (skeleton);

      //update to animation end
      for (int i = 0; i < 3 && !track_entry.IsDisposed (); i++)
      {
        animation_state.Update (track_entry.Duration ());
        skeleton.Apply (animation_state);
      }

      //track entry should be disposed
      printf ("Track entries after dispose:\n");
      print (track_entry);
      print (empty_animation_entry);

      //test invalid animations
      try
      {
        animation_state.SetAnimation (0, "SOME_INVALID_ANIMATION", false);
      }
      catch (xtl::exception& e)
      {
        printf ("Exception: '%s'\n", e.what ());
      }

      try
      {
        animation_state.EnqueueAnimation (0, "SOME_INVALID_ANIMATION", false, 0);
      }
      catch (xtl::exception& e)
      {
        printf ("Exception: '%s'\n", e.what ());
      }
    }
    catch (xtl::exception& e)
    {
      printf ("Exception while testing spine file '%s': '%s'\n", test_desc.spine_file_name, e.what ());
    }
  }

  return 0;
}
