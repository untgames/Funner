#include <stdint.h>

#include <xtl/common_exceptions.h>

#include <common/log.h>
#include <common/strlib.h>

#include <media/geometry/mesh.h>

#include "attachment_impl.h"
#include "bone_impl.h"
#include "skeleton_impl.h"
#include "slot_impl.h"
#include "wrappers.h"

using namespace media::spine;

namespace
{

//constants
const unsigned int SPRITES_MESHES_RESERVE_COUNT       = 4;         //reserve count for sprites meshes array
const unsigned int TRIANGLE_LIST_MESHES_RESERVE_COUNT = 4;         //reserve count for triangle list meshes array
const unsigned int DRAW_ORDER_MESHES_RESERVE_COUNT    = 4;         //reserve count for draw order meshes array
const unsigned int MAX_BATCH_SIZE                     = 65536 / 4; //max batch size to fit in index value range

}

/*
   Constructor
*/

SkeletonImpl::SkeletonImpl ()
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

  //build array of slots and attachments used for drawing
  stl::vector<xtl::com_ptr<SlotImpl> >       slots_draw_order;
  stl::vector<xtl::com_ptr<AttachmentImpl> > attachments_draw_order;

  slots_draw_order.reserve (SlotsCount ());
  attachments_draw_order.reserve (SlotsCount ());

  for (unsigned int i = 0, count = SlotsCount (); i < count; i++)
  {
    //we create new SlotImpl and AttachmentImpl here instead of referencing one from slots array, because it's faster to create new rather than searching for index
    xtl::com_ptr<SlotImpl> slot (CreateSlotForDrawOrder (i), false);

    if (slot->HasAttachment ())
    {
      xtl::com_ptr<AttachmentImpl> attachment (slot->CreateAttachmentImpl (), false);

      switch (attachment->Type ())
      {
        case AttachmentType_Region:
        case AttachmentType_Mesh:
          //attachment should be rendered
          slots_draw_order.push_back (slot);
          attachments_draw_order.push_back (attachment);
          break;
        default:
          break;
      }
    }
  }

  //build meshes
  unsigned int           next_sprite_mesh_to_use = 0,
                         next_triangle_list_mesh_to_use = 0;
  bool                   current_mesh_is_sprite;
  media::geometry::Mesh* current_mesh = 0;
  float*                 current_vertex_position = 0;
  float*                 current_vertex_color_texcoord = 0;

  for (unsigned int i = 0, count = slots_draw_order.size (); i < count; i++)
  {
    SlotImpl*       slot       = slots_draw_order [i].get ();
    AttachmentImpl* attachment = attachments_draw_order [i].get ();

    switch (attachment->Type ())
    {
      case AttachmentType_Region:
      {
        //check if we needs to start new mesh
        if (current_mesh && !current_mesh_is_sprite)
        {
          draw_order.push_back (*current_mesh);
          current_mesh = 0;
        }

        if (!current_mesh)
        {
          current_mesh_is_sprite = true;

          if (next_sprite_mesh_to_use >= sprites_meshes.size ())
          {
            media::geometry::Mesh mesh = CreateMesh ();

            mesh.Rename (common::format ("spine_skeleton_%p_sprites_%u", this, sprites_meshes.size ()).c_str ());

            sprites_meshes.push_back (mesh);
          }

          current_mesh = &sprites_meshes [next_sprite_mesh_to_use++];

          current_mesh->RemoveAllPrimitives ();

          //calculate how many sprites mesh will contain and what will be the max batch size
          unsigned int sprites_count       = 0,
                       current_batch_size  = 0,
                       max_batch_size      = 0,
                       current_base_sprite = 0;

          BlendMode    current_batch_blend_mode      = (BlendMode)-1;
          TexcoordWrap current_batch_texcoord_wrap_u = (TexcoordWrap)-1,
                       current_batch_texcoord_wrap_v = (TexcoordWrap)-1;
          const char*  current_batch_texture_path    = "";
          stl::string  current_material;

          for (unsigned int j = i; j < count; j++)
          {
            AttachmentImpl* current_attachment = attachments_draw_order [j].get ();

            bool         batch_ended = false;
            BlendMode    slot_blend_mode;
            TexcoordWrap attachment_texcoord_wrap_u, attachment_texcoord_wrap_v;
            const char*  attachment_texture_path = 0;

            if (current_attachment->Type () != AttachmentType_Region)
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
                current_mesh->AddPrimitive (media::geometry::PrimitiveType_TriangleList, 0, 0, current_batch_size * 2, current_base_sprite * 2, current_material.c_str ());

                current_base_sprite += current_batch_size;
              }

              current_batch_size = 0;

              if (attachment_texture_path)
              {
                current_batch_blend_mode      = slot_blend_mode;
                current_batch_texcoord_wrap_u = attachment_texcoord_wrap_u;
                current_batch_texcoord_wrap_v = attachment_texcoord_wrap_v;
                current_batch_texture_path    = attachment_texture_path;

                current_material = common::format ("spine_%s_%d_%d_%d", current_batch_texture_path, current_batch_blend_mode, current_batch_texcoord_wrap_u, current_batch_texcoord_wrap_v);

                //add material if it was not added yet
                MaterialsMap::iterator material_iter = materials.find (current_material.c_str ());

                if (material_iter == materials.end ())
                  materials.insert_pair (current_material.c_str (), Wrappers::Wrap<media::spine::Material, media::spine::MaterialImpl> (CreateMaterialImpl (current_material.c_str (), current_batch_texture_path, current_batch_blend_mode, current_batch_texcoord_wrap_u, current_batch_texcoord_wrap_v)));
              }
            }

            if (current_attachment->Type () != AttachmentType_Region)
              break;

            current_batch_size++;
          }

          if (current_batch_size)
          {
            if (current_batch_size > max_batch_size)
              max_batch_size = current_batch_size;

            //we have non-empty batch after loop, add primitive to mesh
            current_mesh->AddPrimitive (media::geometry::PrimitiveType_TriangleList, 0, 0, current_batch_size * 2, current_base_sprite * 2, current_material.c_str ());
          }

          //add new data to index buffer, if required
          media::geometry::IndexBuffer& index_buffer = current_mesh->IndexBuffer ();

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

          media::geometry::VertexBuffer& vertex_buffer                = current_mesh->VertexBuffer (0);
          media::geometry::VertexStream& position_vertex_stream       = vertex_buffer.Stream (0),
                                         color_texcoord_vertex_stream = vertex_buffer.Stream (1);

          if (position_vertex_stream.Size () < vertices_count)
          {
            position_vertex_stream.Resize (vertices_count);
            color_texcoord_vertex_stream.Resize (vertices_count);
          }

          current_vertex_position       = (float*)position_vertex_stream.Data ();
          current_vertex_color_texcoord = (float*)color_texcoord_vertex_stream.Data ();
        }

        //fill vertices position
        attachment->ComputeWorldVertices (slot, current_vertex_position, 0, 2);

        //offset pointer by 4 vertices
        current_vertex_position += 8;   //4 vertices is 8 floats (x, y)

        //fill texcoords and color
        math::vec4f  color                = slot->Color () * attachment->Color ();
        const float* attachment_texcoords = attachment->Texcoords ();

        for (unsigned int i = 0; i < 4; i++, current_vertex_color_texcoord += 6, attachment_texcoords += 2)
        {
          current_vertex_color_texcoord [0] = color.x;
          current_vertex_color_texcoord [1] = color.y;
          current_vertex_color_texcoord [2] = color.z;
          current_vertex_color_texcoord [3] = color.w;
          current_vertex_color_texcoord [4] = attachment_texcoords [0];
          current_vertex_color_texcoord [5] = attachment_texcoords [1];
        }

        //TODO invalidate position vertex stream
        //TODO invalidate color-uv vertex stream in case of data change

        break;
      }
      case AttachmentType_Mesh:
      {
        //check if we needs to start new mesh
        if (current_mesh)
        {
          draw_order.push_back (*current_mesh);
          current_mesh_is_sprite = false;
          current_mesh = 0;
        }

        if (next_triangle_list_mesh_to_use >= sprites_meshes.size ())
        {
          media::geometry::Mesh mesh = CreateMesh ();

          mesh.Rename (common::format ("spine_skeleton_%p_triangle_list_%u", this, triangle_list_meshes.size ()).c_str ());

          triangle_list_meshes.push_back (MeshDesc (mesh));
        }

        MeshDesc* mesh_desc = &triangle_list_meshes [next_triangle_list_mesh_to_use++];;

        current_mesh = &mesh_desc->mesh;

        current_mesh->RemoveAllPrimitives ();

        const char*  texture_path    = attachment->TexturePath ();
        BlendMode    blend_mode      = slot->BlendMode ();
        TexcoordWrap texcoord_wrap_u = attachment->TexcoordWrapU (),
                     texcoord_wrap_v = attachment->TexcoordWrapV ();

        stl::string mesh_material = common::format ("spine_%s_%d_%d_%d", texture_path, blend_mode, texcoord_wrap_u, texcoord_wrap_v);

        //add material if it was not added yet
        MaterialsMap::iterator material_iter = materials.find (mesh_material.c_str ());

        if (material_iter == materials.end ())
          materials.insert_pair (mesh_material.c_str (), Wrappers::Wrap<media::spine::Material, media::spine::MaterialImpl> (CreateMaterialImpl (mesh_material.c_str (), texture_path, blend_mode, texcoord_wrap_u, texcoord_wrap_v)));

        unsigned int triangles_count = attachment->TrianglesCount ();

        current_mesh->AddPrimitive (media::geometry::PrimitiveType_TriangleList, 0, 0, triangles_count, mesh_material.c_str ());

        //replace data in index buffer, if required
        media::geometry::IndexBuffer& index_buffer = current_mesh->IndexBuffer ();

        unsigned int indices_count      = triangles_count * 3,  //3 indices per triangle
                     prev_indices_count = index_buffer.Size (),
                     vertices_count     = attachment->VerticesCount ();

        if (prev_indices_count < indices_count)
          index_buffer.Resize (indices_count);

        size_t indices_hash = common::crc32 (attachment->Indices (), indices_count * sizeof (uint16_t));

        if (indices_hash != mesh_desc->index_buffer_hash)
        {
          uint16_t* indices = index_buffer.Data<uint16_t> ();

          memcpy (indices, attachment->Indices (), indices_count * sizeof (uint16_t));

          mesh_desc->index_buffer_hash = indices_hash;

          //TODO invalidate index buffer
        }

        //replace data in vertex streams, if required
        media::geometry::VertexBuffer& vertex_buffer                = current_mesh->VertexBuffer (0);
        media::geometry::VertexStream& position_vertex_stream       = vertex_buffer.Stream (0),
                                       color_texcoord_vertex_stream = vertex_buffer.Stream (1);

        if (position_vertex_stream.Size () < vertices_count)
        {
          position_vertex_stream.Resize (vertices_count);
          color_texcoord_vertex_stream.Resize (vertices_count);
        }

        float* position_vertices             = (float*)position_vertex_stream.Data ();
        float* current_vertex_color_texcoord = (float*)color_texcoord_vertex_stream.Data ();

        //fill vertices position
        attachment->ComputeWorldVertices (slot, position_vertices, 0, 2);

        //TODO invalidate position vertex stream

        //fill texcoords and color
        math::vec4f  color                = slot->Color () * attachment->Color ();
        const float* attachment_texcoords = attachment->Texcoords ();
        size_t       texcoords_hash       = common::crc32 (attachment_texcoords, vertices_count * 2 * sizeof (float));

        if (color != mesh_desc->color || texcoords_hash != mesh_desc->texcoords_hash)
        {
          for (unsigned int i = 0; i < vertices_count; i++, current_vertex_color_texcoord += 6, attachment_texcoords += 2)
          {
            current_vertex_color_texcoord [0] = color.x;
            current_vertex_color_texcoord [1] = color.y;
            current_vertex_color_texcoord [2] = color.z;
            current_vertex_color_texcoord [3] = color.w;
            current_vertex_color_texcoord [4] = attachment_texcoords [0];
            current_vertex_color_texcoord [5] = attachment_texcoords [1];
          }

          mesh_desc->color          = color;
          mesh_desc->texcoords_hash = texcoords_hash;

          //TODO invalidate color-uv vertex stream
        }

        break;
      }
      default:
        //ignore all other types
        break;
    }
  }

  //push last mesh to draw order list
  if (current_mesh)
    draw_order.push_back (*current_mesh);

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

  return draw_order.size ();
}

media::geometry::Mesh SkeletonImpl::Mesh (unsigned int mesh_index)
{
  if (mesh_index >= MeshesCount ())
    throw xtl::make_range_exception ("media::spine::SkeletonImpl::Mesh", "mesh_index", mesh_index, 0, MeshesCount ());

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

