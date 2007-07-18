#include "export.h"

//экспортируемая вершина
struct Vertex
{
  int     pos;
  Point3* n; //??? ptr or val
  Point3* color;
  Point3* tv [MAX_MESHMAPS];
  Point3* face_map;
};

//экспортируемый треугольник
struct Triangle
{
  int     v [3];
  int     mtl_id;
  Point3  n;
};

struct VertexMap
{
  Vertex*    vertex;
  bool       init;
  VertexMap* next;
};

//экспортируемый мэш
struct ExpMesh
{
  Vertex*    verts;
  Triangle*  tris; 
  int       tris_count,verts_count,points_count;
  bool      export_positions:1;
  bool      export_normals:1;
  bool      export_colors:1;
  bool      export_face_map:1;
  bool      export_tv [MAX_MESHMAPS];
};

struct VertexWeights
{
  int first,count;
};

inline bool check (const Vertex& v1,const Vertex& v2)
{
  for (int i=0;i<MAX_MESHMAPS-1;i++)
    if (v1.tv [i] != v2.tv [i])
      return false;

  //подразумевается, что позиции равны и так

//  return v1.n == v2.n && v1.color == v2.color  && v1.face_map == v2.face_map;
  return *v1.n == *v2.n && v1.color == v2.color && v1.face_map == v2.face_map;
}

static Point3& GetVertexNormal (Mesh* mesh,int faceNo,RVertex* rv)
{
  Face& f       = mesh->faces[faceNo];
  DWORD smGroup = f.smGroup;
  int   numNormals;

  if (rv->rFlags & SPECIFIED_NORMAL)  return rv->rn.getNormal();
  else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
  {
    if (numNormals == 1) return rv->rn.getNormal();
    else 
    {
      Point3* vertexNormal = NULL;

      for (int i=0;i<numNormals;i++) 
          if (rv->ern [i].getSmGroup() & smGroup) 
            vertexNormal = &rv->ern[i].getNormal(); 

      return *vertexNormal;
    }
  }   
  else return mesh->getFaceNormal(faceNo);
}

static void MakeFaceUV (Face& f,Point3** tv)
{
  static int nextpt[3] = {1,2,0};
  static int prevpt[3] = {2,0,1};
  static Point3 basic_tva[3] = { 
          Point3(0.0,0.0,0.0),Point3(1.0,0.0,0.0),Point3(1.0,1.0,0.0)
  };
  static Point3 basic_tvb[3] = { 
          Point3(1.0,1.0,0.0),Point3(0.0,1.0,0.0),Point3(0.0,0.0,0.0)
  };

  // make the invisible edge be 2->0 

  int nhid = 2;

  if (!(f.flags&EDGE_A))      nhid=0;
  else if (!(f.flags&EDGE_B)) nhid=1;
  else if (!(f.flags&EDGE_C)) nhid=2;

  int na = 2 - nhid;

  Point3* basetv = (f.v [prevpt [nhid]] < f.v [nhid]) ? basic_tva : basic_tvb; 

  for (int i=0; i<3; i++) 
  {  
    tv [i] = basetv + na;
    na     = nextpt [na];
  }
}

static int FaceCmp (const Triangle& t1,const Triangle& t2)
{
  return t1.mtl_id == t2.mtl_id ? 0 : t1.mtl_id < t2.mtl_id ? -1 : 1;
}

static void SortFacesByMtl (ExpMesh& mesh)
{
  typedef int (*cmp_fn)(const void*,const void*);

  qsort (mesh.tris,mesh.tris_count,sizeof (Triangle),(cmp_fn)FaceCmp);
}

static ExpMesh* CreateExpMesh (Mesh& mesh,Mtl* mtl,int* order)
{
  bool face_map_enable = mtl && (mtl->Requirements(-1) & MTLREQ_FACEMAP);

  int tris_count = mesh.getNumFaces(),
      verts_count = tris_count*3;

  Vertex*    verts = new Vertex [verts_count];
  VertexMap* vmap  = new VertexMap [verts_count];
  Triangle*  tris  = new Triangle [tris_count];    

  if (!verts || !tris || !vmap)
  {
    delete [] verts;
    delete [] tris;
    delete [] vmap;

    return NULL;
  }    
                 
  for (int i=0;i<verts_count;i++)
  {
    vmap [i].init   = false;
    vmap [i].next   = NULL;
  }    

  Vertex*    first_free_vertex = verts;
  VertexMap* first_free_vmap   = vmap+mesh.getNumVerts();

  for (int i=0;i<tris_count;i++)
  {
    Face&   face   = mesh.faces [i];
    TVFace* vcFace = mesh.vcFace ? mesh.vcFace + i : NULL;

    tris [i].n      = mesh.getFaceNormal (i);
    tris [i].mtl_id = mesh.faces[i].getMatID();

    Point3* face_map [3];

    if (face_map_enable) 
      MakeFaceUV (face,face_map);
    else face_map [0] = face_map [1] = face_map [2] = NULL;

    for (int j=0;j<3;j++)
    {
      Vertex newV;

      newV.pos   = face.v [order [j]];
      newV.n     = &GetVertexNormal (&mesh,i,mesh.getRVertPtr (face.getVert (order [j]))); 
      newV.color = vcFace ? mesh.vertCol + vcFace->t [order [j]] : NULL;
      newV.face_map = face_map [j];

      for (int k=0;k<MAX_MESHMAPS-1;k++) //!!!!!!
        newV.tv [k] = mesh.mapSupport (k) ? mesh.mapVerts (k) + mesh.mapFaces (k)[i].t [order [j]] : NULL; 

      VertexMap* first = vmap + newV.pos, *v=first;

      if (v->init)
      {
        for (;v && !check (*v->vertex,newV);v=v->next);

        if (!v) //no find, need to add vertex
        {
          v           = first_free_vmap++;
          v->next     = first->next;
          first->next = v;
        }
      }

      if (!v->init)
      {
        v->vertex  = first_free_vertex++;
        *v->vertex = newV;
        v->init    = true;
      }

      tris [i].v [j] = v->vertex - verts;
    }
  }

  delete [] vmap;

  ExpMesh* exp_mesh = new ExpMesh;

  if (!exp_mesh)
  {
    delete [] tris;
    delete [] verts;

    return NULL;
  }

  exp_mesh->verts_count = first_free_vertex - verts;
  exp_mesh->tris_count = tris_count;
  exp_mesh->points_count = mesh.getNumVerts();
  exp_mesh->verts  = verts;
  exp_mesh->tris   = tris;

  exp_mesh->export_positions = exp_mesh->export_normals = true;
  exp_mesh->export_colors    = mesh.vcFace != NULL;
  exp_mesh->export_face_map  = face_map_enable;

  for (int k=0;k<MAX_MESHMAPS-1;k++) //!!!!!!
    exp_mesh->export_tv [k] = mesh.mapSupport (k);

  SortFacesByMtl (*exp_mesh);

  return exp_mesh;
}

static void DeleteExpMesh (ExpMesh* mesh)
{
  if (mesh)
  {
    delete [] mesh->tris;
    delete [] mesh->verts;
    delete mesh;
  }
}

static void ExportVertexes (ExpMesh& mesh,Point3* vpos)
{
  if (mesh.export_positions)
  {
    dump<<"vertex_num\t"<<mesh.verts_count<<endl;

    logBeginFrame ("vertexes");

    Vertex* v = mesh.verts;

    for (int i=0;i<mesh.verts_count;i++,v++) 
      dump<<"vertex\t"<<vpos [v->pos]<<endl;

    logEndFrame ();
  }
}

static void ExportTexCoordChannel (ExpMesh& mesh,int channel)
{
  if (mesh.export_tv [channel])
  {
    logBeginFrame ("map_channel");

    dump<<"map_channel\t"<<channel<<endl;

    dump<<"tex_coords_num\t"<<mesh.verts_count<<endl;

    logBeginFrame ("tex_coords");

    Vertex* v = mesh.verts;

    for (int i=0;i<mesh.verts_count;i++,v++) 
      dump<<"tex_coord "<<v->tv [channel]->x<<"\t"<<(1-v->tv [channel]->y)<<"\t"<<v->tv [channel]->z<<endl;

    logEndFrame (); 

    logEndFrame ();
  }
}

static void ExportFaceMap (ExpMesh& mesh)
{
  if (mesh.export_face_map)
  {
    logBeginFrame ("map_channel");

    dump<<"face_map"<<endl;
    dump<<"tex_coords_num "<<mesh.verts_count<<endl;

    logBeginFrame ("tex_coords");

    Vertex* v = mesh.verts;
       
    for (int i=0;i<mesh.verts_count;i++,v++) 
      dump<<"tex_coord "<<*v->face_map<<endl;

    logEndFrame ();

    logEndFrame ();
  }
}

static void ExportTexCoords (ExpMesh& mesh)
{
  if (mesh.export_face_map) ExportFaceMap (mesh);
  else
  {
    for (int mp=0;mp<MAX_MESHMAPS-1;mp++)  ///mp=0????
      if (mesh.export_tv [mp])
        ExportTexCoordChannel (mesh,mp);
  }
}

static void ExportVertexColors (ExpMesh& mesh)
{
  if (mesh.export_colors)
  {
    dump<<"color_vertexes_num\t"<<mesh.verts_count<<endl;

    logBeginFrame ("color_vertexes");

    Vertex* v = mesh.verts;

    for (int i=0;i<mesh.verts_count;i++,v++) 
      dump<<"color_vertex "<<i<<"\t"<<*v->color<<endl;

    logEndFrame ();
  }
}

static void ExportNormals (ExpMesh& mesh)
{
  if (mesh.export_normals)
  {
    dump<<"vrtx_normals_num\t"<<mesh.verts_count<<endl;

    logBeginFrame ("normals");

    Vertex* v = mesh.verts;

    for (int i=0;i<mesh.verts_count;i++,v++)
    {
      Point3 n = *v->n/v->n->Length ();
          
      dump<<"normal\t"<<n<<endl;;
    }  

    logEndFrame ();
  }
}

void ExportSkin (INode* node,ExpMesh& mesh)
{  
  ISkin* skin = FindSkin (node);

  if (!skin)
    return;

  ISkinContextData* context = skin->GetContextInterface (node);

  if (!context || context->GetNumPoints() != mesh.points_count)
    return;

  VertexWeights* vert_weights = new VertexWeights [context->GetNumPoints ()];

  if (!vert_weights)
    return;

  int i,first;

  for (i=0,first=0;i<context->GetNumPoints();i++)
  {
    int cnt = context->GetNumAssignedBones(i);

    vert_weights [i].first = first;
    vert_weights [i].count = cnt;

    first += cnt;
  }

  logBeginFrame ("skin");

  dump<<"bones_num\t"<<skin->GetNumBones()<<endl;
  dump<<"vertex_weights_num\t"<<mesh.verts_count<<endl;
  dump<<"weights_num\t"<<first<<endl;

  logBeginFrame ("bones");

  for (int i=0;i<skin->GetNumBones();i++)
  {
    INode* node = skin->GetBone (i);

    logBeginFrame ("bone");

    dump<<"index\t"<<i<<endl;
    dump<<"name\t'"<<node->GetName ()<<"'"<<endl;

    DumpMatrix ("tm",node->GetObjectTM(0)); 

    logEndFrame ();
  }

  logEndFrame ();

    //доделать нормирование весов и сокращение количества костей

  logBeginFrame ("vertex_weights");

  for (int i=0;i<mesh.verts_count;i++)
  {
    VertexWeights& weights = vert_weights [mesh.verts [i].pos];

    dump<<"vertex_weight\t"<<weights.first<<"\t"<<weights.count<<endl;
  }
  
  logEndFrame ();

  logBeginFrame ("weights");

  for (int i=0;i<context->GetNumPoints();i++)
    for (int j=0;j<context->GetNumAssignedBones(i);j++)
      dump<<"weight\t"<<context->GetAssignedBone (i,j)<<"\t"<<context->GetBoneWeight(i,j)<<endl;

  logEndFrame   ();

  logEndFrame   ();

  delete [] vert_weights;
}

static void ExportFaces (ExpMesh& mesh)
{
  dump<<"faces_num\t"<<mesh.tris_count<<endl;    

  logBeginFrame ("faces");

  for (int i=0;i<mesh.tris_count;i++)     
  {
    dump<<"face"
        <<"\t"<<mesh.tris [i].v [0]
        <<"\t"<<mesh.tris [i].v [1]
        <<"\t"<<mesh.tris [i].v [2]
        <<"\t"<<mesh.tris [i].n        
        <<endl;
  }

  logEndFrame ();
}

static void DumpPrimitive (int first,int count,int mtl_id)
{
  dump<<"primitive\t"<<first<<"\t"<<count;

//  if (mtl)
//    dump<<"\t'"<<mtl->GetName ()<<"'";
  dump<<"\t"<<mtl_id;

  dump<<endl;
}

void ExportPrimitives (ExpMesh& mesh,Mtl* mtl)
{
  if (!mesh.tris)
    return ;

  if (!mtl || mtl->NumSubMtls()<=0)
  {
    dump<<"primitives_num\t"<<1<<endl;

    logBeginFrame ("primitives");

//    DumpPrimitive (0,mesh.tris_count,mtl);
    DumpPrimitive (0,mesh.tris_count,0);

    logEndFrame ();
  }
  else
  {
    int mtl_count = 1;
    int mtl_id    = mesh.tris->mtl_id;

    for (int i=1;i<mesh.tris_count;i++)
      if (mtl_id != mesh.tris [i].mtl_id)
      {
        mtl_id = mesh.tris [i].mtl_id;
        mtl_count++;
      }

    dump<<"primitives_num\t"<<mtl_count<<endl;

    logBeginFrame ("primitives");

    mtl_id = mesh.tris->mtl_id;

    int i,first;

    for (i=1,first=0;i<mesh.tris_count;i++)
      if (mtl_id != mesh.tris [i].mtl_id)
      {
//        DumpPrimitive (first,i-first,mtl_id<mtl->NumSubMtls ()?mtl->GetSubMtl (mtl_id):mtl);
        DumpPrimitive (first,i-first,mtl_id<mtl->NumSubMtls ()?mtl_id:0);

        mtl_id = mesh.tris [i].mtl_id;
        first  = i;
      }      

//    DumpPrimitive (first,i-first,mtl_id<mtl->NumSubMtls ()?mtl->GetSubMtl (mtl_id):mtl);    
    DumpPrimitive (first,i-first,mtl_id<mtl->NumSubMtls ()?mtl_id:0);    

    logEndFrame ();
  }
}

static bool IsNeg (const Matrix3 &m)
{
  return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

static TriObject* GetTriObject (INode* node,TimeValue time)
{
  Object* obj = node->EvalWorldState (time).obj;

  if (obj->CanConvertToType (Class_ID (TRIOBJ_CLASS_ID,0))) 
  { 
    TriObject* tri = (TriObject*)obj->ConvertToType (time,Class_ID(TRIOBJ_CLASS_ID, 0));

    return tri;
  }
  else return NULL;
}

void ExportMesh (INode* node,TimeValue time)
{
  Matrix3     tm  = node->GetObjTMAfterWSM (time); 
  ObjectState os  = node->EvalWorldState (time);
  TriObject*  tri = GetTriObject (node,time);

  int pos_order [] = {0,1,2}, 
      neg_order [] = {2,1,0};

  if (tri)
  {
    Mesh& mesh = tri->GetMesh ();  

    mesh.buildNormals();
    
    ExpMesh* exp_mesh = CreateExpMesh (mesh,node->GetMtl(),IsNeg (tm) ? neg_order : pos_order);

    if (exp_mesh)
    {
      logBeginFrame ("mesh");

      dump<<"time\t"<<time<<endl;
      dump<<"cast_shadows\t"<<node->CastShadows()<<endl;
      dump<<"recv_shadows\t"<<node->RcvShadows()<<endl;
      dump<<"vrtx_normals_num_test\t"<<mesh.normalCount<<endl;
      dump<<"vrtx_num_test\t"<<mesh.getNumVerts()<<endl;
      dump<<"colors_num_test\t"<<mesh.numCVerts<<endl;
      dump<<"faces_num_test\t"<<mesh.getNumFaces()<<endl;

      for (int i=0;i<MAX_MESHMAPS-1;i++)
        if (mesh.getNumMapVerts(i))
          dump<<"tv_count_test\t"<<i<<"\t"<<mesh.getNumMapVerts(i)<<endl;

      ExportPrimitives   (*exp_mesh,node->GetMtl());
      ExportVertexes     (*exp_mesh,mesh.verts);
      ExportNormals      (*exp_mesh);
      ExportVertexColors (*exp_mesh);
      ExportTexCoords    (*exp_mesh);
      ExportSkin         (node,*exp_mesh);
      ExportFaces        (*exp_mesh);

      logEndFrame ();  

      DeleteExpMesh (exp_mesh);
    }

    if (tri != node->EvalWorldState (time).obj)
      delete tri;
  }
}
