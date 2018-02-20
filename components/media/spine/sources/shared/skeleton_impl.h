#ifndef MEDIALIB_SPINE_SHARED_SKELETON_HEADER
#define MEDIALIB_SPINE_SHARED_SKELETON_HEADER

#include <stl/hash_map>
#include <stl/vector>

#include <xtl/intrusive_ptr.h>
#include <xtl/shared_ptr.h>

#include <media/spine/skeleton.h>

#include <material_impl.h>
#include <object.h>

namespace media
{

namespace spine
{

//forward declarations
class AnimationStateImpl;
class BoneImpl;
class SkeletonClippingImpl;
class SlotImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Spine animation state data
///////////////////////////////////////////////////////////////////////////////////////////////////
class SkeletonImpl : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    SkeletonImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Animating
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetToSetupPose      () = 0;
    virtual void ApplyAnimationState (AnimationStateImpl* animation) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with bones
///////////////////////////////////////////////////////////////////////////////////////////////////
            Bone&        RootBone       ();
            Bone&        Bone           (unsigned int index);
    virtual unsigned int BonesCount     () = 0;
    virtual BoneImpl*    CreateBoneImpl (unsigned int index) = 0;
    virtual int          RootBoneIndex  () = 0;
    virtual int          FindBoneIndex  (const char* name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with slots
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int        SlotsCount             () = 0;
    virtual SlotImpl*           CreateSlotForDrawOrder (unsigned int index) = 0;
            media::spine::Slot& Slot                   (unsigned int index);
    virtual SlotImpl*           CreateSlotImpl         (unsigned int index) = 0;
    virtual int                 FindSlotIndex          (const char* name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Rendering
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int                  MeshesCount ();
    geometry::Mesh                Mesh        (unsigned int mesh_index);
    const media::spine::Material& Material    (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Change skin (use 0 to set default skin)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const char* Skin          () = 0;
    virtual bool        SetSkin       (const char* skin_name) = 0;
    virtual bool        SetAttachment (const char* slot_name, const char* attachment_name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Update transform after animation/bones manipulation
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateWorldTransform () = 0;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Helper methods
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual MaterialImpl*         CreateMaterialImpl (const char* material_name, const char* texture_path, BlendMode blend_mode, TexcoordWrap texcoord_wrap_u, TexcoordWrap texcoord_wrap_v) = 0;
    virtual SkeletonClippingImpl* Clipper            () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Setup methods
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ResizeSlots      (unsigned int size);
    void ResizeBones      (unsigned int size);
    void InvalidateMeshes ();
    void BuildMeshes      ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Helper methods
///////////////////////////////////////////////////////////////////////////////////////////////////
    geometry::Mesh CreateMesh         ();
    float*         AddMeshToDrawOrder (unsigned int next_triangle_list_mesh_to_use, SlotImpl* slot, AttachmentImpl* attachment,
                                       unsigned int triangles_count, unsigned int vertices_count, const uint16_t* source_indices,
                                       const float* attachment_texcoords);  //returns pointer to position vertices

  private:
    struct MeshDesc
    {
      geometry::Mesh mesh;
      math::vec4f    color;
      size_t         index_buffer_hash;
      size_t         texcoords_hash;

      MeshDesc (const geometry::Mesh& in_mesh)
        : mesh (in_mesh)
        , index_buffer_hash ()
        , texcoords_hash ()
        {}
    };

    typedef xtl::shared_ptr<media::spine::Slot>                               SlotPtr;
    typedef xtl::shared_ptr<media::spine::Bone>                               BonePtr;
    typedef xtl::com_ptr<MaterialImpl>                                        MaterialImplPtr;
    typedef stl::vector<SlotPtr>                                              SlotsArray;
    typedef stl::vector<BonePtr>                                              BonesArray;
    typedef stl::vector<geometry::Mesh>                                       MeshesArray;
    typedef stl::vector<MeshDesc>                                             MeshDescsArray;
    typedef stl::hash_map<stl::hash_key<const char*>, media::spine::Material> MaterialsMap;

  private:
    BonesArray     bones;                //array of bones for returning media objects
    SlotsArray     slots;                //array of slots for returning media objects
    MeshesArray    sprites_meshes;       //array of meshes used for sprites attachments
    MeshDescsArray triangle_list_meshes; //array of meshes used for mesh attachments
    MeshesArray    draw_order;           //meshes array in draw order
    MaterialsMap   materials;            //materials
};

}

}

#endif
