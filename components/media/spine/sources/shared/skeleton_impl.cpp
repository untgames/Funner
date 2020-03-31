#include <stdint.h>

#include <xtl/common_exceptions.h>

#include <common/log.h>
#include <common/strlib.h>

#include <media/geometry/mesh.h>

#include "attachment_impl.h"
#include "bone_impl.h"
#include "skeleton_clipping_impl.h"
#include "skeleton_impl.h"
#include "slot_impl.h"
#include "wrappers.h"

using namespace media::spine;

namespace
{

//constants
const unsigned int DRAW_ORDER_MESHES_RESERVE_COUNT    = 4;                          //reserve count for draw order meshes array
const char*        LOG_NAME                           = "media.spine.SkeletonImpl"; //log stream name
const unsigned int MAX_BATCH_SIZE                     = 65536 / 4;                  //max batch size to fit in index value range
const unsigned int PRIMITIVES_RESERVE_COUNT           = 4;                          //reserve count for sprites meshes primitives array
const unsigned int SPRITES_MESHES_RESERVE_COUNT       = 4;                          //reserve count for sprites meshes array
const unsigned int TRIANGLE_LIST_MESHES_RESERVE_COUNT = 4;                          //reserve count for triangle list meshes array

}

/*
   Constructor
*/

SkeletonImpl::SkeletonImpl ()
  : clipping_warning_reported (false)
  , meshes_warning_reported (false)
{
  sprites_meshes.reserve (SPRITES_MESHES_RESERVE_COUNT);
  triangle_list_meshes.reserve (TRIANGLE_LIST_MESHES_RESERVE_COUNT);
  draw_order.reserve (DRAW_ORDER_MESHES_RESERVE_COUNT);
}

/*
   Setup methods
*/

void SkeletonImpl::ResizeSlots (unsigned int size)
{
  slots.clear   ();
  slots.resize (size);
}

void SkeletonImpl::ResizeBones (unsigned int size)
{
  bones.clear  ();
  bones.resize (size);
}

void SkeletonImpl::InvalidateMeshes ()
{
  draw_order.clear ();
}

void SkeletonImpl::BuildMeshes ()
{
  //check if meshes are already built
  if (!draw_order.empty ())
    return;

  unsigned int slots_count = SlotsCount ();

  //build arrays of slots and attachments used for drawing
  stl::vector<xtl::com_ptr<SlotImpl> >       slots_draw_order;
  stl::vector<xtl::com_ptr<AttachmentImpl> > attachments_draw_order;

  slots_draw_order.reserve (slots_count);
  attachments_draw_order.reserve (slots_count);

  for (unsigned int i = 0; i < slots_count; i++)
  {
    xtl::com_ptr<SlotImpl>       slot       (Wrappers::Unwrap<media::spine::SlotImpl, media::spine::Slot> (Slot (SlotForDrawOrder (i))));
    xtl::com_ptr<AttachmentImpl> attachment (slot->HasAttachment () ? Wrappers::Unwrap<media::spine::AttachmentImpl, media::spine::Attachment> (*slot->Attachment ()) : 0);

    slots_draw_order.push_back (slot);
    attachments_draw_order.push_back (attachment);
  }

  //build meshes
  unsigned int next_sprite_mesh_to_use = 0,
               next_triangle_list_mesh_to_use = 0;

  SkeletonClippingImpl* clipper = Clipper ();

  xtl::uninitialized_storage<float> vertices_to_clip_buffer;

  for (unsigned int i = 0; i < slots_count; i++)
  {
    SlotImpl*       slot       = slots_draw_order [i].get ();
    AttachmentImpl* attachment = attachments_draw_order [i].get ();

    if (!attachment)
    {
      clipper->ClipEnd (slot);

      continue;
    }

    //clipping
    if (clipper->IsClipping () && (attachment->Type () == AttachmentType_Region || attachment->Type () == AttachmentType_Mesh))
    {
      //fill clip data
      vertices_to_clip_buffer.resize (attachment->VerticesCount () * 2);

      attachment->ComputeWorldVertices (slot, vertices_to_clip_buffer.data (), 0, 2);

      clipper->ClipTriangles (vertices_to_clip_buffer.data (), (int)vertices_to_clip_buffer.size (), (unsigned short*)attachment->Indices (), attachment->TrianglesCount () * 3, (float*)attachment->Texcoords (), 2);

      if (clipper->ClippedTrianglesCount ())
      {
        unsigned int vertices_count = clipper->ClippedVerticesCount ();

        float* position_vertices = AddMeshToDrawOrder (next_triangle_list_mesh_to_use++, slot, attachment, clipper->ClippedTrianglesCount (),
                                                       vertices_count, clipper->ClippedIndices (), clipper->ClippedTexcoords ());

        memcpy (position_vertices, clipper->ClippedVertices (), vertices_count * sizeof (float) * 2);
      }

      clipper->ClipEnd (slot);

      continue;
    }

    switch (attachment->Type ())
    {
      case AttachmentType_Region:
      {
        //Currently we group into batch all consecutive region attachments. We stops batch if slot has null attachment or non-region attachment type.
        //Batching can be improved if we'll skip null attachments and non-mesh attachments when clipping is not active.

        if (next_sprite_mesh_to_use >= sprites_meshes.size ())
        {
          media::geometry::Mesh mesh = CreateMesh ();

          mesh.Rename (common::format ("spine_skeleton_%p_sprites_%u", this, sprites_meshes.size ()).c_str ());

          sprites_meshes.push_back (SpritesMeshDescPtr (new SpritesMeshDesc (mesh), false));
        }

        SpritesMeshDesc&       current_sprites_mesh_desc = *sprites_meshes [next_sprite_mesh_to_use++];
        media::geometry::Mesh& current_sprites_mesh      = current_sprites_mesh_desc.mesh;

        PrimitiveDescsArray new_primitives;

        new_primitives.reserve (stl::max (PRIMITIVES_RESERVE_COUNT, (unsigned int)current_sprites_mesh_desc.primitive_descs.capacity ()));

        //calculate how many sprites mesh will contain and what will be the max batch size
        unsigned int sprites_count       = 0,
                     current_batch_size  = 0,
                     max_batch_size      = 0,
                     current_base_sprite = 0;

        BlendMode    current_batch_blend_mode      = (BlendMode)-1;
        TexcoordWrap current_batch_texcoord_wrap_u = (TexcoordWrap)-1,
                     current_batch_texcoord_wrap_v = (TexcoordWrap)-1;
        const char*  current_batch_texture_path    = "";

        for (unsigned int j = i; j < slots_count; j++)
        {
          AttachmentImpl* current_attachment = attachments_draw_order [j].get ();

          bool         batch_ended = false;
          BlendMode    slot_blend_mode;
          TexcoordWrap attachment_texcoord_wrap_u, attachment_texcoord_wrap_v;
          const char*  attachment_texture_path = 0;

          if (!current_attachment || current_attachment->Type () != AttachmentType_Region)
          {
            batch_ended = true;
          }
          else
          {
            sprites_count++;

            SlotImpl* current_slot = slots_draw_order [j].get ();

            slot_blend_mode            = current_slot->BlendMode ();
            attachment_texcoord_wrap_u = current_attachment->TexcoordWrapU ();
            attachment_texcoord_wrap_v = current_attachment->TexcoordWrapV ();
            attachment_texture_path    = current_attachment->TexturePath ();

            //check if we should end batch due to material change or max batch size reached
            if (current_batch_blend_mode != slot_blend_mode || current_batch_texcoord_wrap_u != attachment_texcoord_wrap_u ||
                current_batch_texcoord_wrap_v != attachment_texcoord_wrap_v || xtl::xstrcmp (current_batch_texture_path, attachment_texture_path) ||
                current_batch_size == MAX_BATCH_SIZE)
            {
              batch_ended = true;
            }
          }

          if (batch_ended)
          {
            if (current_batch_size > max_batch_size)
              max_batch_size = current_batch_size;

            if (current_batch_size)
            {
              //we had non-empty batch, add primitive to mesh
              new_primitives.push_back (PrimitiveDescPtr (new PrimitiveDesc (current_batch_size * 2, current_base_sprite * 4, current_batch_blend_mode, current_batch_texcoord_wrap_u, current_batch_texcoord_wrap_v, current_batch_texture_path), false));

              current_base_sprite += current_batch_size;
            }

            current_batch_size = 0;

            if (attachment_texture_path)
            {
              current_batch_blend_mode      = slot_blend_mode;
              current_batch_texcoord_wrap_u = attachment_texcoord_wrap_u;
              current_batch_texcoord_wrap_v = attachment_texcoord_wrap_v;
              current_batch_texture_path    = attachment_texture_path;
            }
          }

          if (!current_attachment || current_attachment->Type () != AttachmentType_Region)
            break;

          current_batch_size++;
        }

        if (current_batch_size)
        {
          if (current_batch_size > max_batch_size)
            max_batch_size = current_batch_size;

          //we have non-empty batch after loop, add primitive to mesh
          new_primitives.push_back (PrimitiveDescPtr (new PrimitiveDesc (current_batch_size * 2, current_base_sprite * 4, current_batch_blend_mode, current_batch_texcoord_wrap_u, current_batch_texcoord_wrap_v, current_batch_texture_path), false));
        }

        //add new data to index buffer, if required
        media::geometry::IndexBuffer& index_buffer = current_sprites_mesh.IndexBuffer ();

        unsigned int vertices_count     = sprites_count * 4,
                     indices_count      = max_batch_size * 6,  //6 indices per sprite
                     prev_indices_count = index_buffer.Size ();

        if (prev_indices_count < indices_count)
        {
          index_buffer.Resize (indices_count);

          uint16_t* indices = index_buffer.Data<uint16_t> ();

          for (unsigned int sprite_index = prev_indices_count / 6, base_index = prev_indices_count, base_vertex = sprite_index * 4;
              sprite_index < max_batch_size;
              sprite_index++, base_index += 6, base_vertex += 4)
          {
            indices[base_index]     = base_vertex;
            indices[base_index + 1] = base_vertex + 1;
            indices[base_index + 2] = base_vertex + 2;
            indices[base_index + 3] = base_vertex + 2;
            indices[base_index + 4] = base_vertex + 3;
            indices[base_index + 5] = base_vertex;
          }
        }

        media::geometry::VertexBuffer& vertex_buffer                = current_sprites_mesh.VertexBuffer (0);
        media::geometry::VertexStream& position_vertex_stream       = vertex_buffer.Stream (0),
                                       color_texcoord_vertex_stream = vertex_buffer.Stream (1);

        if (position_vertex_stream.Size () < vertices_count)
        {
          position_vertex_stream.Resize (vertices_count);
          color_texcoord_vertex_stream.Resize (vertices_count);
        }

        float* current_vertex_position       = (float*)position_vertex_stream.Data ();
        float* current_vertex_color_texcoord = (float*)color_texcoord_vertex_stream.Data ();

        for (unsigned int j = i; j < slots_count; j++)
        {
          SlotImpl*       current_slot       = slots_draw_order [j].get ();
          AttachmentImpl* current_attachment = attachments_draw_order [j].get ();

          if (!current_attachment || current_attachment->Type () != AttachmentType_Region)
            break;

          i = j;  //slot is consumed, advance i

          //fill vertices position
          current_attachment->ComputeWorldVertices (current_slot, current_vertex_position, 0, 2);

          //offset pointer by 4 vertices
          current_vertex_position += 8;   //4 vertices is 8 floats (x, y)

          //fill texcoords and color
          math::vec4f  color                = current_slot->Color () * current_attachment->Color ();
          const float* attachment_texcoords = current_attachment->Texcoords ();

          for (unsigned int k = 0; k < 4; k++, current_vertex_color_texcoord += 6, attachment_texcoords += 2)
          {
            current_vertex_color_texcoord [0] = color.x;
            current_vertex_color_texcoord [1] = color.y;
            current_vertex_color_texcoord [2] = color.z;
            current_vertex_color_texcoord [3] = color.w;
            current_vertex_color_texcoord [4] = attachment_texcoords [0];
            current_vertex_color_texcoord [5] = 1.f - attachment_texcoords [1];
          }
        }

        bool need_update_primitives = new_primitives.size () != current_sprites_mesh_desc.primitive_descs.size ();

        if (!need_update_primitives)  //save count, check actual primitives data is the same
        {
          for (PrimitiveDescsArray::iterator new_iter = new_primitives.begin (), new_end = new_primitives.end (),
                                             prev_iter = current_sprites_mesh_desc.primitive_descs.begin (), prev_end = current_sprites_mesh_desc.primitive_descs.end ();
                                             (new_iter != new_end) && (prev_iter != prev_end); ++new_iter, ++prev_iter)
          {
            PrimitiveDesc& prev_primitive_desc = **prev_iter;
            PrimitiveDesc& new_primitive_desc  = **new_iter;

            if (prev_primitive_desc != new_primitive_desc)
            {
              need_update_primitives = true;
              break;
            }
          }
        }

        if (need_update_primitives)
        {
          current_sprites_mesh.RemoveAllPrimitives ();

          for (PrimitiveDescsArray::iterator iter = new_primitives.begin (), end = new_primitives.end (); iter != end; ++iter)
          {
            PrimitiveDesc& primitive_desc = **iter;

            stl::string current_material = common::format ("spine_%s_%d_%d_%d", primitive_desc.texture_path.c_str (), primitive_desc.blend_mode, primitive_desc.texcoord_wrap_u, primitive_desc.texcoord_wrap_v);

            //add material if it was not added yet
            MaterialsMap::iterator material_iter = materials.find (current_material.c_str ());

            if (material_iter == materials.end ())
              materials.insert_pair (current_material.c_str (), Wrappers::Wrap<media::spine::Material, media::spine::MaterialImpl> (CreateMaterialImpl (current_material.c_str (), primitive_desc.texture_path.c_str (), primitive_desc.blend_mode, primitive_desc.texcoord_wrap_u, primitive_desc.texcoord_wrap_v)));

            current_sprites_mesh.AddPrimitive (media::geometry::PrimitiveType_TriangleList, 0, 0, primitive_desc.count, primitive_desc.base_vertex, current_material.c_str ());
          }

          current_sprites_mesh_desc.primitive_descs = new_primitives;
        }

        //usually data is changed in positions buffer, so do not calculate hash to check if it was actually changed, just invalidate data
        position_vertex_stream.InvalidateData ();

        size_t new_color_texcoord_hash = common::crc32 (color_texcoord_vertex_stream.Data (), color_texcoord_vertex_stream.Size () * color_texcoord_vertex_stream.VertexSize ());

        if (current_sprites_mesh_desc.color_texcoords_hash != new_color_texcoord_hash)
        {
          current_sprites_mesh_desc.color_texcoords_hash = new_color_texcoord_hash;

          color_texcoord_vertex_stream.InvalidateData ();
        }

        draw_order.push_back (current_sprites_mesh);

        break;
      }
      case AttachmentType_Mesh:
      {
        if (!meshes_warning_reported)
        {
          common::Log (LOG_NAME).Printf ("Warning: skeleton contains meshes, this feature may work slow.");
          meshes_warning_reported = true;
        }

        unsigned int vertices_count = attachment->VerticesCount ();

        float* position_vertices = AddMeshToDrawOrder (next_triangle_list_mesh_to_use++, slot, attachment, attachment->TrianglesCount (),
                                                       vertices_count, attachment->Indices (), attachment->Texcoords ());

        attachment->ComputeWorldVertices (slot, position_vertices, 0, 2);

        break;
      }
      case AttachmentType_Clipping:
        if (!clipping_warning_reported)
        {
          common::Log (LOG_NAME).Printf ("Warning: skeleton contains clipping, this feature may work slow.");
          clipping_warning_reported = true;
        }

        clipper->ClipStart (slot, attachment);
        break;
      default:
        //ignore other attachment types
        clipper->ClipEnd (slot);
        break;
    }
  }

  clipper->ClipEnd ();

  //do not erase extra meshes to free memory, to save time on memory allocation/deallocation
}

/*
   Work with slots
*/

media::spine::Slot& SkeletonImpl::Slot (unsigned int index)
{
  if (index >= slots.size ())
    throw xtl::make_range_exception ("media::spine::SkeletonImpl::Slot", "index", index, 0u, slots.size ());

  if (!slots [index])
    slots [index] = SlotPtr (Wrappers::WrapToNew<media::spine::Slot, media::spine::SlotImpl> (CreateSlotImpl (index)));

  return *slots [index];
}

Bone& SkeletonImpl::RootBone ()
{
  int index = RootBoneIndex ();

  //should never occur, check this just in case
  if (index < 0)
    throw xtl::format_operation_exception ("media::spine::SkeletonImpl::RootBone", "Can't find root bone");

  return Bone (index);
}

Bone& SkeletonImpl::Bone (unsigned int index)
{
  if (index >= bones.size ())
    throw xtl::make_range_exception ("media::spine::SkeletonImpl::Bone", "index", index, 0u, bones.size ());

  if (!bones [index])
    bones [index] = BonePtr (Wrappers::WrapToNew<media::spine::Bone, media::spine::BoneImpl> (CreateBoneImpl (index)));

  return *bones [index];
}

/*
   Rendering
*/

unsigned int SkeletonImpl::MeshesCount ()
{
  if (draw_order.empty ())
    BuildMeshes ();

  return (unsigned int)draw_order.size ();
}

media::geometry::Mesh SkeletonImpl::Mesh (unsigned int mesh_index)
{
  if (mesh_index >= MeshesCount ())
    throw xtl::make_range_exception ("media::spine::SkeletonImpl::Mesh", "mesh_index", mesh_index, (size_t)0, MeshesCount ());

  return draw_order [mesh_index];
}

const media::spine::Material& SkeletonImpl::Material (const char* name)
{
  static const char* METHOD_NAME = "media::spine::SkeletonImpl::Material";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  MaterialsMap::iterator iter = materials.find (name);

  if (iter == materials.end ())
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Can't find material");

  return iter->second;
}

/*
   Helper methods
*/

media::geometry::Mesh SkeletonImpl::CreateMesh ()
{
  media::geometry::Mesh mesh;

  media::geometry::IndexBuffer index_buffer (0, media::geometry::IndexType_UInt16);

  mesh.Attach (index_buffer);

  media::geometry::VertexBuffer vertex_buffer;
  media::geometry::VertexFormat position_vertex_format, color_texcoord_vertex_format;

  position_vertex_format.AddAttribute (media::geometry::VertexAttributeSemantic_Position, media::geometry::VertexAttributeType_Float2, 0);
  color_texcoord_vertex_format.AddAttribute (media::geometry::VertexAttributeSemantic_Color, media::geometry::VertexAttributeType_Float4, 0);
  color_texcoord_vertex_format.AddAttribute (media::geometry::VertexAttributeSemantic_TexCoord0, media::geometry::VertexAttributeType_Float2, sizeof (float) * 4);

  media::geometry::VertexDeclaration position_vertex_declaration       (position_vertex_format),
                                     color_texcoord_vertex_declaration (color_texcoord_vertex_format);
  media::geometry::VertexStream      position_vertex_stream            (position_vertex_declaration),
                                     color_texcoord_vertex_stream      (color_texcoord_vertex_declaration);

  vertex_buffer.Attach (position_vertex_stream);
  vertex_buffer.Attach (color_texcoord_vertex_stream);

  mesh.Attach (vertex_buffer);

  return mesh;
}

float* SkeletonImpl::AddMeshToDrawOrder (unsigned int next_triangle_list_mesh_to_use, SlotImpl* slot, AttachmentImpl* attachment,
                                         unsigned int triangles_count, unsigned int vertices_count, const uint16_t* source_indices,
                                         const float* attachment_texcoords)
{
  if (next_triangle_list_mesh_to_use >= triangle_list_meshes.size ())
  {
    media::geometry::Mesh mesh = CreateMesh ();

    mesh.Rename (common::format ("spine_skeleton_%p_triangle_list_%u", this, triangle_list_meshes.size ()).c_str ());

    triangle_list_meshes.push_back (MeshDescPtr (new MeshDesc (mesh), false));
  }

  MeshDesc& mesh_desc = *triangle_list_meshes [next_triangle_list_mesh_to_use++];;

  geometry::Mesh& current_mesh = mesh_desc.mesh;

  const char*  texture_path    = attachment->TexturePath ();
  BlendMode    blend_mode      = slot->BlendMode ();
  TexcoordWrap texcoord_wrap_u = attachment->TexcoordWrapU (),
               texcoord_wrap_v = attachment->TexcoordWrapV ();

  stl::string mesh_material = common::format ("spine_%s_%d_%d_%d", texture_path, blend_mode, texcoord_wrap_u, texcoord_wrap_v);

  if (mesh_desc.material != mesh_material || mesh_desc.triangles_count != triangles_count)
  {
    //add material if it was not added yet
    MaterialsMap::iterator material_iter = materials.find (mesh_material.c_str ());

    if (material_iter == materials.end ())
      materials.insert_pair (mesh_material.c_str (), Wrappers::Wrap<media::spine::Material, media::spine::MaterialImpl> (CreateMaterialImpl (mesh_material.c_str (), texture_path, blend_mode, texcoord_wrap_u, texcoord_wrap_v)));

    current_mesh.RemoveAllPrimitives ();

    current_mesh.AddPrimitive (media::geometry::PrimitiveType_TriangleList, 0, 0, triangles_count, mesh_material.c_str ());

    mesh_desc.material        = mesh_material;
    mesh_desc.triangles_count = triangles_count;
  }

  //replace data in index buffer, if required
  media::geometry::IndexBuffer& index_buffer = current_mesh.IndexBuffer ();

  unsigned int indices_count      = triangles_count * 3,  //3 indices per triangle
               prev_indices_count = index_buffer.Size ();

  if (prev_indices_count < indices_count)
    index_buffer.Resize (indices_count);

  size_t indices_hash = common::crc32 (source_indices, indices_count * sizeof (uint16_t));

  if (indices_hash != mesh_desc.index_buffer_hash)
  {
    uint16_t* indices = index_buffer.Data<uint16_t> ();

    memcpy (indices, source_indices, indices_count * sizeof (uint16_t));

    mesh_desc.index_buffer_hash = indices_hash;

    index_buffer.InvalidateData ();
  }

  //replace data in vertex streams, if required
  media::geometry::VertexBuffer& vertex_buffer                = current_mesh.VertexBuffer (0);
  media::geometry::VertexStream& position_vertex_stream       = vertex_buffer.Stream (0),
                                 color_texcoord_vertex_stream = vertex_buffer.Stream (1);

  if (position_vertex_stream.Size () < vertices_count)
  {
    position_vertex_stream.Resize (vertices_count);
    color_texcoord_vertex_stream.Resize (vertices_count);
  }

  //fill texcoords and color
  math::vec4f  color          = slot->Color () * attachment->Color ();
  size_t       texcoords_hash = common::crc32 (attachment_texcoords, vertices_count * 2 * sizeof (float));

  if (color != mesh_desc.color || texcoords_hash != mesh_desc.texcoords_hash)
  {
    float* current_vertex_color_texcoord = (float*)color_texcoord_vertex_stream.Data ();

    for (unsigned int i = 0; i < vertices_count; i++, current_vertex_color_texcoord += 6, attachment_texcoords += 2)
    {
      current_vertex_color_texcoord [0] = color.x;
      current_vertex_color_texcoord [1] = color.y;
      current_vertex_color_texcoord [2] = color.z;
      current_vertex_color_texcoord [3] = color.w;
      current_vertex_color_texcoord [4] = attachment_texcoords [0];
      current_vertex_color_texcoord [5] = 1.f - attachment_texcoords [1];
    }

    mesh_desc.color          = color;
    mesh_desc.texcoords_hash = texcoords_hash;

    color_texcoord_vertex_stream.InvalidateData ();
  }

  position_vertex_stream.InvalidateData ();

  draw_order.push_back (current_mesh);

  return (float*)position_vertex_stream.Data ();
}
