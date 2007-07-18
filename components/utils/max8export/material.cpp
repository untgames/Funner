#include "export.h"

#include <set>

typedef std::set<Mtl*> MtlSet;

static MtlSet mtl_set;

static const char* GetMapID (Class_ID cid,int subNo)
{
  if (cid == Class_ID(0,0)) return "Envmap"; 
  else if (cid == Class_ID(DMTL_CLASS_ID, 0)) 
  {
    switch (subNo) 
    {
       case ID_AM: return "AmbientMap";
       case ID_DI: return "DiffuseMap";
       case ID_SP: return "SpecularMap"; 
       case ID_SH: return "ShineMap";
       case ID_SS: return "ShineStrengthMap";
       case ID_SI: return "SelfIllumMap";
       case ID_OP: return "OpacityMap"; 
       case ID_FI: return "FilterColorMap";
       case ID_BU: return "BumpMap"; 
       case ID_RL: return "ReflectMap";
       case ID_RR: return "RefractMap";
     }
  }
  
  return "Generic";
}

static void Export (StdUVGen* uvGen)
{
  int mapType = uvGen->GetCoordMapping (0);
  TimeValue t = static_frame;;

  dump<<"map_type\t";
 
  switch (mapType) 
  {
    case UVMAP_EXPLICIT:    dump<<"Explicit"<<endl;    break;
    case UVMAP_SPHERE_ENV:  dump<<"Spherical"<<endl;   break;
    case UVMAP_CYL_ENV:     dump<<"Cylindrical"<<endl; break;
    case UVMAP_SHRINK_ENV:  dump<<"Shrinkwrap"<<endl;  break;
    case UVMAP_SCREEN_ENV:  dump<<"Screen"<<endl;      break;
  }

  dump<<"u_offset\t"<<uvGen->GetUOffs (t)<<endl;
  dump<<"v_offset\t"<<uvGen->GetVOffs (t)<<endl;
  dump<<"u_tiling\t"<<uvGen->GetUScl (t)<<endl;
  dump<<"v_tiling\t"<<uvGen->GetVScl (t)<<endl;  
  dump<<"angle\t"<<uvGen->GetAng (t)<<endl;
  dump<<"blur\t"<<uvGen->GetBlur(t)<<endl;
  dump<<"blur_offset\t"<<uvGen->GetBlurOffs (t)<<endl;
  dump<<"noise_amount\t"<<uvGen->GetNoiseAmt(t)<<endl;
  dump<<"noise_size\t"<<uvGen->GetNoiseSize(t)<<endl;
  dump<<"noise_level\t"<<uvGen->GetNoiseLev(t)<<endl;
  dump<<"noise_phase\t"<<uvGen->GetNoisePhs(t)<<endl; 

  Matrix3 tm;

  logBeginFrame ("tm");

  uvGen->GetUVTransform (tm);

  dump<<tm;

  logEndFrame ();
}


void Export (Texmap* tex,const char* map_id,float amount)
{
  if (!tex) 
    return;

  TSTR className;
  tex->GetClassName(className);

  logBeginFrame (map_id);

  dump<<"name\t'"<<tex->GetName ()<<"'"<<endl;
  dump<<"class\t"<<className.data ()<<endl;
  dump<<"amount\t"<<amount<<endl;
  dump<<"map_channel\t"<<tex->GetMapChannel ()<<endl;

  // Is this a bitmap texture?
  // We know some extra bits 'n pieces about the bitmap texture
  if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)) 
  {
    logBeginFrame ("bitmap");

    dump<<"bitmap\t'"<<((BitmapTex *)tex)->GetMapName()<<"'"<<endl;

    StdUVGen* uvGen = ((BitmapTex *)tex)->GetUVGen();

    if (uvGen) 
      Export (uvGen);

    TextureOutput* texout = ((BitmapTex*)tex)->GetTexout();

    if (texout->GetInvert()) 
      dump<<"invert"<<endl;

    dump<<"filter\t";
    
    switch (((BitmapTex*)tex)->GetFilterType()) 
    {
      case FILTER_PYR: dump<<"Pyramidal"<<endl; break;
      case FILTER_SAT: dump<<"SAT"<<endl;       break;
      default:         dump<<"None"<<endl;      break;
    }

    logEndFrame ();
  }

  for (int i=0;i<tex->NumSubTexmaps();i++) 
    Export (tex->GetSubTexmap(i),GetMapID (tex->ClassID(),i),1.0f);

  logEndFrame ();
}

void Export (Mtl* mtl,Mtl* parent)
{
  if (mtl_set.find (mtl) != mtl_set.end ())
    return;

  mtl_set.insert (mtl);

  if (mtl->NumSubMtls() > 0)  
  {  
    for (int i=0;i<mtl->NumSubMtls();i++) 
    {
      Mtl* subMtl = mtl->GetSubMtl(i);

      if (subMtl) 
        Export (subMtl,mtl);
    }
  }  

  logBeginFrame ("material");

  TimeValue time = static_frame;

//  dump<<"id\t\t"<<id<<endl;

  TSTR className;
  mtl->GetClassName (className);

  dump<<"name\t'"<<mtl->GetName ()<<"'"<<endl;

  if (parent) 
    dump<<"parent\t'"<<parent->GetName ()<<"'"<<endl;

  dump<<"class\t"<<className.data ()<<endl;

  // We know the Standard material, so we can get some extra info
  if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) 
  {
    StdMat* std = (StdMat*)mtl;

    if (std)
    {
      dump<<"ambient\t"<<std->GetAmbient (time)<<endl;
      dump<<"diffuse\t"<<std->GetDiffuse (time)<<endl;
      dump<<"specular\t"<<std->GetSpecular (time)<<endl;
      dump<<"emission\t"<<std->GetSelfIllumColor (time)<<endl;
      dump<<"shiness\t"<<std->GetShininess (time)<<endl;
      dump<<"shine_str\t"<<std->GetShinStr (time)<<endl;
      dump<<"transp\t"<<std->GetXParency (time)<<endl;
      dump<<"wire_size\t"<<std->WireSize (time)<<endl;
      dump<<"shading\t";

      switch (std->GetShading()) 
      {
        case SHADE_CONST: dump<<"Constant"<<endl;  break;
        case SHADE_PHONG: dump<<"Phong"<<endl;     break;
        case SHADE_METAL: dump<<"Metal"<<endl;     break;
        case SHADE_BLINN: dump<<"Blinn"<<endl;     break;
        default:          dump<<"Other"<<endl;     break;
      }

      dump<<"xp_falloff\t"<<std->GetOpacFalloff (time)<<endl;
      dump<<"self_illum\t"<<std->GetSelfIllum (time)<<endl;
      dump<<"falloff\t"<<(std->GetFalloffOut() ? "out" : "in")<<endl;   
      
      if (std->GetTwoSided())  dump<<"two_sided"<<endl;    
      if (std->GetWire())      dump<<"wire"<<endl;    
      if (std->GetWireUnits()) dump<<"wire_units"<<endl;
      if (std->GetFaceMap())   dump<<"face_map"<<endl;
      if (std->GetSoften())    dump<<"soften"<<endl;

      dump<<"xp_type\t";
      
      switch (std->GetTransparencyType()) 
      {
        case TRANSP_FILTER:      dump<<"Filter"<<endl;      break;
        case TRANSP_SUBTRACTIVE: dump<<"Subtractive"<<endl; break;
        case TRANSP_ADDITIVE:    dump<<"Additive"<<endl;    break;
        default:                 dump<<"Other"<<endl;       break;
      }
    }
  }
  else 
  {
    dump<<"ambient\t"<<mtl->GetAmbient ()<<endl;
    dump<<"diffuse\t"<<mtl->GetDiffuse ()<<endl;
    dump<<"specular\t"<<mtl->GetSpecular ()<<endl;
    dump<<"emission\t"<<mtl->GetSelfIllumColor()<<endl;
    dump<<"shiness\t"<<mtl->GetShininess ()<<endl;
    dump<<"shine_str\t"<<mtl->GetShinStr ()<<endl;
    dump<<"transp\t"<<mtl->GetXParency (time)<<endl;
    dump<<"wire_size\t"<<mtl->WireSize (time)<<endl;
  }

  for (int i=0;i<mtl->NumSubTexmaps();i++) 
  {
    Texmap* subTex = mtl->GetSubTexmap(i);

    float amt = 1.0f;

    if (subTex) 
    {
      if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID,0)) 
      {
        if (!((StdMat*)mtl)->MapEnabled(i))
                continue;
        amt = ((StdMat*)mtl)->GetTexmapAmt (i,0);
      }

      Export (subTex,GetMapID (mtl->ClassID(),i),amt);
    }
  }

  if (mtl->NumSubMtls() > 0)  
  {  
    dump<<"sub_mtls_num\t"<<mtl->NumSubMtls()<<endl;
    
    for (int i=0;i<mtl->NumSubMtls();i++) 
    {
      Mtl* subMtl = mtl->GetSubMtl(i);

      if (subMtl) 
        dump<<"sub_material\t'"<<subMtl->GetName()<<"'"<<endl;
    }
  }  

  logEndFrame (); 
}

void ResetMaterials ()
{
  mtl_set.clear ();
}