#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

///Spine skeleton visual structure builder controller
struct SpineSkeletonVisualStructureBuilder::Impl : public xtl::instance_counter<SpineSkeletonVisualStructureBuilder>
{
  typedef stl::hash_map<size_t, DynamicMesh::Pointer> MeshesMap;

  SpineSkeleton* skeleton;
  MeshesMap      meshes;

  Impl (SpineSkeleton& in_skeleton)
    : skeleton (&in_skeleton)
    {}

  void Update ()
  {
    if (!skeleton->IsVisualStructureDirty ())
      return;

    const media::spine::Skeleton& spine_skeleton = skeleton->Skeleton ();

    stl::hash_set<size_t> visible_meshes;

    for (unsigned int i = 0, count = spine_skeleton.MeshesCount (); i < count; i++)
    {
      const media::geometry::Mesh& mesh = spine_skeleton.Mesh (i);

      visible_meshes.insert (mesh.Id ());

      MeshesMap::iterator iter = meshes.find (mesh.Id ());

      if (iter == meshes.end ())
      {
        DynamicMesh::Pointer dynamic_mesh = DynamicMesh::Create (mesh);

        dynamic_mesh->BindToParent (*skeleton);

        iter = meshes.insert_pair (mesh.Id (), dynamic_mesh).first;
      }
      else
      {
        if (!iter->second->IsVisible ())
          iter->second->SetVisible (true);
      }
    }

    if (visible_meshes.size () != meshes.size ())  //current skeleton state has fewer meshes count than total meshes count, some meshes should be set to invisible state
    {
      //check for each visible mesh if it should be set to invisible state
      for (MeshesMap::iterator iter = meshes.begin (), end = meshes.end (); iter != end; ++iter)
      {
        if (iter->second->IsVisible () && visible_meshes.find (iter->second->Mesh ().Id ()) == visible_meshes.end ())
        {
          iter->second->SetVisible (false);
        }
      }
    }

    skeleton->SetVisualStructureDirty (false);
  }
};

///Constructor / destructor
SpineSkeletonVisualStructureBuilder::SpineSkeletonVisualStructureBuilder (SpineSkeleton& skeleton)
  : Controller (skeleton, ControllerTimeMode_Absolute)
  , impl (new Impl (skeleton))
  {}

SpineSkeletonVisualStructureBuilder::~SpineSkeletonVisualStructureBuilder ()
{
}

/*
   Create controller
*/

SpineSkeletonVisualStructureBuilder::Pointer SpineSkeletonVisualStructureBuilder::Create (SpineSkeleton& skeleton)
{
  return Pointer (new SpineSkeletonVisualStructureBuilder (skeleton), false);
}

/*
    Update
*/

void SpineSkeletonVisualStructureBuilder::Update (const TimeValue& value)
{
  impl->Update ();
}
