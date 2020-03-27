#include "shared.h"

const char* SPINE_FILE_NAME  = "data/vine/export/vine-pro.json";
const char* SPINE_ATLAS_NAME = "data/vine/export/vine.atlas";

int main ()
{
  printf ("Results of spine_skeleton_animation_test:\n");

  try
  {
    media::spine::SkeletonData skeleton_data (SPINE_FILE_NAME, SPINE_ATLAS_NAME);
    media::spine::Skeleton skeleton1 = skeleton_data.CreateSkeleton (), skeleton2 = skeleton_data.CreateSkeleton ();

    SpineSkeleton::Pointer model1 (SpineSkeleton::Create (skeleton1));
    SpineSkeleton::Pointer model2 (SpineSkeleton::Create (skeleton2));
    Node::Pointer          node = Node::Create ();
  
    model2->BindToParent (*model1);
    node->BindToParent (*model1);

    printf ("Is model1 visual structure dirty: %d\n", model1->IsVisualStructureDirty ());
    printf ("Is model2 visual structure dirty: %d\n", model2->IsVisualStructureDirty ());

    model1->SetVisualStructureDirty (false);
    model2->SetVisualStructureDirty (false);

    printf ("Is model1 visual structure dirty: %d\n", model1->IsVisualStructureDirty ());
    printf ("Is model2 visual structure dirty: %d\n", model2->IsVisualStructureDirty ());

    //add animation
    media::spine::AnimationStateData animation_state_data = skeleton_data.CreateAnimationStateData ();
    media::spine::AnimationState     animation_state      = animation_state_data.CreateAnimationState ();

    SpineAnimation::Pointer animation = SpineAnimation::Create (*model1, animation_state);

    printf ("Update with animation\n");

    model1->Update (0);

    printf ("Is model1 visual structure dirty: %d\n", model1->IsVisualStructureDirty ());
    printf ("Is model2 visual structure dirty: %d\n", model2->IsVisualStructureDirty ());

    model1->SetVisualStructureDirty (false);
    model2->SetVisualStructureDirty (false);

    node->Unbind ();
    model2->Unbind ();

    printf ("Update with animation2\n");

    model1->Update (1);

    printf ("Is model1 visual structure dirty: %d\n", model1->IsVisualStructureDirty ());
    printf ("Is model2 visual structure dirty: %d\n", model2->IsVisualStructureDirty ());

    model1->SetVisualStructureDirty (false);
    model2->SetVisualStructureDirty (false);

    model2->BindToParent (*model1);

    printf ("Update with animation3\n");

    model1->Update (2);

    printf ("Is model1 visual structure dirty: %d\n", model1->IsVisualStructureDirty ());
    printf ("Is model2 visual structure dirty: %d\n", model2->IsVisualStructureDirty ());

    SpineSkeletonVisualStructureBuilder::Pointer visual_structure_builder1 = SpineSkeletonVisualStructureBuilder::Create (*model1, 1.f, 2.f);
    SpineSkeletonVisualStructureBuilder::Pointer visual_structure_builder2 = SpineSkeletonVisualStructureBuilder::Create (*model2, 2.f, 3.f);

    printf ("Update with animation and builders\n");

    model1->Update (3);

    printf ("Is model1 visual structure dirty: %d\n", model1->IsVisualStructureDirty ());
    printf ("Is model2 visual structure dirty: %d\n", model2->IsVisualStructureDirty ());

    printf ("model1 children positions:\n");

    for (Node::Pointer child = model1->FirstChild (); child; child = child->NextChild ())
      printf ("  position = %.2f %.2f %.2f\n", child->Position ().x, child->Position ().y, child->Position ().z);

    printf ("model2 children positions:\n");

    for (Node::Pointer child = model2->FirstChild (); child; child = child->NextChild ())
      printf ("  position = %.2f %.2f %.2f\n", child->Position ().x, child->Position ().y, child->Position ().z);
  }
  catch (xtl::exception& e)
  {
    printf ("Exception caught: '%s'\n", e.what ());
  }

  return 0;
}
