#include "shared.h"

using namespace media::collada;

/*
    Разбор библиотеки камер
*/

void DaeParser::ParseLibraryCameras (Parser::Iterator iter)
{
  if (!iter->First ("camera"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_cameras' node. Must be at least one 'camera' sub-tag");
    return;
  }

  for_each_child (*iter, "camera", bind (&DaeParser::ParseCamera, this, _1));
}

/*
    Разбор камеры
*/

void DaeParser::ParseCamera (Parser::Iterator iter)
{
  const char* id = get<const char*> (*iter, "id");

  iter = get_first_child (*iter, "optics.technique_common");

    //чтение типа камеры

  static String2Value<CameraType> types [] = {
    {"perspective",  CameraType_Perspective},
    {"orthographic", CameraType_Orthographic},
  };    
  
  static const size_t types_count = sizeof (types) / sizeof (*types);
  
  size_t i;
  CameraType type = CameraType_Orthographic;
    
  for (i=0; i<types_count; i++)
    if (iter->First (types [i].string))
    {
      type = types [i].value;
      iter = iter->First (types [i].string);
      break;
    }

  if (i == types_count)
    raise_parser_exception (*iter, "Incorrect 'technique_common' tag. No camera type sub-tag (one of 'perspective', 'orthographic')");

    //создание камеры
    
  Camera camera;
  
  camera.SetId (id);
  
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
    if (common::ParseNode param_node = iter->First (params [i].string))
      camera.SetParam (params [i].value, get<float> (param_node, ""));

    //добавление камеры в библиотеку

  model.Cameras ().Insert (id, camera);
}
