#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

///Spine skeleton visual structure builder controller
struct SpineSkeletonVisualStructureBuilder::Impl : public xtl::instance_counter<SpineSkeletonVisualStructureBuilder>
{
  typedef stl::hash_map<size_t, DynamicMesh::Pointer> MeshesMap;

  SpineSkeleton* skeleton;               //skeleton scene graph object to build meshes for
  float          meshes_min_z_including; //z coordinate used for first mesh
  float          meshes_z_range;         //z coordinate used for last mesh
  MeshesMap      meshes;                 //child meshes scene graph objects

  Impl (SpineSkeleton& in_skeleton, float in_meshes_min_z_including, float in_meshes_max_z_excluding)
    : skeleton (&in_skeleton)
    , meshes_min_z_including (in_meshes_min_z_including)
    , meshes_z_range (in_meshes_max_z_excluding - in_meshes_min_z_including)
    {}

  void Update ()
  {
    if (!skeleton->IsVisualStructureDirty ())
      return;

    const media::spine::Skeleton& spine_skeleton = skeleton->Skeleton ();

    stl::hash_set<size_t> visible_meshes;

    float current_z = meshes_min_z_including,
          z_step    = meshes_z_range / spine_skeleton.MeshesCount ();

    for (unsigned int i = 0, count = spine_skeleton.MeshesCount (); i < count; i++, current_z += z_step)
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

      iter->second->SetPosition (0, 0, current_z);
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
SpineSkeletonVisualStructureBuilder::SpineSkeletonVisualStructureBuilder (SpineSkeleton& skeleton, float meshes_min_z_including, float meshes_max_z_excluding)
  : Controller (skeleton, ControllerTimeMode_Absolute)
  , impl (new Impl (skeleton, meshes_min_z_including, meshes_max_z_excluding))
  {}

SpineSkeletonVisualStructureBuilder::~SpineSkeletonVisualStructureBuilder ()
{
}

/*
   Create controller
*/

SpineSkeletonVisualStructureBuilder::Pointer SpineSkeletonVisualStructureBuilder::Create (SpineSkeleton& skeleton, float meshes_min_z_including, float meshes_max_z_excluding)
{
  return Pointer (new SpineSkeletonVisualStructureBuilder (skeleton, meshes_min_z_including, meshes_max_z_excluding), false);
}

/*
    Update
*/

void SpineSkeletonVisualStructureBuilder::Update (const TimeValue& value)
{
  impl->Update ();
}
