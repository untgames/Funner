#include "shared.h"

using namespace script;
using namespace math;

namespace script
{

/*
    Регистрация математической библиотеки
*/

void bind_math_library (InvokerRegistry& registry)
{
    //регистрация операций над векторами
    
  registry.Register ("vecLength",    make_invoker<float (vec3f)> (&math::length<float, 3>));
  registry.Register ("vecQLength",   make_invoker<float (vec3f)> (&math::qlen<float, 3>));
////  registry.Register ("vecNormalize", make_invoker<vec3f (vec3f)> (&math::normalize<float, 3>)); //сделать адаптер вызова
  registry.Register ("vecDot",       make_invoker<float (vec3f, vec3f)> (&math::dot<float, 3>));
////  registry.Register ("vecCross",     make_invoker<vec3f (vec3f, vec3f)> (&math::cross<float, 3>));//сделать адаптер вызова
////  registry.Register ("vecAngle",     make_invoker<float (vec3f)> (&math::angle<float, 3>));
  registry.Register ("vecAbs",       make_invoker<vec3f (vec3f)> (&math::abs<float, 3>));
  registry.Register ("vecMin",       make_invoker<vec3f (vec3f, vec3f)> (&math::min<float, 3>));
  registry.Register ("vecMax",       make_invoker<vec3f (vec3f, vec3f)> (&math::max<float, 3>));
}

}
