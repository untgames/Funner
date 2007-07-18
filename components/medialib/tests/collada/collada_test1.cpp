#include <stl/string>             
#include <common/exception.h>
#include <media/collada.h>
#include <stdio.h>

using namespace common;
using namespace stl;
using namespace medialib;

const char* file_name = "data/cube.dae";

void print_space(int count)
{
  for(int i = 0; i < count; i++)
    printf(" ");
}

void print_camera(ColladaCamera* camera, int level)
{
  print_space(level++ * 2);
  printf("Camera: %s\n", camera->EntityID());
  print_space(level * 2);
  printf("type:   ");
  switch (camera->CameraType())
  {
    case (COLLADA_CAMERA_PERSPECTIVE):  printf("perspective\n");  break;
    case (COLLADA_CAMERA_ORTHOGRAPHIC): printf("orthographic\n"); break;
  }
  print_space(level * 2);
  printf("znear:  %f\n", camera->ZNear());
  print_space(level * 2);
  printf("zfar:   %f\n", camera->ZFar());
  print_space(level * 2);
  printf("aspect: %f\n", camera->AspectRatio());
  print_space(level * 2);
  printf("xfov:   %f\n", camera->XFov());
  print_space(level * 2);
  printf("yfov:   %f\n", camera->YFov());
}

void print_effect (ColladaEffect* effect, int level)
{
  print_space(level++ * 2);
  printf("\nEffect: %s\n", effect->EntityID());
  print_space(level * 2);
  printf("shader type: ");
  switch (effect->ShaderType())
  {
    case(COLLADA_SHADER_CONSTANT): printf("constant\n"); break;
    case(COLLADA_SHADER_LAMBERT):  printf("lambert\n");  break;
    case(COLLADA_SHADER_PHONG):    printf("phong\n");    break;
    case(COLLADA_SHADER_BLINN):    printf("blinn\n");    break;
  }

  print_space(level * 2);
  printf("reflectivity: %f\n", effect->Reflectivity());
  print_space(level * 2);
  printf("transparency: %f\n", effect->Transparency());
  print_space(level * 2);
  printf("refraction index: %f\n", effect->RefractonIndex());
  print_space(level * 2);
  printf("shininess: %f\n", effect->Shininess());
  if(effect->HasTexture(COLLADA_MAP_BUMP))
  {
    print_space(level * 2);
    printf("Has bump texture\n");
  }
}

void print_light(ColladaLight* light, int level)
{
  print_space(level++ * 2);
  printf("Light: %s\n", light->EntityID());
  print_space(level * 2);
  printf("type:                  ");
  switch (light->LightType ())
  {
    case (COLLADA_LIGHT_AMBIENT): printf("ambient\n");     break;
    case (COLLADA_LIGHT_DIRECT):  printf("directional\n"); break;
    case (COLLADA_LIGHT_SPOT):    printf("spot\n");        break;
    case (COLLADA_LIGHT_POINT):   printf("point\n");       break;
  }
  print_space(level * 2);
  printf("color:                 %f %f %f\n", light->Color().x, light->Color().y, light->Color().z);
  print_space(level * 2);
  printf("attenuation constant:  %f\n", light->AttenuationConstant());
  print_space(level * 2);
  printf("attenuation linear:    %f\n", light->AttenuationLinear());
  print_space(level * 2);
  printf("attenuation quadratic: %f\n", light->AttenuationQuadratic());
  print_space(level * 2);
  printf("falloff angle:         %f\n", light->FalloffAngle());
  print_space(level * 2);
  printf("falloff exponent:      %f\n", light->FalloffExponent());
}

void print_mesh(ColladaMesh* mesh, int level)
{
  print_space(level++ * 2);
  printf("Mesh: %s\n", mesh->EntityID());
  print_space(level++ * 2);
  printf("surfaces count: %d\n", mesh->SurfacesCount());
  for(size_t j = 0; j < mesh->SurfacesCount(); j++)
  {
    ColladaSurface cur_surface = mesh->Surface(j);
    print_space(level++ * 2);
    printf("surface %d:\n", j);
    print_space(level * 2);
    printf("type: ");
    switch (cur_surface.PrimitiveType())
    {
      case(COLLADA_PRIMITIVE_LINE_LIST):      printf("linelist\n");  break;
      case(COLLADA_PRIMITIVE_LINE_STRIP):     printf("linestrip\n"); break;
      case(COLLADA_PRIMITIVE_TRIANGLE_LIST):  printf("trilist\n");   break;
      case(COLLADA_PRIMITIVE_TRIANGLE_FAN):   printf("trifan\n");    break;
      case(COLLADA_PRIMITIVE_TRIANGLE_STRIP): printf("tristrip\n");  break;
    }
    print_space(level * 2);
    printf("vertex count:      %d\n", cur_surface.VerticesCount());    
    print_space(level * 2);
    printf("index count:       %d\n", cur_surface.IndicesCount());    
    print_space(level * 2);
    printf("textures count:    %d\n", cur_surface.TextureChannels());    
    for(size_t k = 0; k < cur_surface.TextureChannels(); k++)
    {
      print_space(level * 2+2);
      printf("channel %d name:  %s\n", k, cur_surface.TextureChannelName(k));
    }
    print_space(level * 2);
    printf("material name:     %s\n", cur_surface.Material());    
    print_space(level * 2);
    printf("has vertex colors: %d\n", cur_surface.HasVertexColors());    
  }
}

void print_controller(ColladaController* controller, int level)
{
  print_space(level++ * 2);
  printf("Controller: %s\n", controller->EntityID());
  print_space(level * 2);
  printf("type: ");
  switch (controller->Type ())
  {
    case (COLLADA_CONTROLLER_SKIN):  printf("          skin\n");  break;
    case (COLLADA_CONTROLLER_MORPH): printf("          morph\n"); break;
  }
}

void print_node(ColladaNode* node, int level)
{
  printf("\n");
  print_space(level++ * 2);
  printf("Node:\n");
  print_space(level * 2);
  printf("ID:   %s\n", node->EntityID());
  print_space(level * 2);
  printf("SID:  %s\n", node->SubID());
  print_space(level * 2);
  printf("name: %s\n", node->Name());
  if(node->LayersCount())
  {
    print_space(level * 2);
    printf("layers: ");
    for (size_t i = 0; i < node->LayersCount (); i++)
      printf("%s ", node->Layer(i));
    printf("\n");
  }
  print_space(level * 2);
  printf("Content:\n");
  if(node->CamerasCount())
  for (size_t i = 0; i < node->CamerasCount(); i++)
    print_camera(&node->Camera(i), level+1);
  if(node->LightsCount())
  for (size_t i = 0; i < node->LightsCount(); i++)
    print_light(&node->Light(i), level+1);
  if(node->MeshesCount())
  for (size_t i = 0; i < node->MeshesCount(); i++)
    print_mesh(&node->Mesh(i), level+1);
//  if(node->ControllersCount())
//  for (size_t i = 0; i < node->ControllersCount(); i++)
//    print_controller(&node->Controller(i), level+1);
  for(size_t i = 0; i < node->ChildrenCount(); i++)
    print_node(&node->Child(i), level + 1);
}

void print_animation(ColladaAnimation* animation, int level)
{
  printf("\n");
  print_space(level * 2);
  printf("Line: %d\n", animation->ParseLineNumber());
  print_space(level * 2);
  printf ("anim_lib.size = %d\n", animation->ChildrenCount());
  print_space(level * 2);
//  printf("Animation %s:\n", animation->id.c_str());
  if (animation->ChannelsCount())
  {
    for(size_t i = 0; i < animation->ChannelsCount(); i++)
    {
      print_space(level * 2 + 2);
      printf("Channel %d:\n", i);
      print_space(level * 2 + 4);
      printf("Target %s:\n", animation->Channel(i).TargetName());
//      print_space(level * 2 + 4);
//      printf("Frames in channel: %d\n", animation->Channel(i).input.size());
    }
  }
  for(size_t i = 0; i < animation->ChildrenCount(); i++)
    print_animation(&animation->Child(i), level + 1);
}

int main ()
{
  try
  {
//  int count;
  Parser::Log log;

  printf ("Results of collada_test1:\n");

  Collada p_collada (&log, file_name);

  printf ("\n\n---  Collada statistic  ---\n");

  ColladaAnimationLib anim_lib(p_collada);
  ColladaEffectLib    effect_lib (p_collada);

  ColladaVisualScene root_scene = p_collada.ActiveScene();

  printf ("anim_lib.size = %d\n", anim_lib.Count());
  for(size_t i = 0; i < anim_lib.Count(); i++)
  {
    print_animation(&anim_lib[i], 0);
  }

  printf ("\neffect_lib.size = %d\n", effect_lib.Count());
  for(size_t i = 0; i < effect_lib.Count(); i++)
    print_effect(&effect_lib[i], 1);

  printf("\n\nRoot scene: %s\n", root_scene.EntityID());
  printf("  nodes count: %d\n", root_scene.NodesCount());
  for(size_t i = 0; i < root_scene.NodesCount(); i++)
    print_node(&root_scene.Node(i), 1);
  
/*
  count = p_collada.MaterialCount();
  printf ("\n\nMaterials loaded: %d\n", count);
  for (int i = 0; i < count; i++)
  {
    ColladaMaterial *cur_material = p_collada.Material(i);

    printf("\nMaterial %s:\n", cur_material->id.c_str());
    printf("  effect: %s\n", cur_material->effect.c_str());
  }

  count = p_collada.ImageCount();
  printf ("\n\nImages loaded: %d\n", count);
  for (int i = 0; i < count; i++)
  {
    ColladaImage *cur_image = p_collada.Image(i);

    printf("\nImage %s:\n", cur_image->id.c_str());
    printf("  URL:    %s\n", cur_image->url.c_str());
  }

  count = p_collada.AnimationCount();
  printf ("\n\nAnimations loaded: %d\n", count);
  for (int i = 0; i < count; i++)
  {
    ColladaAnimation *cur_animation = p_collada.Animation(i);

    print_animation(cur_animation, 0);
  }

  count = p_collada.AnimationClipCount();
  printf ("\n\nAnimation clips loaded: %d\n", count);
  for (int i = 0; i < count; i++)
  {
    ColladaAnimationClip *cur_clip = p_collada.AnimationClip(i);
    
    printf("\nAnimation clip %s:\n", cur_clip->id.c_str());
    printf("  start:                     %f\n", cur_clip->start);
    printf("  end:                       %f\n", cur_clip->end);
    printf("  animation instances count: %d\n", cur_clip->instances_anim.size());
  } */

  return 0;  
  }
  catch (std::exception& exception)
  {                                               
    printf ("exception: %s\n",exception.what ()); 
  }                                               
}

