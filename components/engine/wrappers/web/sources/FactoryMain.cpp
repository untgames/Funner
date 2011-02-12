/**********************************************************\

  Auto-generated FactoryMain.cpp

  This file contains the auto-generated factory methods
  for the Funner Engine project

\**********************************************************/

#include "FactoryDefinitions.h"
#include "FunnerEngine.h"

FB::PluginCore *_getMainPlugin()
{
    return new FunnerEngine();
}

void GlobalPluginInitialize()
{
    FunnerEngine::StaticInitialize();
}

void GlobalPluginDeinitialize()
{
    FunnerEngine::StaticDeinitialize();
}
