#pragma once

#include <windows.h>
#include <max.h>
#include <decomp.h>
#include <stdmat.h>
#include <iparamb2.h>
#include <iskin.h>

#include <stdarg.h>

#include "log.h"

extern Interface*    iface;      //Интерфейс доступа
extern ExpInterface* exp_iface;   //Интерфейс доступа для экспорта
extern TimeValue     static_frame;  //Статичный кадр  
extern INode*        root;          //корень

extern Interval      anim_range;
extern float         anim_fps;        

const float ANIM_FRAMES_STEP = 100;

///флаги экспорта

extern bool flag_export_materials;
extern bool flag_export_selected;
extern bool flag_export_nodes;
extern bool flag_export_lights;
extern bool flag_export_cameras;
extern bool flag_export_shapes;
extern bool flag_export_geometry;
extern bool flag_export_helpers;

///собственно экспорт

void Preprocess     (INode*);

void Export         ();
void Export         (INode*,INode*);
void Export         (Mtl* mtl,Mtl* parent_mtl);
void ExportMesh     (INode*,TimeValue=0);
void ExportAnimKeys (INode*);
void ExportHelper   (INode*);
void ExportLight    (INode*);
void ExportCamera   (INode*);

//модификаторы
ISkin* FindSkin (INode*);

///управаление прогрессом
void logProgressNext ();

//есть ли анимация
bool TestAnim (Control*);

//сброс материалов
void ResetMaterials ();

//тип helper'а
const char* GetHelperType (Object&);
