#include <cstdio>

#include <common/strlib.h>

#include <spine/Atlas.h>
#include <spine/AtlasAttachmentLoader.h>
#include <spine/extension.h>
#include <spine/SkeletonJson.h>

using namespace SPINE_NAMESPACE_NAME;

namespace SPINE_NAMESPACE_NAME
{

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path)
{
  self->rendererObject = (void*)(rand() % 1000);

  printf ("_spAtlasPage_createTexture called with path '%s', created renderer object %p\n", path, self->rendererObject);
}

void _spAtlasPage_disposeTexture (spAtlasPage* self)
{
  printf ("_spAtlasPage_disposeTexture called with renderer object %p\n", self->rendererObject);
}

char* _spUtil_readFile (const char* path, int* length)
{
  printf ("_spUtil_readFile called with path %s and length %p\n", path, length);

  return _spReadFile(path, length);
}

}

struct TestDesc
{
  const char* spine_file_name;
  const char* atlas_file_name;
};

TestDesc TEST_DESCS [] = { { "data/vine/export/vine-pro.json", "data/vine/export/vine.atlas" } };

void print (spAtlasPage* page)
{
  printf ("  Atlas Page '%s':\n", page->name);
  printf ("    width: %d\n", page->width);
  printf ("    height: %d\n", page->height);
  printf ("    renderer object: %p\n", page->rendererObject);
  printf ("    wrap mode: %d x %d\n", page->uWrap, page->vWrap);
}

void print (spAtlasRegion* region)
{
  printf ("  Atlas Region '%s':\n", region->name);
  printf ("    width: %d\n", region->width);
  printf ("    height: %d\n", region->height);
}

void print (spAtlas* atlas)
{
  printf ("Atlas renderer object: %p\n", atlas->rendererObject);

  printf ("Atlas pages:\n");

  for (spAtlasPage* page = atlas->pages; page; page = page->next)
    print (page);

  printf ("Atlas regions:\n");

  for (spAtlasRegion* region = atlas->regions; region; region = region->next)
    print (region);
}

void print (spTimeline* timeline)
{
  printf ("        timeline type %d\n", timeline->type);
}

void print (spAnimation* animation)
{
  printf ("    Animation '%s':\n", animation->name);
  printf ("      duration: %f:\n", animation->duration);

  printf ("      timelines (count %d):\n", animation->timelinesCount);

  for (int i = 0; i < animation->timelinesCount; i++)
    print (animation->timelines [i]);
}

void print (spBoneData* bone)
{
  printf ("    Bone '%s':\n", bone->name);
  printf ("      length %.2f\n", bone->length);
  printf ("      rotation %.2f\n", bone->rotation);
  printf ("      scale %.2f x %.2f\n", bone->scaleX, bone->scaleY);
  printf ("      shear %.2f x %.2f\n", bone->shearX, bone->shearY);
  printf ("      position %.2f x %.2f\n", bone->x, bone->y);
  printf ("      transformMode %d\n", bone->transformMode);
}

void print (spSkin* skin)
{
  printf ("    Skin '%s'\n", skin->name);
}

void print (spEventData* event)
{
  printf ("    Event '%s': '%s' %d %f\n", event->name, event->stringValue, event->intValue, event->floatValue);
}

void print (spSlotData* slot)
{
  printf ("    Slot '%s':\n", slot->name);
  printf ("      attachmentName '%s':\n", slot->attachmentName);
  printf ("      blendMode %d\n", slot->blendMode);
  printf ("      color %.2f %.2f %.2f %.2f\n", slot->color.r, slot->color.g, slot->color.b, slot->color.a);

  if (slot->darkColor)
    printf ("      darkColor %.2f %.2f %.2f %.2f:\n", slot->darkColor->r, slot->darkColor->g, slot->darkColor->b, slot->darkColor->a);
}

void print (spSkeletonData* skeleton_data)
{
  printf ("Skeleton Data v'%s':\n", skeleton_data->version);

  printf ("  Size %.2f x %.2f\n", skeleton_data->width, skeleton_data->height);

  printf ("  Animations (count %d):\n", skeleton_data->animationsCount);

  for (int i = 0; i < skeleton_data->animationsCount; i++)
    print (skeleton_data->animations [i]);

  printf ("  Bones (count %d):\n", skeleton_data->bonesCount);

  for (int i = 0; i < skeleton_data->bonesCount; i++)
    print (skeleton_data->bones[i]);

  if (skeleton_data->defaultSkin)
  {
    printf ("  Default skin:\n");
    print (skeleton_data->defaultSkin);
  }

  printf ("  Skins (count %d):\n", skeleton_data->skinsCount);

  for (int i = 0; i < skeleton_data->skinsCount; i++)
    print (skeleton_data->skins [i]);

  printf ("  Events (count %d):\n", skeleton_data->eventsCount);

  for (int i = 0; i < skeleton_data->eventsCount; i++)
    print (skeleton_data->events [i]);

  printf ("  Slots (count %d):\n", skeleton_data->slotsCount);

  for (int i = 0; i < skeleton_data->slotsCount; i++)
    print (skeleton_data->slots [i]);
}

void print_offset (int offset)
{
  for (int i = 0; i < offset; i++)
    printf ("  ");
}

void print (spBone* bone, int offset)
{
  print_offset (offset);
  printf ("bone '%s':\n", bone->data->name);

  spBone_updateAppliedTransform(bone);

  print_offset (offset);
  printf ("  applied transform: rotation %.2f, scale %.2f x %.2f, shear %.2f x %.2f, translation %.2f x %.2f\n", bone->arotation, bone->ascaleX, bone->ascaleY, bone->ashearX, bone->ashearY, bone->ax, bone->ay);

  print_offset (offset);
  printf ("          transform: rotation %.2f, scale %.2f x %.2f, shear %.2f x %.2f, translation %.2f x %.2f\n", bone->rotation, bone->scaleX, bone->scaleY, bone->shearX, bone->shearY, bone->x, bone->y);

  spBone_updateWorldTransform(bone);

  print_offset (offset);
  printf ("  world   transform: rotation x %.2f, rotation y %.2f, scale %.2f x %.2f, translation %.2f x %.2f\n", spBone_getWorldRotationX (bone), spBone_getWorldRotationY (bone), spBone_getWorldScaleX (bone), spBone_getWorldScaleY (bone), bone->worldX, bone->worldY);

  if (bone->childrenCount)
  {
    print_offset (offset);
    printf ("  children bones (count %d):\n", bone->childrenCount);

    for (int i = 0; i < bone->childrenCount; i++)
      print (bone->children [i], offset + 2);
  }
}

void print (spSlot* slot)
{
  printf ("    slot '%s':\n", slot->data->name);

  printf ("      color %.2f %.2f %.2f %.2f\n", slot->color.r, slot->color.g, slot->color.b, slot->color.a);

  if (slot->darkColor)
    printf ("      darkColor %.2f %.2f %.2f %.2f:\n", slot->darkColor->r, slot->darkColor->g, slot->darkColor->b, slot->darkColor->a);

  printf ("      attachment vertices count %d\n", slot->attachmentVerticesCount);

  if (slot->attachment)
  {
    printf ("      attachment type %d\n", slot->attachment->type);

    switch (slot->attachment->type)
    {
      case SP_ATTACHMENT_REGION:
      {
        spRegionAttachment* region_attachment = (spRegionAttachment*)slot->attachment;

        printf ("      region attachment data:\n");
        printf ("        renderer object %p\n", region_attachment->rendererObject);

        float vertices_data [4 * 2];

        spRegionAttachment_computeWorldVertices (region_attachment, slot->bone, vertices_data, 0, 2);

        printf ("        vertices: %.2f x %.2f, %.2f x %.2f, %.2f x %.2f, %.2f x %.2f\n", vertices_data [0], vertices_data [1], vertices_data [2], vertices_data [3], vertices_data [4], vertices_data [5], vertices_data [6], vertices_data [7]);
        printf ("        uvs: %.2f x %.2f, %.2f x %.2f, %.2f x %.2f, %.2f x %.2f\n", region_attachment->uvs [0], region_attachment->uvs [1], region_attachment->uvs [2], region_attachment->uvs [3], region_attachment->uvs [4], region_attachment->uvs [5], region_attachment->uvs [6], region_attachment->uvs [7]);

        break;
      }
      case SP_ATTACHMENT_MESH:
      {
        spMeshAttachment* mesh_attachment = (spMeshAttachment*)slot->attachment;

        printf ("      mesh attachment data:\n");
        printf ("        renderer object %p\n", mesh_attachment->rendererObject);

        int    vertices_count = mesh_attachment->super.worldVerticesLength;
        float* vertices_data  = new float [vertices_count * 2];

        spVertexAttachment_computeWorldVertices (&mesh_attachment->super, slot, 0, vertices_count, vertices_data, 0, 2);

        printf ("triangles (count %d):\n", mesh_attachment->trianglesCount);

        for (int i = 0; i < mesh_attachment->trianglesCount; i++)
          printf (" %d, ", mesh_attachment->triangles[i]);

        printf ("\n");

        printf ("vertices (count %d):\n", vertices_count);

        printf ("positions:\n");

        for (int i = 0; i < vertices_count; i += 2)
          printf (" %.2f x %.2f,", vertices_data [i], vertices_data [i + 1]);

        printf ("\n");

        printf ("uvs:\n");

        for (int i = 0; i < vertices_count; i += 2)
          printf (" %.2f x %.2f,", mesh_attachment->uvs [i], mesh_attachment->uvs [i + 1]);

        printf ("\n");

        delete [] vertices_data;

        break;
      }
      default:
        printf ("NOT IMPLEMENTED\n");
        break;
    }
  }
}

void print (spSkeleton* skeleton)
{
  printf ("Skeleton:\n");

  printf ("  color %.2f %.2f %.2f %.2f\n", skeleton->color.r, skeleton->color.g, skeleton->color.b, skeleton->color.a);

  printf ("  draw order (slots count %d):\n", skeleton->slotsCount);

  for (int i = 0; i < skeleton->slotsCount; i++)
    print (skeleton->drawOrder [i]);

  printf ("  time %.f\n", skeleton->time);
  printf ("  position %.2f x %.2f\n", skeleton->x, skeleton->y);

  if (skeleton->root)
  {
    printf ("  bones:");
    print (skeleton->root, 2);
  }
}

int main ()
{
  printf ("Results of native spine test:\n");

  for (size_t i = 0, count = sizeof (TEST_DESCS) / sizeof (*TEST_DESCS); i < count; i++)
  {
    const TestDesc& test_desc = TEST_DESCS[i];

    printf ("Testing spine file '%s':\n", test_desc.spine_file_name);

    spAtlas* atlas = spAtlas_createFromFile (test_desc.atlas_file_name, (void*)15);

    print (atlas);

//    spAtlasAttachmentLoader* atlas_attachment_loader = spAtlasAttachmentLoader_create (atlas);

    spSkeletonJson* skeleton_json = spSkeletonJson_create (atlas);

    spSkeletonData* skeleton_data = spSkeletonJson_readSkeletonDataFile (skeleton_json, test_desc.spine_file_name);

    spSkeletonJson_dispose (skeleton_json);

    print (skeleton_data);

    spSkeleton* skeleton = spSkeleton_create (skeleton_data);

    spSkeleton_updateWorldTransform (skeleton);

    print (skeleton);

    spAnimationStateData* animation_state_data = spAnimationStateData_create (skeleton_data);

    spAnimationState* animation_state = spAnimationState_create (animation_state_data);

    spAnimationState_setAnimationByName (animation_state, 0, skeleton_data->animations[0]->name, true);

    spAnimationState_update (animation_state, 0.5f);
    spAnimationState_apply (animation_state, skeleton);
    spSkeleton_updateWorldTransform (skeleton);

    printf ("After animation:\n");
    print (skeleton);

    spAnimationState_dispose (animation_state);
    spAnimationStateData_dispose (animation_state_data);

    spSkeletonData_dispose (skeleton_data);

  //???? no such method - memory leak???  spAttachmentLoader_dispose (atlas_attachment_loader);

    spAtlas_dispose (atlas);
  }

  return 0;
}
