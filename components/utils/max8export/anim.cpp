#include "export.h"

template <class KeyType>
static void EnumKeys (IKeyControl& ctrl,const char* type)
{
  dump<<"type "<<type<<endl;

  KeyType key;
  int     count = ctrl.GetNumKeys ();

  for (int i=0;i<count;i++)
  {
    ctrl.GetKey (i,&key);

    dump<<"key \t"<<key<<endl;
  }
}

static void ExportPosKeys (Control& ctrl)
{
  IKeyControl* ikc = GetKeyControlInterface ((&ctrl));

  if (ikc)
  {
    if (ctrl.ClassID() != Class_ID (TCBINTERP_POSITION_CLASS_ID,0)   &&
        ctrl.ClassID() != Class_ID (HYBRIDINTERP_POSITION_CLASS_ID,0) &&
        ctrl.ClassID() != Class_ID (LININTERP_POSITION_CLASS_ID,0))
          return;

    logBeginFrame ("pos");

    if (ctrl.ClassID () == Class_ID (TCBINTERP_POSITION_CLASS_ID,0))
        EnumKeys<ITCBPoint3Key> (*ikc,"tcb");

    if (ctrl.ClassID () == Class_ID (HYBRIDINTERP_POSITION_CLASS_ID,0))
        EnumKeys<IBezPoint3Key> (*ikc,"bezie");

    if (ctrl.ClassID () == Class_ID (LININTERP_POSITION_CLASS_ID,0))
        EnumKeys<ILinPoint3Key> (*ikc,"linear");

    logEndFrame ();
  }
}

static void ExportRotKeys (Control& ctrl)
{
  IKeyControl* ikc = GetKeyControlInterface ((&ctrl));

  if (ikc)
  {
    if (ctrl.ClassID() != Class_ID (TCBINTERP_ROTATION_CLASS_ID,0)   &&
        ctrl.ClassID() != Class_ID (HYBRIDINTERP_ROTATION_CLASS_ID,0) &&
        ctrl.ClassID() != Class_ID (LININTERP_ROTATION_CLASS_ID,0))
          return;

    logBeginFrame ("rot");

    if (ctrl.ClassID () == Class_ID (TCBINTERP_ROTATION_CLASS_ID,0))
        EnumKeys<ITCBRotKey> (*ikc,"tcb");

    if (ctrl.ClassID () == Class_ID (HYBRIDINTERP_ROTATION_CLASS_ID,0))
        EnumKeys<IBezQuatKey> (*ikc,"bezie");

    if (ctrl.ClassID () == Class_ID (LININTERP_ROTATION_CLASS_ID,0))
        EnumKeys<ILinRotKey> (*ikc,"linear");

    logEndFrame ();
  } 
}

static void ExportScaleKeys (Control& ctrl)
{
  IKeyControl* ikc = GetKeyControlInterface ((&ctrl));

  if (ikc)
  {
    if (ctrl.ClassID() != Class_ID (TCBINTERP_SCALE_CLASS_ID,0)   &&
        ctrl.ClassID() != Class_ID (HYBRIDINTERP_SCALE_CLASS_ID,0) &&
        ctrl.ClassID() != Class_ID (LININTERP_SCALE_CLASS_ID,0))
          return;

    logBeginFrame ("scale");

    if (ctrl.ClassID () == Class_ID (TCBINTERP_SCALE_CLASS_ID,0))
        EnumKeys<ITCBScaleKey> (*ikc,"tcb");

    if (ctrl.ClassID () == Class_ID (HYBRIDINTERP_SCALE_CLASS_ID,0))
        EnumKeys<IBezScaleKey> (*ikc,"bezie");

    if (ctrl.ClassID () == Class_ID (LININTERP_SCALE_CLASS_ID,0))
        EnumKeys<ILinScaleKey> (*ikc,"linear");

    logEndFrame ();
  } 
}

static bool IsKnownController (Control* ctrl)
{
  if (ctrl)
  {
    ulong partA = ctrl->ClassID ().PartA (), 
          partB = ctrl->ClassID ().PartB ();

    if (!partB)
    {
      switch (partA) 
      {
        case TCBINTERP_POSITION_CLASS_ID:
        case TCBINTERP_ROTATION_CLASS_ID:
        case TCBINTERP_SCALE_CLASS_ID:
        case HYBRIDINTERP_POSITION_CLASS_ID:
        case HYBRIDINTERP_ROTATION_CLASS_ID:
        case HYBRIDINTERP_SCALE_CLASS_ID:
        case LININTERP_POSITION_CLASS_ID:
        case LININTERP_ROTATION_CLASS_ID:
        case LININTERP_SCALE_CLASS_ID:
          return true;
      }
    }
  }

  return false;
}

bool TestAnim (Control* ctrl)
{
  return ctrl && IsKnownController (ctrl) && GetKeyControlInterface (ctrl) && 
         GetKeyControlInterface (ctrl)->GetNumKeys ();
}

static bool TestAnim (INode* node,bool& isPos,bool& isRot,bool& isScale)
{
  Control* ctrl = node->GetTMController();

  if (ctrl)
  {
    isPos   = TestAnim (ctrl->GetPositionController());
    isRot   = TestAnim (ctrl->GetRotationController());
    isScale = TestAnim (ctrl->GetScaleController());

    return isPos || isScale || isRot;
  }
  else return false;
}

void ExportAnimKeys (INode* node)
{
  bool isPos    = false,
       isRot    = false,
       isScale  = false;

  if (TestAnim (node,isPos,isRot,isScale))
  {
    logBeginFrame ("anim_keys");

    dump<<"name\t"<<node->GetName ()<<endl;
    
    if (isPos)    ExportPosKeys   (*node->GetTMController()->GetPositionController());
    if (isRot)    ExportRotKeys   (*node->GetTMController()->GetRotationController());
    if (isScale)  ExportScaleKeys (*node->GetTMController()->GetScaleController());

    logEndFrame ();
  }
}
