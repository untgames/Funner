#include "export.h"

static void Export (const CameraState& cs,CameraObject* cam,TimeValue t)
{
  logBeginFrame ("camera_settings");

  dump<<"time\t"<<t<<endl;

  if (cs.manualClip) 
  {
    dump<<"hitcher\t"<<cs.hither<<endl;
    dump<<"yon\t"<<cs.yon<<endl;
  }

  dump<<"near_range\t"<<cs.nearRange<<endl;
  dump<<"far_range\t"<<cs.farRange<<endl;
  dump<<"manual_clip\t"<<cs.manualClip<<endl;
  dump<<"near_clip\t"<<cs.hither<<endl;
  dump<<"far_clip\t"<<cs.yon<<endl;
  dump<<"fov\t"<<cs.fov<<endl;
  dump<<"tdist\t"<<cam->GetTDist(t)<<endl;

  logEndFrame ();
}

void ExportCamera (INode* node)
{
  logBeginFrame ("camera");

  INode* target = node->GetTarget();

  if (target) dump<<"type\ttarget"<<endl;
  else        dump<<"type\tfree"<<endl;

  // If we have a target object, export animation keys for the target too.
/*  if (target) {
          ExportNodeTM(target, indentLevel);
  }
*/
  
  CameraState   cs;
  TimeValue     t         = static_frame;
  Interval      valid     = FOREVER;
  Interval      animRange = iface->GetAnimRange();  
  ObjectState   os        = node->EvalWorldState (t);
  CameraObject* cam       = (CameraObject*)os.obj;
  
  cam->EvalCameraState (t,valid,&cs);
  
  Export (cs,cam,t);

/*
  // Export animated camera settings
  if (!valid.InInterval(animRange) && GetIncludeCamLightAnim()) {

          fprintf(pStream,"%s\t%s {\n", indent.data(), ID_CAMERA_ANIMATION);

          TimeValue t = animRange.Start();
          
          while (1) {
                  valid = FOREVER; // Extend the validity interval so the camera can shrink it.
                  cam->EvalCameraState(t,valid,&cs);

                  t = valid.Start() < animRange.Start() ? animRange.Start() : valid.Start();
                  
                  // Export the camera settings at this frame
                  ExportCameraSettings(&cs, cam, t, indentLevel+1);
                  
                  if (valid.End() >= animRange.End()) {
                          break;
                  }
                  else {
                          t = (valid.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
                  }
          }

          fprintf(pStream,"%s\t}\n", indent.data());
  }
*/
  
  logEndFrame ();
}
