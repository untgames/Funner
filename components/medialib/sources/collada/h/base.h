#include <media/collada.h>
#include <stl/string>

namespace medialib
{

class ColladaEntityImpl
{
  public:
    stl::string        id;
    static stl::string file_name;
    size_t             line;
    static ColladaImpl *collada_impl;
};

template <class Wrapper> struct ColladaImplTraits;
template <>              struct ColladaImplTraits<ColladaMesh>          { typedef ColladaMeshImpl          Impl; };
template <>              struct ColladaImplTraits<ColladaMaterial>      { typedef ColladaMaterialImpl      Impl; };
template <>              struct ColladaImplTraits<ColladaEffect>        { typedef ColladaEffectImpl        Impl; };
template <>              struct ColladaImplTraits<ColladaLight>         { typedef ColladaLightImpl         Impl; };
template <>              struct ColladaImplTraits<ColladaCamera>        { typedef ColladaCameraImpl        Impl; };
template <>              struct ColladaImplTraits<ColladaController>    { typedef ColladaControllerImpl    Impl; };
template <>              struct ColladaImplTraits<ColladaNode>          { typedef ColladaNodeImpl          Impl; };
template <>              struct ColladaImplTraits<ColladaVisualScene>   { typedef ColladaVisualSceneImpl   Impl; };
template <>              struct ColladaImplTraits<ColladaAnimation>     { typedef ColladaAnimationImpl     Impl; };
template <>              struct ColladaImplTraits<ColladaAnimationClip> { typedef ColladaAnimationClipImpl Impl; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Враппер для создания объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
class ColladaWrappers
{
  public:
    template <class Wrapper,class Impl> static Wrapper Create (Impl* impl) { return impl; }
};

}
