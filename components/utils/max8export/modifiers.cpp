#include "export.h"

#include <modstack.h>

ISkin* FindSkin (INode* node)
{
  IDerivedObject* derivedObj = NULL; 

  for (Object* obj=node->GetObjectRef();obj && obj->SuperClassID() == GEN_DERIVOB_CLASS_ID;obj=derivedObj->GetObjRef())
  {
    derivedObj = static_cast<IDerivedObject*> (obj);

    for (int i=0;i<derivedObj->NumModifiers();i++)
    {
      Modifier* mod = derivedObj->GetModifier (i);

      if (mod->ClassID() == SKIN_CLASSID)
        return (ISkin*)mod->GetInterface (I_SKIN);
    }    
  }

  return NULL;
}