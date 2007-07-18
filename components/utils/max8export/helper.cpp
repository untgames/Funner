#include "export.h"

#include <gizmo.h>
#include <gizmoimp.h>
#include <dummy.h>

static void ExportCylinder (INode* node,HelperObject& obj)
{
  CylGizmoObject& cyl = (CylGizmoObject&)obj;
  Box3            box;
  Matrix3         x   (1);

  cyl.GetBoundBox (x,static_frame,box);

  dump<<"radius\t"<<(box.pmax.x-box.pmin.x) / 2.0f<<endl;
  dump<<"height\t"<<(box.pmax.z-box.pmin.z)<<endl;
  dump<<"center\t"<<(box.pmax+box.pmin)/2.0f<<endl;
}

static void ExportSphere (INode* node,HelperObject& obj)
{
  SphereGizmoObject& s = (SphereGizmoObject&)obj;
  Box3               box;
  Matrix3            x   (1);

  s.GetBoundBox (x,static_frame,box);

  dump<<"radius\t"<<(box.pmax.z-box.pmin.z)/2.0f<<endl;
  dump<<"center\t"<<(box.pmax+box.pmin)/2.0f<<endl;
}

static void ExportBox (INode* node,HelperObject& obj)
{
  Box3            box;
  BoxGizmoObject& cyl = (BoxGizmoObject&)obj;
  Matrix3         x (1);

  cyl.GetBoundBox (x,static_frame,box);  

  dump<<"center\t"<<(box.pmax+box.pmin)/2.0f<<endl;
  dump<<"size\t"<<(box.pmax-box.pmin)<<endl;
}

void ExportDummy (INode* node,HelperObject& obj)
{
  Box3         box;
  Box3         local_box;
  DummyObject& dummy = (DummyObject&)obj;
  Matrix3      x (1);

  dummy.GetDeformBBox (static_frame,box,&x);

  dump<<"center\t"<<(box.pmax+box.pmin)/2.0f<<endl;
  dump<<"size\t"<<(box.pmax-box.pmin)<<endl;
}

const char* GetHelperType (Object& obj)
{
  if      (obj.ClassID () == CYLGIZMO_CLASSID)    return "gizmo_cylinder";
  else if (obj.ClassID () == SPHEREGIZMO_CLASSID) return "gizmo_sphere";
  else if (obj.ClassID () == BOXGIZMO_CLASSID)    return "gizmo_box";
  else if (obj.ClassID () == dummyClassID)        return "dummy";
  else return NULL;
}

void ExportHelper (INode* node)
{
  HelperObject& obj  = (HelperObject&)*node->EvalWorldState (0).obj;
  const char*   type = GetHelperType (obj);

  if (type)
  {
    logBeginFrame (type);

    if (obj.ClassID () == CYLGIZMO_CLASSID)    ExportCylinder (node,obj);
    if (obj.ClassID () == SPHEREGIZMO_CLASSID) ExportSphere   (node,obj);
    if (obj.ClassID () == BOXGIZMO_CLASSID)    ExportBox      (node,obj);
    if (obj.ClassID () == dummyClassID)        ExportDummy    (node,obj);

    logEndFrame ();
  }                  
}
