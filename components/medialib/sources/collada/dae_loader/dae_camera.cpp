#include "shared.h"

using namespace media::collada;

/*
    Разбор библиотеки камер
*/

void DaeParser::ParseLibraryCameras (Parser::Iterator iter)
{
  if (!test (iter, "camera"))
  {
    LogError (iter, "Incorrect 'library_cameras' node. Must be at least one 'camera' sub-tag");
    return;
  }
  
  LogScope scope (iter, *this);

  for_each_child (iter, "camera", bind (&DaeParser::ParseCamera, this, _1));
}

/*
    Разбор камеры
*/

void DaeParser::ParseCamera (Parser::Iterator iter)
{
  const char* id = get<const char*> (iter, "id");
  
  if (!id)
  {
    LogError (iter, "No id");
    return;
  }
  
  LogScope scope (iter, *this);
  
  iter = iter->First ("optics.technique_common");
  
  if (!iter)
    return;

    //чтение типа камеры
    
  static String2Value<CameraType> types [] = {
    {"perspective",  CameraType_Perspective},
    {"orthographic", CameraType_Orthographic},
  };    
  
  static const size_t types_count = sizeof (types) / sizeof (*types);
  
  size_t i;
  CameraType type;
    
  for (i=0; i<types_count; i++)
    if (test (iter, types [i].string))
    {
      type = types [i].value;      
      iter = iter->First (types [i].string);
      break;
    }
    
  if (i == types_count)
  {
    LogError (iter, "Incorrect 'technique_common' tag. No camera type sub-tag (one of 'perspective', 'orthographic')");
    return;
  }    
  
    //создание камеры
    
  Camera& camera = model.Cameras ().Create (id);
  
    //установка типа камеры
    
  camera.SetType (type);
  
    //чтение параметров камеры

  static String2Value<CameraParam> params [] = {
    {"xfov.#text",         CameraParam_XFov},
    {"yfov.#text",         CameraParam_YFov},
    {"xmag.#text",         CameraParam_XMagnitude},
    {"ymag.#text",         CameraParam_YMagnitude},
    {"znear.#text",        CameraParam_ZNear},
    {"zfar.#text",         CameraParam_ZFar},
    {"aspect_ratio.#text", CameraParam_AspectRatio}
  };
  
  static const size_t params_num = sizeof (params) / sizeof (*params);

  for (i=0; i<params_num; i++)
    if (test (iter, params [i].string))
    {
      float value = 0.0f;
      
      if (CheckedRead (iter, params [i].string, value))
        camera.SetParam (params [i].value, value);
    }
}
