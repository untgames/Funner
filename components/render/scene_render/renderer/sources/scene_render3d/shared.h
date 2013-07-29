#ifndef RENDER_SCENE_RENDER3D_SHARED_HEADER
#define RENDER_SCENE_RENDER3D_SHARED_HEADER
               
#include <stl/algorithm>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>
#include <xtl/visitor.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <sg/light.h>
#include <sg/camera.h>
#include <sg/scene.h>
#include <sg/visual_model.h>

#include <render/scene_render.h>

namespace render
{

namespace scene_render3d
{

//forwards
class  Light;
class  Node;
class  Render;
class  Renderable;
class  Scene;
class  Technique;
struct TraverseResult;
class  View;
class  VisualModel;
class  IVisitor;
class  ITraverseResultCache;

//pointers
typedef xtl::intrusive_ptr<Light>      LightPtr;
typedef xtl::intrusive_ptr<Node>       NodePtr;
typedef xtl::intrusive_ptr<Render>     RenderPtr;
typedef xtl::intrusive_ptr<Renderable> RenderablePtr;
typedef xtl::intrusive_ptr<Scene>      ScenePtr;
typedef xtl::intrusive_ptr<Technique>  TechniquePtr;
typedef xtl::intrusive_ptr<View>       ViewPtr;

//includes
#include "node.h"
#include "light.h"
#include "log.h"
#include "render.h"
#include "renderable.h"
#include "scene.h"
#include "technique.h"
#include "utils.h"
#include "view.h"
#include "visitor.h"
#include "visual_model.h"

//include techinques
#include "techniques/for_each_light.h"

}

}

#endif
