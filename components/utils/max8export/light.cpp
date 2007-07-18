#include "export.h"

static void Export (const LightState& ls,GenLight* light,TimeValue t)
{
  logBeginFrame ("light_settings");

  // Frame #
  dump<<"time\t"<<t<<endl;
  dump<<"color\t"<<ls.color<<endl;
  dump<<"intensity\t"<<ls.intens<<endl;
  dump<<"aspect\t"<<ls.aspect<<endl;
  dump<<"affect_diffuse\t"<<ls.affectDiffuse<<endl;
  dump<<"affect_specular\t"<<ls.affectSpecular<<endl;
  dump<<"far_atten\t"<<ls.useAtten<<endl;
  dump<<"far_atten_start\t"<<ls.attenStart<<endl;
  dump<<"far_atten_end\t"<<ls.attenEnd<<endl;
  dump<<"near_atten\t"<<ls.useNearAtten<<endl;
  dump<<"near_atten_start\t"<<ls.nearAttenStart<<endl;
  dump<<"near_atten_end\t"<<ls.nearAttenEnd<<endl;

  if (ls.type != OMNI_LIGHT) 
  {
    dump<<"hot_spot\t"<<ls.hotsize<<endl;
    dump<<"falloff\t"<<ls.fallsize<<endl;
  }

  if (ls.type != DIR_LIGHT && ls.useAtten) 
  {
    dump<<"attenuation_start\t"<<ls.attenStart<<endl;
    dump<<"attenuation_end\t"<<ls.attenEnd<<endl;
  }

  dump<<"tdist\t"<<light->GetTDist (t,FOREVER)<<endl;
  dump<<"map_bias\t"<<light->GetMapBias (t,FOREVER)<<endl;
  dump<<"map_range\t"<<light->GetMapRange (t,FOREVER)<<endl;
  dump<<"map_size\t"<<light->GetMapSize (t,FOREVER)<<endl;
  dump<<"ray_bias\t"<<light->GetRayBias(t, FOREVER)<<endl;
  dump<<"shadow_color\t"<<light->GetShadColor (t,FOREVER)<<endl;
  dump<<"shadow_dens\t"<<light->GetShadMult (t,FOREVER)<<endl;

  logEndFrame ();
}

static void Export (ExclList& list)
{
  logBeginFrame (list.TestFlag (NT_INCLUDE)?"include":"exclude");

  dump<<"count\t"<<list.Count ()<<endl;

  if (list.TestFlag (NT_AFFECT_ILLUM))
    dump<<"affect_illumination"<<endl;

  if (list.TestFlag (NT_AFFECT_SHADOWCAST))
    dump<<"affect_shadow_cast"<<endl;

  logBeginFrame ("nodes");

  for (int i=0;i<list.Count();i++)
    dump<<"node\t'"<<list [i]->GetName()<<"'"<<endl;

  logEndFrame ();

  logEndFrame ();
}

void ExportLight (INode* node)
{
  TimeValue t = static_frame; 
  ObjectState os = node->EvalWorldState(t);

  logBeginFrame ("light");
 
  GenLight* light = (GenLight*)os.obj;

  LightState ls;

  Interval valid     = FOREVER;
  Interval animRange = iface->GetAnimRange ();

  light->EvalLightState (t,valid,&ls);

  // This is part os the lightState, but it doesn't
  // make sense to output as an animated setting so
  // we dump it outside of ExportLightSettings()

  dump<<"type\t";

  switch (ls.type) {
    case OMNI_LIGHT:  dump<<"omni"<<endl;        break;
    case TSPOT_LIGHT: dump<<"target"<<endl;      break;
    case DIR_LIGHT:   dump<<"directional"<<endl; break;
    case FSPOT_LIGHT: dump<<"free"<<endl;        break;
    default:          dump<<"other"<<endl;       break;
  }

/*
  // If we have a target object, export Node TM for the target too.
  INode* target = node->GetTarget();
  if (target) {
          ExportNodeTM(target, indentLevel);
  }
*/

  int shadowMethod = light->GetShadowMethod();

  dump<<"shadows\t"<<(shadowMethod == LIGHTSHADOW_NONE ? "off" :
                  shadowMethod == LIGHTSHADOW_MAPPED ? "mapped" :
                  "raytraced")<<endl;

  dump<<"enable\t"<<light->GetUseLight()<<endl;
  dump<<"spot_shape\t"<<(light->GetSpotShape() == RECT_LIGHT ? "rect" : "circle")<<endl;
  dump<<"use_global\t"<<light->GetUseGlobal()<<endl;
  dump<<"abs_map_bias\t"<<light->GetAbsMapBias()<<endl;
  dump<<"overshoot\t"<<light->GetOvershoot()<<endl;

  Export (*light->GetExclList());

  // Export light settings for frame 0
  Export (ls,light,t);

  // Export animated light settings
/*  if (!valid.InInterval (animRange) && GetIncludeCamLightAnim()) 
  {
    TimeValue t = animRange.Start();
    
    while (1) 
    {
      valid = FOREVER; // Extend the validity interval so the camera can shrink it.

      light->EvalLightState (t,valid,&ls);

      t = valid.Start() < animRange.Start() ? animRange.Start() : valid.Start();
      
      Export (ls,light,t);
      
      if (valid.End() >= animRange.End()) break;
      else  t = (valid.End()/GetTicksPerFrame()+GetMeshFrameStep()) * GetTicksPerFrame();
    }
  }*/
  
  logEndFrame ();
}
