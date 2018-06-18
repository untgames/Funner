#ifndef MEDIALIB_GEOMETRY_MESH_OBSERVER_HEADER
#define MEDIALIB_GEOMETRY_MESH_OBSERVER_HEADER

#include <media/geometry/mesh.h>

namespace media
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Mesh updates events listener
///////////////////////////////////////////////////////////////////////////////////////////////////
class IMeshUpdatesListener
{
  public:
    ///Events handlers
    virtual void OnMeshStructureChanged               (const Mesh&,               bool& continue_processing) {} //index buffer was replaced or vertex buffer was added or removed
    virtual void OnMeshPrimitivesChanged              (const Mesh&,               bool& continue_processing) {} //primitives were added or removed
    virtual void OnIndexBufferStructureChanged        (const IndexBuffer&,        bool& continue_processing) {} //index buffer type or size changed
    virtual void OnIndexBufferDataChanged             (const IndexBuffer&,        bool& continue_processing) {} //index buffer data changed
    virtual void OnVertexBufferStructureChanged       (const VertexBuffer&,       bool& continue_processing) {} //vertex buffer streams or weights were changed
    virtual void OnVertexStreamStructureChanged       (const VertexStream&,       bool& continue_processing) {} //vertex stream size was changed
    virtual void OnVertexStreamDataChanged            (const VertexStream&,       bool& continue_processing) {} //vertex stream data was changed
    virtual void OnVertexWeightStreamStructureChanged (const VertexWeightStream&, bool& continue_processing) {} //vertex weight stream size was changed
    virtual void OnVertexWeightStreamDataChanged      (const VertexWeightStream&, bool& continue_processing) {} //vertex weight stream data was changed
    virtual void OnMaterialMapDataChanged             (const MaterialMap&,        bool& continue_processing) {} //materials were added or removed or material map was replaced

  protected:
    virtual ~IMeshUpdatesListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Observer notifying about structure and data changes of mesh and it's components
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshObserver
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Constructor / destructor
///////////////////////////////////////////////////////////////////////////////////////////////////
    MeshObserver  ();
    MeshObserver  (const MeshObserver&);
    ~MeshObserver ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Assignment
///////////////////////////////////////////////////////////////////////////////////////////////////
    MeshObserver& operator = (const MeshObserver&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Check meshes for changes and notifies listener. Calls structure changes first, after all
///structure changes calls data changes. If structure change was reported for some item, data change
///will not be reported for this item.
///////////////////////////////////////////////////////////////////////////////////////////////////
    void NotifyUpdates (const Mesh& mesh, IMeshUpdatesListener& listener) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (MeshObserver&);

  private:
    struct Impl;
    xtl::intrusive_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Swap
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (MeshObserver&, MeshObserver&);

}

}

#endif
