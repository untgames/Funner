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
            media::spine::Bone& RootBone       ();
            media::spine::Bone& Bone           (unsigned int index);
    virtual unsigned int        BonesCount     () = 0;
    virtual BoneImpl*           CreateBoneImpl (unsigned int index) = 0;
    virtual int                 RootBoneIndex  () = 0;
    virtual int                 FindBoneIndex  (const char* name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Work with slots
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int        SlotsCount             () = 0;
    virtual SlotImpl*           CreateSlotForDrawOrder (unsigned int index) = 0;
            media::spine::Slot& Slot                   (unsigned int index);
    virtual SlotImpl*           CreateSlotImpl         (unsigned int index) = 0;
    virtual int                 FindSlotIndex          (const char* name) = 0;
    virtual int                 SlotForDrawOrder       (unsigned int index) = 0;

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
    struct MeshDesc : public xtl::reference_counter
    {
      geometry::Mesh mesh;
      stl::string    material;
      unsigned int   triangles_count;
      math::vec4f    color;
      size_t         index_buffer_hash;
      size_t         texcoords_hash;

      MeshDesc (const geometry::Mesh& in_mesh)
        : mesh (in_mesh)
        , triangles_count ()
        , index_buffer_hash ()
        , texcoords_hash ()
        {}
    };

    struct PrimitiveDesc : public xtl::reference_counter
    {
      uint32_t     count;
      uint32_t     base_vertex;
      BlendMode    blend_mode;
      TexcoordWrap texcoord_wrap_u;
      TexcoordWrap texcoord_wrap_v;
      stl::string  texture_path;

      PrimitiveDesc (uint32_t in_count, uint32_t in_base_vertex, BlendMode in_blend_mode, TexcoordWrap in_texcoord_wrap_u, TexcoordWrap in_texcoord_wrap_v, const char* in_texture_path)
        : count (in_count)
        , base_vertex (in_base_vertex)
        , blend_mode (in_blend_mode)
        , texcoord_wrap_u (in_texcoord_wrap_u)
        , texcoord_wrap_v (in_texcoord_wrap_v)
        , texture_path (in_texture_path)
        {}

      bool operator == (const PrimitiveDesc& right) const
      {
        return count == right.count && base_vertex == right.base_vertex && blend_mode == right.blend_mode && texcoord_wrap_u == right.texcoord_wrap_u &&
               texcoord_wrap_v == right.texcoord_wrap_v && texture_path == right.texture_path;
      }

      bool operator != (const PrimitiveDesc& right) const
      {
        return !(*this == right);
      }

    };

    typedef xtl::intrusive_ptr<PrimitiveDesc> PrimitiveDescPtr;
    typedef stl::vector<PrimitiveDescPtr>     PrimitiveDescsArray;

    struct SpritesMeshDesc : public xtl::reference_counter
    {
      geometry::Mesh      mesh;
      size_t              color_texcoords_hash;
      PrimitiveDescsArray primitive_descs;

      SpritesMeshDesc (const geometry::Mesh& in_mesh)
        : mesh (in_mesh)
        , color_texcoords_hash ()
        {}
    };

    typedef xtl::shared_ptr<media::spine::Slot>                               SlotPtr;
    typedef xtl::shared_ptr<media::spine::Bone>                               BonePtr;
    typedef xtl::com_ptr<MaterialImpl>                                        MaterialImplPtr;
    typedef stl::vector<SlotPtr>                                              SlotsArray;
    typedef stl::vector<BonePtr>                                              BonesArray;
    typedef stl::vector<media::geometry::Mesh>                                MeshesArray;
    typedef xtl::intrusive_ptr<SpritesMeshDesc>                               SpritesMeshDescPtr;
    typedef stl::vector<SpritesMeshDescPtr>                                   SpritesMeshDescsArray;
    typedef xtl::intrusive_ptr<MeshDesc>                                      MeshDescPtr;
    typedef stl::vector<MeshDescPtr>                                          MeshDescsArray;
    typedef stl::hash_map<stl::hash_key<const char*>, media::spine::Material> MaterialsMap;

  private:
    bool                  clipping_warning_reported; //we have sent message to log that skeleton contains clipping and it may work slow
    bool                  meshes_warning_reported;   //we have sent message to log that skeleton contains meshes and it may work slow
    BonesArray            bones;                     //array of bones for returning media objects
    SlotsArray            slots;                     //array of slots for returning media objects
    SpritesMeshDescsArray sprites_meshes;            //array of meshes used for sprites attachments
    MeshDescsArray        triangle_list_meshes;      //array of meshes used for mesh attachments
    MeshesArray           draw_order;                //meshes array in draw order
    MaterialsMap          materials;                 //materials
};

}

}

#endif
