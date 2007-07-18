#include "export.h"

ofstream dout ("c:/temp/max_export/runlog");

static int total_process = 0, cur_process = 0;

bool flag_export_selected  = false;
bool flag_export_materials = true;
bool flag_export_nodes     = true;
bool flag_export_lights    = true;
bool flag_export_cameras   = true;
bool flag_export_shapes    = true;
bool flag_export_geometry  = true;
bool flag_export_helpers   = true;

Interval anim_range;
float    anim_fps;  

static DWORD WINAPI Progress (LPVOID arg);

void Export ()
{
  anim_range = iface->GetAnimRange ();  
  anim_fps   = GetFrameRate ();

  ResetMaterials ();

  Preprocess (iface->GetRootNode ());

  iface->ProgressStart (_T("Export scene..."),TRUE,Progress,NULL); 

  dump<<"anim_range\t"<<anim_range.Start()/GetFrameRate()/GetTicksPerFrame()<<" "
                      <<anim_range.End()/GetFrameRate()/GetTicksPerFrame()<<" "
                      <<anim_range.Duration()/GetFrameRate()/GetTicksPerFrame()<<endl;
  dump<<"fps\t"<<anim_fps<<endl;  
  dump<<"ambient_light\t"<<iface->GetAmbient (static_frame,FOREVER)<<endl;

  ViewExp* vp  = iface->GetActiveViewport ();

  if (vp)
  {
    INode* cam = vp->GetViewCamera ();

    if (cam)
      dump<<"active_camera\t'"<<cam->GetName()<<"'"<<endl;

   iface->ReleaseViewport (vp);
  }

  Export (root,NULL);

  iface->ProgressEnd ();
}

void Preprocess (INode* node)
{
  if (node)
  {  
    total_process++;

    if (node->GetMtl ())
      Export (node->GetMtl (),NULL);

    for (int i=0;i<node->NumberOfChildren ();i++) 
      if (!flag_export_selected || node->GetChildNode (i)->Selected ())
        Preprocess (node->GetChildNode (i));
  }
}

void logProgressNext ()
{
  cur_process++;
   
  iface->ProgressUpdate ((int)((float)cur_process/total_process*100.0f)); 
}

DWORD WINAPI Progress (LPVOID arg)
{
  iface->ProgressUpdate ((int)((float)cur_process/total_process*100.0f)); 

  if (cur_process >= total_process)
    iface->ProgressEnd ();

  return 0;
}                
