#include "export.h"

static const char* GetType (Object& obj)
{
  switch (obj.SuperClassID())
  {
    case GEOMOBJECT_CLASS_ID: return flag_export_geometry ? "mesh" : NULL; 
    case CAMERA_CLASS_ID:     return flag_export_cameras ? "camera" : NULL;
    case LIGHT_CLASS_ID:      return flag_export_lights ? "light" : NULL; 
    case SHAPE_CLASS_ID:      return flag_export_shapes ? "shape" : NULL;
    case HELPER_CLASS_ID:     return flag_export_helpers ? GetHelperType (obj) : NULL; 
    default:                  return NULL; 
  }
}

static Matrix3 GetNodeTM (INode* node,float t)
{
  Matrix3 tm = node->GetObjTMAfterWSM (t);

  if (node->GetParentNode ())
    tm = tm * Inverse (node->GetParentNode()->GetObjTMAfterWSM(t));

  return tm;
}

static Matrix3 GetPivotTM (INode* node,float t)
{
  Matrix3 tm = node->GetNodeTM (t);

  if (node->GetParentNode ())
    tm = tm * Inverse (node->GetParentNode()->GetNodeTM (t));  

  tm = tm * Inverse (GetNodeTM (node,t));

  return tm;
}

static bool TestAnim (INode* node)
{
  return node->GetTMController() != NULL;
}

void ExportNodeTM (INode* node)
{
  logBeginFrame ("transform");

  Control* ctrl = node->GetTMController();

  if (ctrl)
  {
    DWORD iFlags = ctrl->GetInheritanceFlags();

    dump<<"inherit_pos"
        <<"\t"<<(INHERIT_POS_X & iFlags ? 1 : 0)
        <<"\t"<<(INHERIT_POS_Y & iFlags ? 1 : 0)
        <<"\t"<<(INHERIT_POS_Z & iFlags ? 1 : 0)
        <<endl;

    dump<<"inherit_rot"
        <<"\t"<<(INHERIT_ROT_X & iFlags ? 1 : 0)
        <<"\t"<<(INHERIT_ROT_Y & iFlags ? 1 : 0)
        <<"\t"<<(INHERIT_ROT_Z & iFlags ? 1 : 0)
        <<endl;

    dump<<"inherit_scale"
        <<"\t"<<(INHERIT_SCL_X & iFlags ? 1 : 0)
        <<"\t"<<(INHERIT_SCL_Y & iFlags ? 1 : 0)
        <<"\t"<<(INHERIT_SCL_Z & iFlags ? 1 : 0)
        <<endl;
  }

  logBeginFrame ("node_tm");  

  dump<<GetNodeTM (node,static_frame);

  logEndFrame ();

  logBeginFrame ("pivot");  

  dump<<GetPivotTM (node,static_frame);

  logEndFrame ();

/*  AffineParts ap;
  float       rotAngle;
  Point3      rotAxis;
  float       scaleAngle;
  Point3      scaleAxis;
  
  decomp_affine (pivot,&ap);

  AngAxisFromQ (ap.q,&rotAngle,rotAxis);
  AngAxisFromQ (ap.u,&scaleAngle,scaleAxis);


  dump<<"pos\t\t"<<ap.t<<endl;
  dump<<"rot_axis\t"<<rotAxis<<endl;
  dump<<"rot_angle\t"<<rotAngle<<endl;;
  dump<<"scale\t\t"<<ap.k<<endl;
  dump<<"scale_axis\t"<<scaleAxis<<endl;
  dump<<"scale_angle\t"<<scaleAngle<<endl;
*/

    ///!! добавить определение присутствия анимации  

  if (TestAnim (node))
  {
    logBeginFrame ("tm_anim");

    dump<<"count\t"<<(anim_range.End()-anim_range.Start())/GetTicksPerFrame ()<<endl;

    for (float t=anim_range.Start();t<anim_range.End();t+=GetTicksPerFrame())
    {
      logBeginFrame ("tm");

      dump<<"time\t"<<t/GetTicksPerFrame()/GetFrameRate ()<<endl;
      dump<<GetNodeTM (node,t);

      logEndFrame ();
    }

    logEndFrame ();
  }

  logEndFrame ();
}

void Export (INode* node,INode* parent)
{
  if (!flag_export_selected || node->Selected ())
  {
    const char* type = NULL;
    ObjectState os = node->EvalWorldState(0);

    if (os.obj)
      type = GetType (*os.obj);

    if (!type)
      type = "";

    logBeginFrame ("node",type);

    dump<<"name\t'"<<node->GetName()<<"'"<<endl;

  //  if (node->GetParentNode () && !node->GetParentNode ()->IsRootNode ())
    if (node->GetParentNode ())
      dump<<"parent\t'"<<node->GetParentNode ()->GetName ()<<"'"<<endl;

    if (node->GetMtl ())
        dump<<"material\t'"<<node->GetMtl ()->GetName ()<<"'"<<endl;

    Color col (GetRValue (node->GetWireColor()) / 255.0,
               GetGValue (node->GetWireColor()) / 255.0,
               GetBValue (node->GetWireColor()) / 255.0);

    dump<<"wire_color\t"<<col<<endl;

    if (!node->CastShadows ()) dump<<"no_cast_shadows"<<endl;
    if (!node->RcvShadows())   dump<<"no_recv_shadows"<<endl;
    if (node->GetBackCull())   dump<<"back_cull"<<endl;
    if (node->MotBlur())       dump<<"motion_blur"<<endl;
    if (!node->Renderable() || node->IsHidden())   dump<<"hidden"<<endl;

    ExportNodeTM   (node);
    ExportAnimKeys (node);

    TSTR prop_buffer = "";

    node->GetUserPropBuffer (prop_buffer);

    dump<<prop_buffer<<endl;    

    os = node->EvalWorldState(0); //unknown bug at enum node tm

    if (os.obj)// && (type = GetType (os.obj->SuperClassID ())))
    {        
      switch (os.obj->SuperClassID())
      {
        case GEOMOBJECT_CLASS_ID: if (flag_export_geometry) ExportMesh (node);  break; //!!!!!!!
        case CAMERA_CLASS_ID:     if (flag_export_cameras) ExportCamera (node); break;
        case LIGHT_CLASS_ID:      if (flag_export_lights) ExportLight  (node); break;
  //        case SHAPE_CLASS_ID:      ShapeExport  (node); break;
        case HELPER_CLASS_ID:   if (flag_export_helpers) ExportHelper (node); break; 
      }
    }

    logProgressNext ();

    for (int i=0;i<node->NumberOfChildren ();i++) 
        Export (node->GetChildNode (i),node);

    logEndFrame ();              
  }
  else
  {
    logProgressNext ();

    for (int i=0;i<node->NumberOfChildren ();i++) 
        Export (node->GetChildNode (i),node);    
  }
}
