#include "shared.h"

const char* LIBRARY_NAMES [] = { "data/particle_designer/fire big.plist", "data/particle_designer/fireBall_keno_all-scene.plist" };

void print_log (const char* log, const char* message)
{
  stl::string filtered_message;

  filtered_message.reserve (xtl::xstrlen (message));

  common::StringArray message_lines = common::split (message, "\n", "");

  for (size_t i = 0, count = message_lines.Size (); i < count; i++)
  {
    const char* line = message_lines [i];

    if (i)
      filtered_message += "\n";

    if (common::wcmatch (line, "*common::*::FileOpen*"))
      filtered_message += "    at common::FILE_SYSTEM::FileOpen";
    else
      filtered_message += line;
  }

  printf ("%s: %s\n", log, filtered_message.c_str ());
}

int main ()
{
  printf ("Results of particle_designer_emitter_loader_test:\n");

  LogFilter log_filter ("*", &print_log);

  for (size_t current_library = 0, count = sizeof (LIBRARY_NAMES) / sizeof (*LIBRARY_NAMES); current_library < count; current_library++)
  {
    printf ("\nTesting library '%s':\n", LIBRARY_NAMES [current_library]);

    try
    {
      ParticleSystemLibrary library (LIBRARY_NAMES [current_library]);

      printf ("  name: '%s'\n", library.Name ());
      printf ("  size: %u\n", library.Size ());

      RandomGenerator random;

      for (ParticleSystemLibrary::ConstIterator iter = library.CreateIterator (); iter; ++iter)
      {
        ParticleSystem system (random);

        (*iter)->Configure (system);

        printf ("Testing system '%s':\n", library.ItemId (iter));
        printf ("Scenes count %u:\n", system.ScenesCount ());

        for (size_t i = 0, scenes_count = system.ScenesCount (); i < scenes_count; i++)
        {
          ParticleScene& scene = system.Scene (i);

          printf ("Scene %u name is '%s'\n", i, scene.Name ());
          printf ("  Material '%s':\n", scene.MaterialName ());
          printf ("  Offset is %.2f %.2f %.2f\n", scene.Offset ().x, scene.Offset ().y, scene.Offset ().z);
          printf ("  Animation frames per second %.3f:\n", scene.AnimationFramesPerSecond ());
          printf ("  Animation frames count %u:\n", scene.AnimationFramesCount ());

          for (size_t j = 0, frames_count = scene.AnimationFramesCount (); j < frames_count; j++)
          {
            const ParticleTexDesc& tex_desc = scene.AnimationFrames () [j];

            printf ("    Frame %u offset = [%.3f; %.3f] size = [%.3f; %.3f]\n", j, tex_desc.tex_offset.x, tex_desc.tex_offset.y, tex_desc.tex_size.x, tex_desc.tex_size.y);
          }
        }

        printf ("simulation:\n");
        TimeValue tv;

        for (size_t i = 0; i < 4; i++, tv += 1)
        {
          system.Update (tv);

          for (size_t j = 0, scenes_count = system.ScenesCount (); j < scenes_count; j++)
          {
            bound_volumes::aaboxf box = system.Scene (j).BoundBox ();

            printf ("  scene %zu aabb(%.2f): [%.3f; %.3f; %.3f] - [%.3f; %.3f; %.3f]\n", j, tv.cast<float> (), box.minimum ().x, box.minimum ().y, box.minimum ().z,
                    box.maximum ().x, box.maximum ().y, box.maximum ().z);
          }
        }

        printf ("simulation with spawning disabled:\n");

        tv += 10;

        common::PropertyMap processing_properties;

        processing_properties.SetProperty ("particles.ParticleDesigner.EmitCountMultiplier", 0.f);

        system.Update (tv, &processing_properties);

        for (size_t j = 0, scenes_count = system.ScenesCount (); j < scenes_count; j++)
        {
          bound_volumes::aaboxf box = system.Scene (j).BoundBox ();

          printf ("  scene %zu aabb(%.2f): [%.3f; %.3f; %.3f] - [%.3f; %.3f; %.3f]\n", j, tv.cast<float> (), box.minimum ().x, box.minimum ().y, box.minimum ().z,
                  box.maximum ().x, box.maximum ().y, box.maximum ().z);
        }
      }
    }
    catch (std::exception& e)
    {
      printf ("exception: %s\n", e.what ());
    }
  }
}
